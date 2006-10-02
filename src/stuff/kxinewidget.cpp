/***************************************************************************
                           kxinewidget.cpp
		A KDE/QT API for the xine-lib. Define USE_QT_ONLY if you use QT only.
                           -------------------
    begin                : Fre Apr 18 2003
    revision             : $Revision: 1.48 $
    last modified        : $Date: 2005/08/22 19:36:46 $ by $Author: juergenk $
    copyright            : (C) 2003-2005 by Jürgen Kofler
    email                : kaffeine@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qwidget.h>
#include <qstringlist.h>

#include <qtimer.h>
#include <qevent.h>
#include <qthread.h>
#include <qdir.h>
#include <qcursor.h>
#include <qimage.h>
#include <qdatetime.h>
#include <qtextcodec.h>

#include <xine/xineutils.h>

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "kxinewidget.h"

#ifdef HAVE_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif

#ifndef USE_QT_ONLY
#include "kxinewidget.moc"
#include <klocale.h>
#include <kdebug.h>
#endif

#define TIMER_EVENT_PLAYBACK_FINISHED   100
#define TIMER_EVENT_NEW_CHANNELS        101
#define TIMER_EVENT_NEW_TITLE           102
#define TIMER_EVENT_NEW_STATUS          103
#define TIMER_EVENT_CHANGE_CURSOR       104
#define TIMER_EVENT_NEW_MRL_REFERENCE   105
#define TIMER_EVENT_NEW_XINE_MESSAGE    106
#define TIMER_EVENT_NEW_XINE_ERROR      107
#define TIMER_EVENT_FRAME_FORMAT_CHANGE 108
#define TIMER_EVENT_NEW_VOLUME_LEVEL    109
#define TIMER_EVENT_RESTART_PLAYBACK    200
#define TIMER_EVENT_RESIZE_PARENT       300


KXineWidget::KXineWidget(QWidget* parent, const char* name,
                         const QString& pathToConfigFile, const QString& pathToLogoFile,
                         const QString& audioDriver, const QString& videoDriver,
                         bool startManual, bool verbose)
		: JAMStreamPlayer(parent,name), QThread(), m_startXineManual(startManual), m_xineReady(false),
		m_logoFile(pathToLogoFile), m_preferedAudio(audioDriver), m_preferedVideo(videoDriver), m_xineVerbose(verbose),
		m_xineDisplay(NULL), m_xineEngine(NULL), m_audioDriver(NULL), m_videoDriver(NULL), m_xineStream(NULL),
		m_eventQueue(NULL), m_osd(NULL), m_osdUnscaled(false), m_osdShow(false), m_osdSize(0), m_osdFont(NULL),
		m_audioChoices(NULL), m_audioInfo(NULL), m_videoChoices(NULL), m_videoInfo(NULL), m_mixerInfo(NULL),
		m_osdShowInfo(NULL),
		m_osdSizeOptions(NULL), m_osdSizeInfo(NULL), m_osdFontInfo(NULL),
#ifndef USE_QT_ONLY
		m_videoFiltersEnabled(true), m_audioFiltersEnabled(true), m_deinterlaceFilter(NULL),
		m_deinterlaceEnabled(false),
		m_visualPlugin(NULL),
#else
		m_xinePost(NULL), m_postAudioSource(NULL), m_postInput(NULL),
#endif
		m_visualPluginName(QString::null), m_currentSpeed(Normal), m_softwareMixer(false), m_volumeGain(false),
		m_currentZoom(100), m_currentAudio(0), m_currentSub(0), m_savedPos(0), m_autoresizeEnabled(false)
{
	setMinimumSize(QSize(20,20)); // set a size hint
	setPaletteBackgroundColor(QColor(0,0,0)); //black

	/*  dvb  */
	TimeShiftFilename = "";
	dvbHaveVideo = 0;
	dvbOSD = 0;
	dvbColor[0] = 0;
	connect( &dvbOSDHideTimer, SIGNAL(timeout()), this, SLOT(dvbHideOSD()) );

	if (pathToConfigFile.isNull())
	{
		debugOut("Using default config file ~/.xine/config");
		m_configFilePath = QDir::homeDirPath();
		m_configFilePath.append("/.xine/config");
	}
	else
		m_configFilePath = pathToConfigFile;

	if (!m_logoFile.isNull())
		appendToQueue(m_logoFile);

#ifndef USE_QT_ONLY
	m_videoFilterList.setAutoDelete(true); /*** delete post plugin on removing from list ***/
	m_audioFilterList.setAutoDelete(true); /*** delete post plugin on removing from list ***/
#endif

	connect(&m_posTimer, SIGNAL(timeout()), this, SLOT(slotSendPosition()));
	connect(&m_lengthInfoTimer, SIGNAL(timeout()), this, SLOT(slotEmitLengthInfo()));
	connect(&m_mouseHideTimer, SIGNAL(timeout()), this, SLOT(slotHideMouse()));
	connect(&m_osdTimer, SIGNAL(timeout()), this, SLOT(slotOSDHide()));
	connect(&m_recentMessagesTimer, SIGNAL(timeout()), this, SLOT(slotNoRecentMessage()));

	setUpdatesEnabled(false);
	setMouseTracking(true);
}


KXineWidget::~KXineWidget()
{
	/* "careful" shutdown, maybe xine initialization was not successful */
	m_xineReady = false;

	/* stop all timers */
	m_posTimer.stop();
	m_mouseHideTimer.stop();

#ifndef USE_QT_ONLY
	slotRemoveAllAudioFilters();
	slotRemoveAllVideoFilters();
#endif
	if (m_osd)
		xine_osd_free(m_osd);

	if (m_xineStream)
		xine_close(m_xineStream);

	if (running())
	{
		XEvent ev;
		ev.type = Expose;
		ev.xexpose.display = m_xineDisplay;
		ev.xexpose.window = winId();
		ev.xexpose.x = x();
		ev.xexpose.y = y();
		ev.xexpose.width = width();
		ev.xexpose.height = height();
		ev.xexpose.count = 0;

		XSendEvent( x11Display(), winId(), False, ExposureMask, &ev );  /* send a fake expose event */
		XFlush(x11Display());

		if( !wait(1000) )  /* wait a second for thread exiting */
		{
			warningOut("XEvent thread don't exit. Terminating it...");
			terminate();
		}
	}

	debugOut("Shut down xine engine");

#ifndef USE_QT_ONLY
	if (m_deinterlaceFilter)
	{
		debugOut("Unwire video filters");
		unwireVideoFilters();
		delete m_deinterlaceFilter;
		m_deinterlaceFilter = NULL;
	}
	if (m_visualPlugin)
	{
		debugOut("Unwire audio filters");
		unwireAudioFilters();
		debugOut(QString("Dispose visual plugin: %1").arg(m_visualPluginName ));
		delete m_visualPlugin;
		m_visualPlugin = NULL;
	}
#else
	if (m_xinePost)
	{
		debugOut(QString("Dispose visual plugin: %1").arg(m_visualPluginName));
		m_postAudioSource = xine_get_audio_source(m_xineStream);
		xine_post_wire_audio_port(m_postAudioSource, m_audioDriver);
		xine_post_dispose(m_xineEngine, m_xinePost);
	}
#endif
	if (m_eventQueue)
	{
		debugOut("Dispose event queue");
		xine_event_dispose_queue(m_eventQueue);
	}
	if (m_xineStream)
	{
		debugOut("Dispose stream");
		xine_dispose(m_xineStream);
	}
	if (m_audioDriver)
	{
		debugOut("Close audio driver");
		xine_close_audio_driver(m_xineEngine, m_audioDriver);
	}
	if (m_videoDriver)
	{
		debugOut("Close video driver");
		xine_close_video_driver(m_xineEngine, m_videoDriver);
	}
	if (m_xineEngine)
	{
		saveXineConfig();
		debugOut("Close xine engine");
		xine_exit(m_xineEngine);
	}
	m_xineEngine = NULL;

	/* free xine config strings */
	if (m_osdShowInfo) free(m_osdShowInfo);

	if (m_osdFontInfo) free(m_osdFontInfo);
	if (m_osdFont) free(m_osdFont);

	if (m_osdSizeInfo) free(m_osdSizeInfo);
	if (m_osdSizeOptions)
	{
		int i=0;
		while (m_osdSizeOptions[i])
		{
			free(m_osdSizeOptions[i]);
			i++;
		}
		delete [] m_osdSizeOptions;
	}

	if (m_mixerInfo) free(m_mixerInfo);

	if (m_videoInfo) free(m_videoInfo);
	if (m_videoChoices)
	{
		int i=0;
		while (m_videoChoices[i])
		{
			free(m_videoChoices[i]);
			i++;
		}
		delete [] m_videoChoices;
	}

	if (m_audioInfo) free(m_audioInfo);
	if (m_audioChoices)
	{
		int i=0;
		while (m_audioChoices[i])
		{
			free(m_audioChoices[i]);
			i++;
		}
		delete [] m_audioChoices;
	}

	if (m_xineDisplay)
	{
		debugOut("Close xine display");
		XCloseDisplay(m_xineDisplay);  /* close xine display */
	}
	m_xineDisplay = NULL;

	debugOut("xine closed");
}


void KXineWidget::saveXineConfig()
{
	debugOut("Set CD/VCD/DVD path back");
	xine_cfg_entry_t config;

	if (!m_cachedCDPath.isNull())
	{
		xine_config_lookup_entry (m_xineEngine, "input.cdda_device", &config);
		config.str_value = (char*)m_cachedCDPath.latin1();
		xine_config_update_entry (m_xineEngine, &config);
	}

	if (!m_cachedVCDPath.isNull())
	{
		xine_config_lookup_entry (m_xineEngine, "input.vcd_device", &config);
		config.str_value = (char*)m_cachedVCDPath.latin1();
		xine_config_update_entry (m_xineEngine, &config);
	}

	if (!m_cachedDVDPath.isNull())
	{
		xine_config_lookup_entry (m_xineEngine, "input.dvd_device", &config);
		config.str_value = (char*)m_cachedDVDPath.latin1();
		xine_config_update_entry (m_xineEngine, &config);
	}

	debugOut(QString("Save xine config to: %1").arg(m_configFilePath));
	xine_config_save(m_xineEngine, m_configFilePath);
}


/***************************************************
 *         CALLBACKS
 ***************************************************/

void KXineWidget::destSizeCallback(void* p, int /*video_width*/, int /*video_height*/, double /*video_aspect*/,
                                   int* dest_width, int* dest_height, double* dest_aspect)

{
	if (p == NULL) return;
	KXineWidget* vw = (KXineWidget*) p;

	*dest_width = vw->width();
	*dest_height = vw->height();
	*dest_aspect = vw->m_displayRatio;
}


void KXineWidget::frameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                                      int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                                      double* dest_aspect, int* win_x, int* win_y)

{
	if (p == NULL) return;
	KXineWidget* vw = (KXineWidget*) p;

	*dest_x = 0;
	*dest_y = 0 ;
	*dest_width = vw->width();
	*dest_height = vw->height();
	*win_x = vw->m_globalX;
	*win_y = vw->m_globalY;
	*dest_aspect = vw->m_displayRatio;

        //debugOut(QString("Window size: %1x%2 - aspect ratio: %3").arg(*dest_width).arg(*dest_height).arg(*dest_aspect));

	/* correct size with video aspect */
	if (video_aspect >= vw->m_displayRatio)
		video_width  = (int) ( (double) (video_width * video_aspect / vw->m_displayRatio + 0.5) );
	else
		video_height = (int) ( (double) (video_height * vw->m_displayRatio / video_aspect) + 0.5);

	/* frame size changed */
	if ( (video_width != vw->m_videoFrameWidth) || (video_height != vw->m_videoFrameHeight) )
	{
		debugOut(QString("New video frame size: %1x%2 - aspect ratio: %3").arg(video_width).arg(video_height).arg(video_aspect));
		vw->m_videoFrameWidth = video_width;
		vw->m_videoFrameHeight = video_height;
		vw->m_videoAspect = video_aspect;
		QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_FRAME_FORMAT_CHANGE));

		/* auto-resize parent widget */
		if ((vw->m_autoresizeEnabled) && (vw->parentWidget()) && (vw->m_posTimer.isActive()) && (!vw->parentWidget()->isFullScreen())
		        && (video_width > 0) && (video_height > 0))
		{
			vw->m_newParentSize = vw->parentWidget()->size() - QSize((vw->width() - video_width), vw->height() - video_height);

			debugOut(QString("Resize video window to: %1x%2").arg(vw->m_newParentSize.width()).arg(vw->m_newParentSize.height()));

			/* we should not do a resize() inside a xine thread,
			   but post an event to the main thread */
			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_RESIZE_PARENT));
		}
	}
}


/*
 * XINE EVENT THREAD
 * only the QT event thread should do GUI operations,
 * we use QApplication::postEvent() and a reimplementation of QObject::timerEvent() to
 * make sure all critical jobs are done within the QT main thread context
 *
 * for more information see http://doc.trolltech.com/3.1/threads.html
 */

void KXineWidget::xineEventListener(void *p, const xine_event_t* xineEvent)
{

	if (p == NULL) return;
	KXineWidget* vw = (KXineWidget*) p;

	switch (xineEvent->type)
	{
		case XINE_EVENT_UI_PLAYBACK_FINISHED:
		{
			debugOut("xine event: playback finished");
			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_PLAYBACK_FINISHED ));
			break;
		}
		case XINE_EVENT_UI_CHANNELS_CHANGED:  /* new channel informations */
		{
			debugOut("xine event: channels changed");
			int i,j,channels;
			char* lang = new char[128];
			QString num;

			vw->m_audioCh.clear();
			vw->m_audioCh.append(i18n("auto"));
			vw->m_subCh.clear();
			vw->m_subCh.append(i18n("off"));

			/*** get audio channels  ***/
			channels = QMAX(10, xine_get_stream_info(vw->m_xineStream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL));
			for(i = 0; i < channels; i++)
			{
				if (xine_get_audio_lang(vw->m_xineStream, i, lang))
					vw->m_audioCh << lang;
				else
					vw->m_audioCh << i18n("Ch ") + num.setNum(i+1);
			}

			/*** get subtitle channels ***/

			channels = QMAX(10, xine_get_stream_info(vw->m_xineStream, XINE_STREAM_INFO_MAX_SPU_CHANNEL));
			for(j = 0; j < channels; j++)
			{
				if (xine_get_spu_lang(vw->m_xineStream, j, lang))
					vw->m_subCh << lang;
				else
					vw->m_subCh << i18n("Ch ") + num.setNum(j+1);
			}

			delete [] lang;

			vw->m_currentAudio = xine_get_param(vw->m_xineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL) + 1;
			vw->m_currentSub = xine_get_param(vw->m_xineStream, XINE_PARAM_SPU_CHANNEL) + 1;

			if (vw->m_currentAudio > (i-1))
			{
				vw->slotSetAudioChannel(0);
				vw->m_currentAudio = -1;
			}

			if (vw->m_currentSub > (j-1))
			{
				vw->slotSetSubtitleChannel(0);
				vw->m_currentSub = -1;
			}

			//check if stream is seekable
			vw->m_trackIsSeekable = (bool)xine_get_stream_info(vw->m_xineStream, XINE_STREAM_INFO_SEEKABLE);

			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_CHANNELS));
			break;
		}
		case XINE_EVENT_UI_SET_TITLE:  /* set new title */
		{
			debugOut("xine event: ui set title");
			xine_ui_data_t* xd = (xine_ui_data_t*)xineEvent->data;
			vw->m_trackTitle = QString::fromLocal8Bit( (char*)xd->str );

			vw->m_lengthInfoTries = 0;
			vw->m_lengthInfoTimer.start(1000); /* May be new Length on Changing DVD/VCD titles */
			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_TITLE));
			break;
		}
		case XINE_EVENT_PROGRESS:
		{
			debugOut("xine event: progress info");
			xine_progress_data_t* pd = (xine_progress_data_t*)xineEvent->data;

			vw->m_statusString = QString::fromLocal8Bit(pd->description) + " " + QString::number(pd->percent) + "%";

			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_STATUS));
			break;
		}
		case XINE_EVENT_DROPPED_FRAMES:
		{
			debugOut("xine event: dropped frames");
			xine_dropped_frames_t* dropped = (xine_dropped_frames_t*)xineEvent->data;

			warningOut(QString("Skipped frames: %1 - discarded frames: %2").arg(dropped->skipped_frames/10).arg(dropped->discarded_frames/10));

			break;
		}
		case XINE_EVENT_SPU_BUTTON:
		{
			debugOut("xine event: spu button");
			xine_spu_button_t* button = (xine_spu_button_t*)xineEvent->data;

			if (button->direction == 1) /* enter a button */
			{
				debugOut("DVD Menu: Mouse entered button");
				vw->m_DVDButtonEntered = true;
			}
			else
			{
				debugOut("DVD Menu: Mouse leaved button");
				vw->m_DVDButtonEntered = false;
			}

			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_CHANGE_CURSOR));
			break;
		}
		case XINE_EVENT_UI_NUM_BUTTONS:
		{
			debugOut("xine event: ui num buttons");

			break;
		}
		case XINE_EVENT_MRL_REFERENCE:
		{
			debugOut("xine event: mrl reference");
			xine_mrl_reference_data_t* mrldata = (xine_mrl_reference_data_t*)xineEvent->data;
			vw->m_newMRLReference = mrldata->mrl;

			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_MRL_REFERENCE));
			break;
		}
		case XINE_EVENT_FRAME_FORMAT_CHANGE:
		{
			// debugOut("xine event: frame format change");

			// QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_FRAME_FORMAT_CHANGE));
			break;
		}
		case XINE_EVENT_AUDIO_LEVEL:
		{
			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_VOLUME_LEVEL));
			break;
		}
		case XINE_EVENT_UI_MESSAGE:
		{
			debugOut("xine event: xine message");

			xine_ui_message_data_t *data = (xine_ui_message_data_t *)xineEvent->data;
			QString message;

			switch(data->type)
			{
				case XINE_MSG_NO_ERROR:
				{
					/* copy strings, and replace '\0' separators by '\n' */
					char* s = data->messages;
					char* d = new char[2000];

					while(s && (*s != '\0') && ((*s + 1) != '\0'))
					{
						switch(*s)
						{
							case '\0':
							{
								*d = '\n';
								break;
							}
							default:
							{
								*d = *s;
								break;
							}
						}
						s++;
						d++;
					}
					*++d = '\0';

					message = d;
					delete [] d;
					break;
				}
				case XINE_MSG_GENERAL_WARNING:
				{
					message = i18n("General Warning: \n");

					if(data->explanation)
						message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);
					else
						message = message + i18n("No Informations available.");

					break;
				}
				case XINE_MSG_SECURITY:
				{
					message = i18n("Security Warning: \n");

					if(data->explanation)
						message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);

					break;
				}
				case XINE_MSG_UNKNOWN_HOST:
				{
					message = i18n("The host you're trying to connect is unknown.\nCheck the validity of the specified hostname. ");
					if(data->explanation)
						message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_UNKNOWN_DEVICE:
				{
					message = i18n("The device name you specified seems invalid. ");
					if(data->explanation)
						message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_NETWORK_UNREACHABLE:
				{
					message = i18n("The network looks unreachable.\nCheck your network setup and the server name. ");
					if(data->explanation)
						message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_AUDIO_OUT_UNAVAILABLE:
				{
					message = i18n("Audio output unavailable. Device is busy. ");
					if(data->explanation)
						message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_CONNECTION_REFUSED:
				{
					message = i18n("The connection was refused.\nCheck the host name. ");
					if(data->explanation)
						message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_FILE_NOT_FOUND:
				{
					message = i18n("The specified file or url was not found. Please check it. ");
					if(data->explanation)
						message = message + "(" + QString::fromLocal8Bit((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_PERMISSION_ERROR:
				{
					message = i18n("Permission to this source was denied. ");
					// if(data->explanation)
					message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_READ_ERROR:
				{
					message = i18n("The source can't be read.\nMaybe you don't have enough rights for this, or source doesn't contain data (e.g: no disc in drive). ");
					if(data->explanation)
						message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				case XINE_MSG_LIBRARY_LOAD_ERROR:
				{
					message = i18n("A problem occur while loading a library or a decoder: ");
					if(data->explanation)
						message = message + ((char *) data + data->parameters);
					break;
				}
				case XINE_MSG_ENCRYPTED_SOURCE:
				{
					message = i18n("The source seems encrypted, and can't be read. ");
					if (vw->m_trackURL.contains("dvd:/"))
						message = message + i18n("\nYour DVD is probably crypted. According to your country laws, you can or can't use libdvdcss to be able to read this disc. ");
					if(data->explanation)
						message = message + "(" + ((char *) data + data->parameters) + ")";
					break;
				}
				default:
				{
					message = i18n("Unknown error: \n");
					if(data->explanation)
						message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);
					break;
				}
			}

			vw->m_xineMessage = message;
			QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_XINE_MESSAGE));
			break;
		}
		default:
		{
			//debugOut("xine event: unhandled type ");
			break;
		}
	}
}

void KXineWidget::timerEvent( QTimerEvent* tevent )
{
	switch ( tevent->timerId() )
	{
		case TIMER_EVENT_PLAYBACK_FINISHED:
		{
			if ( !TimeShiftFilename.isEmpty() )
			{
				QTimer::singleShot(0, this, SLOT(slotPlayTimeShift()));
				break;
			}
			if ( m_trackURL=="DVB" || m_trackURL.contains(".kaxtv.ts") )
				break;
			if (isQueueEmpty())
			{
				if (m_trackURL != m_logoFile)
					emit signalPlaybackFinished();
				else
					xine_stop(m_xineStream);
			}
			else
				QTimer::singleShot(0, this, SLOT(slotPlay()));
			break;
		}
		case TIMER_EVENT_NEW_CHANNELS:
		{
			emit signalNewChannels(m_audioCh, m_subCh, m_currentAudio, m_currentSub);
			break;
		}
		case TIMER_EVENT_NEW_TITLE:
		{
			emit signalTitleChanged();
			break;
		}
		case TIMER_EVENT_FRAME_FORMAT_CHANGE:
		{
			if ((m_trackHasVideo) && (m_trackURL != m_logoFile))
				emit signalVideoSizeChanged();
			break;
		}
		case TIMER_EVENT_NEW_STATUS:
		{
			emit signalXineStatus(m_statusString);
			break;
		}
		case TIMER_EVENT_CHANGE_CURSOR:
		{
			if (m_DVDButtonEntered)
				setCursor(QCursor(Qt::PointingHandCursor));
			else
				setCursor(QCursor(Qt::ArrowCursor));
			break;
		}
		case TIMER_EVENT_NEW_MRL_REFERENCE:
		{
			m_queue.prepend(m_newMRLReference );
			break;
		}
		case TIMER_EVENT_NEW_VOLUME_LEVEL:
		{
			emit signalSyncVolume();
			break;
		}
		case TIMER_EVENT_NEW_XINE_MESSAGE:
		{
			if (!m_recentMessagesTimer.isActive())
			{
				m_recentMessagesTimer.start(1500);
				emit signalXineMessage(m_xineMessage);
			}
			else
			{
				//restart
				warningOut(QString("Message: '%1' was blocked!").arg(m_xineMessage));
				m_recentMessagesTimer.start(1500);
			}
			break;
		}
		case TIMER_EVENT_NEW_XINE_ERROR:
		{
			emit signalXineError(m_xineError);
			break;
		}
		case TIMER_EVENT_RESTART_PLAYBACK:
		{
			appendToQueue(m_trackURL);
			slotPlay();
			break;
		}
		case TIMER_EVENT_RESIZE_PARENT:
		{
			parentWidget()->resize(m_newParentSize);
			break;
		}
		default: break;
	}
}

void KXineWidget::slotNoRecentMessage()
{
	m_recentMessagesTimer.stop();
}

/******************* new video driver *********************/

void KXineWidget::videoDriverChangedCallback(void* p, xine_cfg_entry_t* entry)
{
	if (p == NULL) return;
	if (entry == NULL) return;
#ifndef USE_QT_ONLY
	KXineWidget* vw = (KXineWidget*) p;
	xine_video_port_t* oldVideoDriver = vw->m_videoDriver;
	xine_video_port_t* noneVideoDriver;

	int pos, time, length;

	debugOut(QString("New video driver: %1").arg(entry->enum_values[entry->num_value]));

	if (vw->m_osd)
	{
		xine_osd_free(vw->m_osd);
		vw->m_osd = NULL;
	}

	noneVideoDriver = xine_open_video_driver(vw->m_xineEngine, "none",
	                  XINE_VISUAL_TYPE_NONE, NULL);
	if (!noneVideoDriver)
	{
		errorOut("Can't init Video Driver 'none', operation aborted.");
		return;
	}

	bool playing = false;
	if (vw->isPlaying())
	{
		playing = true;
		vw->m_savedPos = 0;

		int t = 0, ret = 0;
		while(((ret = xine_get_pos_length(vw->m_xineStream, &pos, &time, &length)) == 0) && (++t < 5))
			xine_usec_sleep(100000);

		if ( ret != 0 )
			vw->m_savedPos = pos;
	}

	xine_close(vw->m_xineStream);

	/* wire filters to "none" driver so the old one can be safely disposed */
	vw->m_videoDriver = noneVideoDriver;
	vw->unwireVideoFilters();
	vw->wireVideoFilters();

	vw->unwireAudioFilters();
	if (vw->m_visualPlugin)
	{
		debugOut(QString("Dispose visual plugin: %1").arg(vw->m_visualPluginName));
		delete vw->m_visualPlugin;
		vw->m_visualPlugin = NULL;
	}

	xine_event_dispose_queue(vw->m_eventQueue);
	xine_dispose(vw->m_xineStream);

	xine_close_video_driver(vw->m_xineEngine, oldVideoDriver);

	vw->m_videoDriver = xine_open_video_driver(vw->m_xineEngine,
	                    entry->enum_values[entry->num_value], XINE_VISUAL_TYPE_X11,
	                    (void *) &(vw->m_x11Visual));

	if (!vw->m_videoDriver)
	{
		vw->m_xineError = i18n("Error: Can't init new Video Driver %1 - using %2!").arg(entry->enum_values[entry->num_value]).arg(vw->m_videoDriverName);
		QApplication::postEvent(vw, new QTimerEvent( TIMER_EVENT_NEW_XINE_ERROR));
		playing = false;
		vw->m_videoDriver = xine_open_video_driver(vw->m_xineEngine,
		                    vw->m_videoDriverName, XINE_VISUAL_TYPE_X11,
		                    (void *) &(vw->m_x11Visual));
	}
	else
	{
		vw->m_videoDriverName = entry->enum_values[entry->num_value];
		vw->m_statusString = i18n("Using Video Driver: ") + vw->m_videoDriverName;
		QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_STATUS));
	}

	vw->m_xineStream = xine_stream_new(vw->m_xineEngine, vw->m_audioDriver, vw->m_videoDriver);
	vw->m_eventQueue = xine_event_new_queue (vw->m_xineStream);
	xine_event_create_listener_thread(vw->m_eventQueue, &KXineWidget::xineEventListener, p);

	/* rewire filters to the new driver */
	vw->unwireVideoFilters();
	vw->wireVideoFilters();

	/* "none" can now be disposed too */
	xine_close_video_driver(vw->m_xineEngine, noneVideoDriver);

	vw->initOSD();

	if (playing)
		QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_RESTART_PLAYBACK));
#endif
}

/*********************** new audio driver *************************/

void KXineWidget::audioDriverChangedCallback(void* p, xine_cfg_entry_t* entry)
{
	if (p == NULL) return;
	if (entry == NULL) return;
#ifndef USE_QT_ONLY
	KXineWidget* vw = (KXineWidget*) p;

	int pos, time, length;

	debugOut(QString("New audio driver: %1").arg(entry->enum_values[entry->num_value]));

	if (vw->m_osd)
	{
		xine_osd_free(vw->m_osd);
		vw->m_osd = NULL;
	}

	vw->unwireVideoFilters();

	bool playing = false;
	if (vw->isPlaying())
	{
		playing = true;
		vw->m_savedPos = 0;

		int t = 0, ret = 0;
		while(((ret = xine_get_pos_length(vw->m_xineStream, &pos, &time, &length)) == 0) && (++t < 5))
			xine_usec_sleep(100000);

		if ( ret != 0 )
			vw->m_savedPos = pos;
	}

	xine_close(vw->m_xineStream);

	vw->unwireAudioFilters();
	if (vw->m_visualPlugin)
	{
		debugOut(QString("Dispose visual plugin: %1").arg(vw->m_visualPluginName));
		delete vw->m_visualPlugin;
		vw->m_visualPlugin = NULL;
	}

	xine_event_dispose_queue(vw->m_eventQueue);
	xine_dispose(vw->m_xineStream);
	xine_close_audio_driver(vw->m_xineEngine, vw->m_audioDriver);
	vw->m_audioDriver = NULL;

	vw->m_audioDriver = xine_open_audio_driver(vw->m_xineEngine, entry->enum_values[entry->num_value], NULL);

	if (!vw->m_audioDriver)
	{
		vw->m_xineError = i18n("Error: Can't init new Audio Driver %1 - using %2!").arg(entry->enum_values[entry->num_value]).arg(vw->m_audioDriverName);
		QApplication::postEvent(vw, new QTimerEvent( TIMER_EVENT_NEW_XINE_ERROR));
		playing = false;
		vw->m_audioDriver = xine_open_audio_driver(vw->m_xineEngine, vw->m_audioDriverName, NULL);
	}
	else
	{
		vw->m_audioDriverName = entry->enum_values[entry->num_value];
		vw->m_statusString = i18n("Using Audio Driver: ") + vw->m_audioDriverName;
		QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_NEW_STATUS));
	}

	vw->m_xineStream = xine_stream_new(vw->m_xineEngine, vw->m_audioDriver, vw->m_videoDriver);
	vw->m_eventQueue = xine_event_new_queue (vw->m_xineStream);
	xine_event_create_listener_thread(vw->m_eventQueue, &KXineWidget::xineEventListener, p);

	vw->wireVideoFilters();

	vw->initOSD();

	if (playing)
		QApplication::postEvent(vw, new QTimerEvent(TIMER_EVENT_RESTART_PLAYBACK));
#endif
}

/******** change audio mixer method ****************/
void KXineWidget::audioMixerMethodChangedCallback(void* p, xine_cfg_entry_t* entry)
{
	if (p == NULL) return;
	KXineWidget* vw = (KXineWidget*) p;

	vw->m_softwareMixer = (bool)entry->num_value;
}

/******** Callback for OSD configuration ****************/
void KXineWidget::showOSDMessagesChangedCallback(void* p, xine_cfg_entry_t* entry)
{
	if (p == NULL) return;
	KXineWidget* vw = (KXineWidget*) p;

	if (vw->m_osd)
		vw->m_osdShow = (bool)entry->num_value;
}

void KXineWidget::sizeForOSDMessagesChangedCallback(void* p, xine_cfg_entry_t* entry)
{
	if (p == NULL) return;
	KXineWidget* vw = (KXineWidget*) p;

	const int fontsizetable[] = { 16,20,24,32,48,64 };

	if (entry->num_value >= 6)
	{
		debugOut("Font size not defined : Shouldn't have happened");
		return;
	}

	if (vw->m_osd)
	{
		vw->m_osdSize = entry->num_value;
		xine_osd_set_font(vw->m_osd, vw->m_osdFont, fontsizetable[vw->m_osdSize]);
	}
}

void KXineWidget::fontForOSDMessagesChangedCallback(void* p, xine_cfg_entry_t* entry)
{
	if (p == NULL) return;
	KXineWidget* vw = (KXineWidget*) p;

	const int fontsizetable[] = { 16,20,24,32,48,64 };

	if (vw->m_osd)
		if (entry->str_value)
		{
			free(vw->m_osdFont);
			vw->m_osdFont = strdup(entry->str_value);
			if (!xine_osd_set_font(vw->m_osd, vw->m_osdFont, fontsizetable[vw->m_osdSize]))
			{
				free(vw->m_osdFont);
				vw->m_osdFont = strdup("sans");
				if (!xine_osd_set_font(vw->m_osd, vw->m_osdFont, fontsizetable[vw->m_osdSize]))
					warningOut("Default SANS font not found : shouldn't have happened.");
			}
		}
}

/**********************************************
 *      EVENT LOOP
 *********************************************/

void KXineWidget::run()
{
	debugOut("Start event loop...");

	XEvent event;

	while (isXineReady())
	{
		XNextEvent (m_xineDisplay, &event);
		XLockDisplay(m_xineDisplay);

		if (event.type == Expose)
		{
			if (event.xexpose.count == 0)
			{
				xine_port_send_gui_data(m_videoDriver, XINE_GUI_SEND_EXPOSE_EVENT, &event);
			}
		}

		XUnlockDisplay(m_xineDisplay);
	}

	debugOut("Exiting event loop...");
}

/**********************************************************
 *           INIT XINE ENGINE
 *********************************************************/

void KXineWidget::polish()
{
	if ((!m_startXineManual) && (!isXineReady()))  /* start xine engine automatically? */
	{
		initXine();
	}
}

bool KXineWidget::initXine()
{
	if (isXineReady())
		return true;

	emit signalXineStatus(i18n("Init xine..."));
	globalPosChanged(); /* get global pos of the window */

	/**** INIT XINE DISPLAY  ****/

	XInitThreads();

	m_xineDisplay = XOpenDisplay( getenv("DISPLAY") );

	if (!m_xineDisplay)
	{
		emit signalXineFatal(i18n("Failed to connect to X-Server!"));
		return false;
	}

	m_xineScreen = DefaultScreen(m_xineDisplay);
	m_xineWindow = winId();

	XLockDisplay(m_xineDisplay);

	XSelectInput(m_xineDisplay, m_xineWindow, ExposureMask);


	/* determine display aspect ratio  */
	double resHor = DisplayWidth(m_xineDisplay, m_xineScreen)*1000 / DisplayWidthMM(m_xineDisplay, m_xineScreen);
	double resVer = DisplayHeight(m_xineDisplay, m_xineScreen)*1000 / DisplayHeightMM(m_xineDisplay, m_xineScreen);

	m_displayRatio = resVer / resHor;

#ifdef HAVE_XINERAMA
	int dummy_event, dummy_error;

	if (XineramaQueryExtension(m_xineDisplay, &dummy_event, &dummy_error))
	{
		int count = 1;
		debugOut("Xinerama extension present");
		XineramaQueryScreens(m_xineDisplay, &count);
		debugOut(QString("%1 screens detected").arg(count));
		if (count > 1)
			/* multihead -> assuming square pixels */
			m_displayRatio = 1.0;
	}
#endif

	debugOut(QString("Display aspect ratio (v/h): %1").arg(m_displayRatio));

	XUnlockDisplay(m_xineDisplay);

	/**** INIT XINE ENGINE ****/

	debugOut(QString("Using xine version %1").arg(xine_get_version_string()));

	m_xineEngine = xine_new();
	if (!m_xineEngine)
	{
		emit signalXineFatal(i18n("Can't init xine Engine!"));
		return false;
	}

	if (m_xineVerbose)
		xine_engine_set_param(m_xineEngine, XINE_ENGINE_PARAM_VERBOSITY, 99);

	/* load configuration */

	if (!QFile::exists(m_configFilePath))
		warningOut("No config file found, will create one...");
	else
		xine_config_load(m_xineEngine, m_configFilePath);


	debugOut("Post-init xine engine");
	xine_init(m_xineEngine);

	/** set xine parameters **/

	const char* const* drivers = NULL;
	drivers = xine_list_audio_output_plugins(m_xineEngine);
	int i = 0;
	while (drivers[i] != NULL) i++;
	m_audioChoices = new char*[i+2];
	m_audioChoices[0] = strdup("auto");
	m_audioDriverList << m_audioChoices[0];
	i = 0;
	while(drivers[i])
	{
		m_audioChoices[i+1] = strdup(drivers[i]);
		m_audioDriverList << m_audioChoices[i+1];
		i++;
	}
	m_audioChoices[i+1] = NULL;

	m_audioInfo = strdup(i18n("Audiodriver to use (default: auto)").utf8());
	i = xine_config_register_enum(m_xineEngine, "audio.driver", 0,
	                              m_audioChoices, m_audioInfo, NULL, 10, &KXineWidget::audioDriverChangedCallback, this);

	if (m_audioDriverList.contains(m_preferedAudio))
		m_audioDriverName = m_preferedAudio;
	else
		m_audioDriverName = m_audioChoices[i];

	debugOut(QString("Use audio driver %1").arg(m_audioDriverName));

	drivers = xine_list_video_output_plugins(m_xineEngine);
	i = 0;
	while (drivers[i] != NULL) i++;
	m_videoChoices = new char*[i+2];
	m_videoChoices[0] = strdup("auto");
	m_videoDriverList << m_videoChoices[0];
	i = 0;
	while(drivers[i])
	{
		m_videoChoices[i+1] = strdup(drivers[i]);
		m_videoDriverList << m_videoChoices[i+1];
		i++;
	}
	m_videoChoices[i+1] = NULL;

	m_videoInfo = strdup(i18n("Videodriver to use (default: auto)").utf8());
	i = xine_config_register_enum(m_xineEngine, "video.driver", 0,
	                              m_videoChoices, m_videoInfo, NULL, 10, &KXineWidget::videoDriverChangedCallback, this);

	if (m_videoDriverList.contains(m_preferedVideo))
		m_videoDriverName = m_preferedVideo;
	else
		m_videoDriverName = m_videoChoices[i];

	debugOut(QString("Use video driver %1").arg(m_videoDriverName));

	m_mixerInfo = strdup(i18n("Use software audio mixer").utf8());
	m_softwareMixer = (bool)xine_config_register_bool(m_xineEngine, "audio.mixer_software", 1, m_mixerInfo,
	                  NULL, 10, &KXineWidget::audioMixerMethodChangedCallback, this);

	m_osdShowInfo = strdup(i18n("Show OSD Messages").utf8());
	m_osdShow = (bool)xine_config_register_bool(m_xineEngine, "osd.osd_messages", 1, m_osdShowInfo,
	            NULL, 10, &KXineWidget::showOSDMessagesChangedCallback, this);

	m_osdSizeOptions = new char*[7];
	m_osdSizeOptions[0] = strdup("tiny");
	m_osdSizeOptions[1] = strdup("small");
	m_osdSizeOptions[2] = strdup("medium");
	m_osdSizeOptions[3] = strdup("large");
	m_osdSizeOptions[4] = strdup("very large");
	m_osdSizeOptions[5] = strdup("huge");
	m_osdSizeOptions[6] = NULL;

	m_osdSizeInfo = strdup(i18n("Size of OSD text").utf8());
	m_osdSize = (int)xine_config_register_enum(m_xineEngine, "osd.osd_size", 1 /*small - 20P*/, m_osdSizeOptions, m_osdSizeInfo,
	            NULL, 10, &KXineWidget::sizeForOSDMessagesChangedCallback, this);

	m_osdFontInfo = strdup(i18n("Font for OSD Messages").utf8());
	m_osdFont = strdup((char*)xine_config_register_string(m_xineEngine, "osd.osd_font", "sans", m_osdFontInfo,
	                   NULL, 10, &KXineWidget::fontForOSDMessagesChangedCallback, this));

	/* init video driver */
	debugOut("Init video driver");

	m_x11Visual.display          = m_xineDisplay;
	m_x11Visual.screen           = m_xineScreen;
	m_x11Visual.d                = m_xineWindow;
	m_x11Visual.dest_size_cb     = &KXineWidget::destSizeCallback;
	m_x11Visual.frame_output_cb  = &KXineWidget::frameOutputCallback;
	m_x11Visual.user_data        = (void*)this;

	m_videoDriver = xine_open_video_driver(m_xineEngine,
	                                       m_videoDriverName,  XINE_VISUAL_TYPE_X11,
	                                       (void *) &(m_x11Visual));

	if (!m_videoDriver && m_videoDriverName != "auto")
	{
		emit signalXineError(i18n("Can't init Video Driver '%1' - trying 'auto'...").arg(m_videoDriverName));
		m_videoDriverName = "auto";
		m_videoDriver = xine_open_video_driver(m_xineEngine,
		                                       m_videoDriverName,  XINE_VISUAL_TYPE_X11,
		                                       (void *) &(m_x11Visual));
	}

	if (!m_videoDriver)
	{
		emit signalXineFatal(i18n("All Video Drivers failed to initialize!"));
		return false;
	}

	/* init audio driver */
	debugOut("Init audio driver");

	m_audioDriver = xine_open_audio_driver(m_xineEngine, m_audioDriverName, NULL);

	if (!m_audioDriver && m_audioDriverName != "auto")
	{
		emit signalXineError(i18n("Can't init Audio Driver '%1' - trying 'auto'...").arg(m_audioDriverName));
		m_audioDriverName = "auto";
		m_audioDriver = xine_open_audio_driver (m_xineEngine, m_audioDriverName, NULL);
	}

	if (!m_audioDriver)
	{
		emit signalXineFatal(i18n("All Audio Drivers failed to initialize!"));
		return false;
	}

	//debugOut("Open xine stream");

	m_xineStream  = xine_stream_new(m_xineEngine, m_audioDriver, m_videoDriver);
	if (!m_xineStream)
	{
		emit signalXineFatal(i18n("Can't create a new xine Stream!"));
		return false;
	}

	/*** OSD ***/

	initOSD();

	/** event handling **/

	m_eventQueue = xine_event_new_queue (m_xineStream);
	xine_event_create_listener_thread(m_eventQueue, &KXineWidget::xineEventListener, (void*)this);

	//maybe user closed player in muted state
	if (m_softwareMixer)
		xine_set_param(m_xineStream, XINE_PARAM_AUDIO_AMP_MUTE, 0);
	else
		xine_set_param(m_xineStream, XINE_PARAM_AUDIO_MUTE, 0);

	m_xineReady = true;
	start();

	debugOut("xine init successful");

	emit signalXineStatus(i18n("Ready"));
	emit signalXineReady();

	/** something to play? **/
	slotPlay();

	return true;
}

void KXineWidget::initOSD()
{
	debugOut("Init OSD");
	const int fontsizetable[] = { 16,20,24,32,48,64 };
	m_osd = xine_osd_new(m_xineStream, 10, 10, 1000, 200);
	if (m_osd)
	{
		if (!xine_osd_set_font(m_osd, m_osdFont, fontsizetable[m_osdSize]))
		{
			debugOut(QString("Font ->%1<- specified for OSD doesn't exists.").arg(m_osdFont));
			free(m_osdFont);
			m_osdFont = strdup("sans");
			xine_osd_set_font(m_osd, m_osdFont, fontsizetable[m_osdSize]);
		}
		debugOut(QString("Font for OSD: %1").arg(m_osdFont));
		xine_osd_set_text_palette(m_osd, XINE_TEXTPALETTE_WHITE_BLACK_TRANSPARENT, XINE_OSD_TEXT1);
		m_osdUnscaled = (xine_osd_get_capabilities(m_osd) & XINE_OSD_CAP_UNSCALED);
		if (m_osdUnscaled)
			debugOut("Unscaled OSD available");
	}
	else
		warningOut("Initialisation of xine OSD failed.");
}

/************************************************
 *              PLAY MRL
 ************************************************/

void KXineWidget::play(QString mrl)
{
    clearQueue();
    appendToQueue(mrl);
    slotPlay();
}


bool KXineWidget::playDvb()
{
#ifndef USE_QT_ONLY
	unwireAudioFilters();

	QPtrList<PostFilter> activeList;

	if (m_audioFilterList.count() && m_audioFiltersEnabled)
		activeList = m_audioFilterList;

	if ( !dvbHaveVideo )
	{
		if (!m_visualPlugin)
		{
			debugOut(QString("Init visual plugin: %1").arg(m_visualPluginName));
			m_visualPlugin = new PostFilter(m_visualPluginName, m_xineEngine, m_audioDriver, m_videoDriver, NULL);
		}

		activeList.insert (0, m_visualPlugin);
	}
	else
	{
		if (m_visualPlugin)
		{
			debugOut(QString("Dispose visual plugin: %1").arg(m_visualPluginName));
			delete m_visualPlugin;
			m_visualPlugin = NULL;
		}
	}

	if (activeList.count())
	{
		xine_post_wire_audio_port(activeList.at(activeList.count()-1)->getOutput(), m_audioDriver);

		for (uint i = activeList.count()-1; i >0; i--)
		{
			xine_post_wire(activeList.at(i-1)->getOutput(), activeList.at(i)->getInput());
		}

		xine_post_wire( xine_get_audio_source(m_xineStream), activeList.at(0)->getInput());
	}
#endif

	if (!xine_play(m_xineStream, 0,0))
	{
		sendXineError();
		return false;
	}

	m_currentSpeed = Normal;
	m_trackHasChapters = false;
	m_trackArtist = QString::null;
	m_trackAlbum = QString::null;
	m_trackNumber = QString::null;
	m_trackYear = QString::null;
	m_trackComment = QString::null;

	m_trackIsSeekable = false;

	if ( !dvbHaveVideo ) m_trackHasVideo = false;
	else m_trackHasVideo = (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_VIDEO);
	if (m_trackHasVideo)
	{
		m_trackVideoCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_VIDEOCODEC);
		m_videoFrameWidth = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_WIDTH);
		m_videoFrameHeight = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_HEIGHT);
		m_trackVideoBitrate = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_BITRATE);
	}
	else
	{
		m_trackVideoCodec = QString::null;
		m_videoFrameWidth = 0;
		m_videoFrameHeight = 0;
		m_trackVideoBitrate = 0;
	}

	m_trackHasAudio = (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_AUDIO);
	if (m_trackHasAudio)
	{
		m_trackAudioCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_AUDIOCODEC);
		m_trackAudioBitrate = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_AUDIO_BITRATE);
	}
	else
	{
		m_trackAudioCodec = QString::null;
		m_trackAudioBitrate = 0;
	}

	m_trackLength = getLengthInfo();

	slotSetAudioChannel(0); //refresh channel info
	m_posTimer.start(1000);

	emit signalXinePlaying();
	emit signalXineStatus(i18n("Playing"));

	return true;
}

unsigned int KXineWidget::rgb2yuv( unsigned int R, unsigned int G, unsigned int B )
{
	return ((((((66*R+129*G+25*B+128)>>8)+16)<<8)|(((112*R-94*G-18*B+128)>>8)+128))<<8|(((-38*R-74*G+112*B+128)>>8)+128));
}

void KXineWidget::initDvbPalette()
{
	if ( dvbColor[0] ) return;

	memset( dvbColor, 0, sizeof(dvbColor) );
	memset( dvbTrans, 0, sizeof(dvbTrans) );
	dvbColor[0]=1;

	unsigned int blueText[11] = {
	                                rgb2yuv(0,0,0), /* 0 : not used */
	                                rgb2yuv(0,0,0), /* 1 : font bg */
	                                rgb2yuv(10,50,40), /* 2 : transition bg->border */
	                                rgb2yuv(30,100,85), /* 3 */
	                                rgb2yuv(50,150,130), /* 4 */
	                                rgb2yuv(70,200,175), /* 5 */
	                                rgb2yuv(90,255,220), /* 6 : border */
	                                rgb2yuv(90,255,220), /* 7 : transition border->fg */
	                                rgb2yuv(90,255,220), /* 8 */
	                                rgb2yuv(90,255,220), /* 9 */
	                                rgb2yuv(90,255,220), /* 10 : font fg */
	                            };
	unsigned int whiteText[11] = {
	                                 rgb2yuv(0,0,0),
	                                 rgb2yuv(0,0,0),
	                                 rgb2yuv(50,50,50),
	                                 rgb2yuv(100,100,100),
	                                 rgb2yuv(150,150,150),
	                                 rgb2yuv(200,200,200),
	                                 rgb2yuv(255,255,255),
	                                 rgb2yuv(255,255,255),
	                                 rgb2yuv(255,255,255),
	                                 rgb2yuv(255,255,255),
	                                 rgb2yuv(255,255,255),
	                             };
	unsigned int greenText[11] = {
	                                 rgb2yuv(0,0,0),
	                                 rgb2yuv(0,0,0),
	                                 rgb2yuv(30,50,30),
	                                 rgb2yuv(60,100,30),
	                                 rgb2yuv(90,150,90),
	                                 rgb2yuv(120,200,120),
	                                 rgb2yuv(150,255,150),
	                                 rgb2yuv(150,255,150),
	                                 rgb2yuv(150,255,150),
	                                 rgb2yuv(150,255,150),
	                                 rgb2yuv(150,255,150),
	                             };
	unsigned char textAlpha[11] = { 0, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15, };
#define DVB_TEXT_WHITE 100
#define DVB_TEXT_BLUE 111
#define DVB_TEXT_GREEN 122
	int a;
	for ( a=DVB_TEXT_BLUE; a<DVB_TEXT_BLUE+11; a++ )
	{
		dvbColor[a]=blueText[a-DVB_TEXT_BLUE];
		dvbTrans[a]=textAlpha[a-DVB_TEXT_BLUE];
	}
	for ( a=DVB_TEXT_GREEN; a<DVB_TEXT_GREEN+11; a++ )
	{
		dvbColor[a]=greenText[a-DVB_TEXT_GREEN];
		dvbTrans[a]=textAlpha[a-DVB_TEXT_GREEN];
	}
	for ( a=DVB_TEXT_WHITE; a<DVB_TEXT_WHITE+11; a++ )
	{
		dvbColor[a]=whiteText[a-DVB_TEXT_WHITE];
		dvbTrans[a]=textAlpha[a-DVB_TEXT_WHITE];
	}
#define DVB_COLOR_RED 200
	dvbColor[DVB_COLOR_RED] = rgb2yuv(255,0,0); dvbTrans[DVB_COLOR_RED] = 15;
#define DVB_COLOR_GREEN 201
	dvbColor[DVB_COLOR_GREEN] = rgb2yuv(0,255,0); dvbTrans[DVB_COLOR_GREEN] = 15;
#define DVB_COLOR_MAGENTA 202
	dvbColor[DVB_COLOR_MAGENTA] = rgb2yuv(255,128,255); dvbTrans[DVB_COLOR_MAGENTA] = 15;
}

void KXineWidget::dvbShowOSD()
{
	if ( m_trackURL!="DVB" ) return;

	if ( xine_get_status(m_xineStream)!=XINE_STATUS_PLAY ) return;

	if ( !dvbHaveVideo ) m_trackHasVideo = false;
	else m_trackHasVideo = (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_VIDEO);
	if (m_trackHasVideo)
	{
		m_trackVideoCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_VIDEOCODEC);
		m_videoFrameWidth = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_WIDTH);
		m_videoFrameHeight = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_HEIGHT);
		m_trackVideoBitrate = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_BITRATE);
	}
	else
	{
		m_trackVideoCodec = QString::null;
		m_videoFrameWidth = 0;
		m_videoFrameHeight = 0;
		m_trackVideoBitrate = 0;
	}

	m_trackHasAudio = (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_AUDIO);
	if (m_trackHasAudio)
	{
		m_trackAudioCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_AUDIOCODEC);
		m_trackAudioBitrate = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_AUDIO_BITRATE);
	}
	else
	{
		m_trackAudioCodec = QString::null;
		m_trackAudioBitrate = 0;
	}

	if ( dvbOSD )
	{
		xine_osd_free( dvbOSD );
		dvbOSD = 0;
	}

	int border=40;
	int w = m_videoFrameWidth;
	int h = m_videoFrameHeight;
	if ( !w || !h ) return;
	if ( w<800 )
	{
		if ( dvbCurrentNext[0]=="E" )
		{
			dvbOSDHideTimer.stop();
			dvbOSD = xine_osd_new( m_xineStream, border, border, w-(2*border), h-(2/border) );
		}
		else dvbOSD = xine_osd_new( m_xineStream, border, h-border-100, w-(2*border), 100 );
	}
	if ( dvbOSD )
	{
		if ( !dvbColor[0] ) initDvbPalette();
		//xine_osd_get_palette( dvbOSD, dvbColor, dvbTrans );
		xine_osd_set_palette( dvbOSD, dvbColor, dvbTrans );
		xine_osd_set_font( dvbOSD, "cetus", 16 );
		xine_osd_set_encoding( dvbOSD, NULL );
		if ( dvbCurrentNext[0]=="E" ) xine_osd_draw_rect( dvbOSD, 0, 0, w-(2*border), h-(2*border), DVB_TEXT_WHITE+1, 1 );
		else xine_osd_draw_rect( dvbOSD, 0, 0, w-(2*border), 100, DVB_TEXT_WHITE+1, 1 );
		QString t = QTime::currentTime().toString( "hh:mm" );
		int tw, th, len;
		xine_osd_get_text_size( dvbOSD, t, &tw, &th );
		len = tw;
		int offset = 5;
		xine_osd_draw_text( dvbOSD, w-(2*border)-tw-offset, 5, t, DVB_TEXT_BLUE );
		int i;
		for ( i=0; i<(int)dvbCurrentNext.count(); i++ )
		{
			if ( dvbCurrentNext[i]=="R" )
			{
				xine_osd_draw_rect( dvbOSD, offset, 5, offset+16, 21, DVB_COLOR_RED, 1 );
				offset+=21;
			}
			else if ( dvbCurrentNext[i]=="T" )
			{
				xine_osd_draw_rect( dvbOSD, offset, 5, offset+16, 21, DVB_COLOR_GREEN, 1 );
				offset+=21;
			}
		}
		t = m_trackTitle;
		i=0;
		while ( i<(int)t.length() )
		{
			xine_osd_get_text_size( dvbOSD, t, &tw, &th );
			if ( tw<=(w-(2*border)-offset-5-len) ) break;
			t = t.remove( t.length()-1, t.length() );
			i++;
		}
		xine_osd_draw_text( dvbOSD, offset, 5, t, DVB_TEXT_BLUE );
		xine_osd_draw_line( dvbOSD, 5, 10+18, w-(2*border)-5, 10+18, DVB_COLOR_MAGENTA );

		QString s, c;
		int y=43;
		int pos;
		if ( dvbCurrentNext[0]=="E" )
		{
			if ( dvbCurrentNext.count()<2 )
			{
				xine_osd_show( dvbOSD, 0 );
				return;
			}
			if ( !dvbCurrentNext[1].isEmpty() )
			{
				s = dvbCurrentNext[1];
				pos = s.find("-");
				c = s.left( pos+1 );
				s = s.right( s.length()-pos-1 );
				t = s;
				xine_osd_draw_text( dvbOSD, 10, y, c, DVB_TEXT_GREEN );
				xine_osd_get_text_size( dvbOSD, c, &offset, &th );
				i=0;
				while ( i<(int)t.length() )
				{
					t = s.remove( s.length()-i, s.length() );
					xine_osd_get_text_size( dvbOSD, t, &tw, &th );
					if ( tw<=(w-(2*border)-20) ) break;
					i++;
				}
				xine_osd_draw_text( dvbOSD, 10+offset, y, t, DVB_TEXT_WHITE );
				y+= 40;
			}
			if ( !dvbCurrentNext[2].isEmpty() )
			{
				c = dvbCurrentNext[2];
				pos = 0;
				while ( pos!=-1 && y<(h-(2*border)-23) )
				{
					t = c;
					i = 0;
					while ( pos!=-1 )
					{
						xine_osd_get_text_size( dvbOSD, t, &tw, &th );
						if ( tw<=(w-(2*border)-20) ) break;
						pos = t.findRev(" ");
						t = t.left( pos );
						i++;
					}
					xine_osd_draw_text( dvbOSD, 10, y, t, DVB_TEXT_BLUE );
					if ( !t.length() || !i ) break;
					c = c.right( c.length()-t.length()-1 );
					y+= 28;
				}
				y+= 40;
			}
			if ( !dvbCurrentNext[3].isEmpty() )
			{
				c = dvbCurrentNext[3];
				pos = 0;
				while ( pos!=-1 && y<(h-(2*border)-23) )
				{
					t = c;
					i = 0;
					while ( pos!=-1 )
					{
						xine_osd_get_text_size( dvbOSD, t, &tw, &th );
						if ( tw<=(w-(2*border)-20) ) break;
						pos = t.findRev(" ");
						t = t.left( pos );
						i++;
					}
					xine_osd_draw_text( dvbOSD, 10, y, t, DVB_TEXT_WHITE );
					if ( !t.length() || !i ) break;
					c = c.right( c.length()-t.length()-1 );
					y+= 28;
				}
			}
			xine_osd_show( dvbOSD, 0 );
			return;
		}
		for ( int j=0; j<(int)dvbCurrentNext.count(); j++ )
		{
			if ( (dvbCurrentNext[ j ]=="T") || (dvbCurrentNext[ j ]=="R") ) continue;
			s = dvbCurrentNext[ j ];
			pos = s.find("-");
			c = s.left( pos+1 );
			s = s.right( s.length()-pos-1 );
			t = s;
			xine_osd_draw_text( dvbOSD, 10, y, c, DVB_TEXT_GREEN );
			xine_osd_get_text_size( dvbOSD, c, &offset, &th );
			i=0;
			while ( i<(int)t.length() )
			{
				t = s.remove( s.length()-i, s.length() );
				xine_osd_get_text_size( dvbOSD, t, &tw, &th );
				if ( tw<=(w-(2*border)-20) ) break;
				i++;
			}
			xine_osd_draw_text( dvbOSD, 10+offset, y, t, DVB_TEXT_WHITE );
			y+= 28;
		}
		xine_osd_show( dvbOSD, 0 );
		dvbOSDHideTimer.start( 5000, true );
	}
}

void KXineWidget::dvbHideOSD()
{
	if ( dvbOSD )
	{
		xine_osd_hide( dvbOSD, 0 );
		xine_osd_free( dvbOSD );
		dvbOSD = 0;
	}
}

void KXineWidget::setDvbCurrentNext( const QStringList &list )
{
	if ( list[0]=="STOP" )
	{
		dvbHideOSD();
		return;
	}
	dvbCurrentNext = list;
	QTimer::singleShot( 0, this, SLOT(dvbShowOSD()) );
}

void KXineWidget::setDvb( const QString &pipeName, const QString &chanName, int haveVideo )
{
	m_trackURL = pipeName;
	m_trackTitle = chanName;
	dvbHaveVideo = haveVideo;
}

bool KXineWidget::openDvb()
{
	if ( dvbOSD )
	{
		dvbOSDHideTimer.stop();
		xine_osd_hide( dvbOSD, 0 );
		xine_osd_free( dvbOSD );
		dvbOSD = 0;
	}

	clearQueue();
	m_lengthInfoTimer.stop();
	m_posTimer.stop();
	xine_set_param( m_xineStream, XINE_PARAM_METRONOM_PREBUFFER, 115000);
	if (!xine_open(m_xineStream, m_trackURL))
	{
		sendXineError();
		return false;
	}
	else fprintf(stderr,"xine pipe opened\n");
	m_trackURL = "DVB";
	emit signalXineStatus(i18n("DVB : opening ..."));
	QTimer::singleShot( 0, this, SLOT(playDvb()) );

	return true;
}

void KXineWidget::slotPlayTimeShift()
{
	m_lengthInfoTimer.stop();
	m_posTimer.stop();
	xine_set_param( m_xineStream, XINE_PARAM_METRONOM_PREBUFFER, 0);
	if (!xine_open(m_xineStream, TimeShiftFilename))
	{
		sendXineError();
		return;
	}
	if (!xine_play(m_xineStream, 0,0))
	{
		sendXineError();
		return;
	}
	m_trackIsSeekable = true;
	m_lengthInfoTimer.start(1000);
	m_posTimer.start(1000);
}

void KXineWidget::slotPlay()
{
	if ((!isXineReady()) || (isQueueEmpty())) return;

	if (m_logoFile != NULL && m_trackURL == m_logoFile && isPlaying()) return;

	/* dvb  */
	if ( dvbOSD )
	{
		dvbOSDHideTimer.stop();
		xine_osd_hide( dvbOSD, 0 );
		xine_osd_free( dvbOSD );
		dvbOSD = 0;
	}
	xine_set_param( m_xineStream, XINE_PARAM_METRONOM_PREBUFFER, 90000 );

	m_lengthInfoTimer.stop();
	m_posTimer.stop();
	m_currentSpeed = Normal;

	setCursor(QCursor(Qt::WaitCursor));

	m_trackURL = m_queue.first();
	m_queue.remove(m_queue.find(m_trackURL));

	if (m_trackURL != m_logoFile)
		emit signalXineStatus(i18n("Opening..."));

	/* check for external subtitle file or save url */
	m_trackSubtitleURL = QString::null;
	m_trackSaveURL = QString::null;

	QString ref;

	for (int i = 1; i <= m_trackURL.contains('#'); i++)
	{
		ref = m_trackURL.section('#', i, i);
		if (ref.section(':', 0, 0) == "subtitle")
			m_trackSubtitleURL = ref.section(':', 1);
		if (ref.section(':', 0, 0) == "save")
			m_trackSaveURL = ref.section(':', 1);
	}

	debugOut(QString("Playing: %1").arg(m_trackURL.local8Bit()));

	if (!xine_open(m_xineStream, m_trackURL.local8Bit())) /** pass mrl local 8Bit encoded **/
	{
		sendXineError();
		setCursor(QCursor(Qt::ArrowCursor));
		return;
	}

	/**** use visualization ? ****/
#ifndef USE_QT_ONLY
	unwireAudioFilters();
	wireAudioFilters();
#else
	if ((xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_AUDIO)) &&
	        (!xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_VIDEO)))
	{
		if (m_visualPluginName && (!m_xinePost))
		{
			debugOut(QString("Init visual plugin: %1").arg(m_visualPluginName));
			m_xinePost = xine_post_init(m_xineEngine, m_visualPluginName, 0,
			                            &m_audioDriver,
			                            &m_videoDriver);

			m_postAudioSource = xine_get_audio_source(m_xineStream);
			m_postInput = (xine_post_in_t*)xine_post_input (m_xinePost, const_cast<char*>("audio in"));
			xine_post_wire(m_postAudioSource, m_postInput);
		}
	}
	else
	{
		if (m_xinePost)
		{
			debugOut(QString("Dispose visual plugin: %1").arg(m_visualPluginName));
			m_postAudioSource = xine_get_audio_source(m_xineStream);
			xine_post_wire_audio_port(m_postAudioSource, m_audioDriver);
			xine_post_dispose(m_xineEngine, m_xinePost);
			m_xinePost = NULL;
		}
	}
#endif

	/*** play ***/
	int savedPos = m_savedPos;
	m_savedPos = 0;
	if (!xine_play(m_xineStream, savedPos, 0))
	{
		sendXineError();
		setCursor(QCursor(Qt::ArrowCursor));
		return;
	}

	/* do the stream have chapters ? */
	m_trackHasChapters = (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_CHAPTERS);

	/** information requirement **/

	m_trackTitle = QString::null;

	bool currentUtf8Locale;
	QTextCodec* testUtf8Local = QTextCodec::codecForLocale();
	if (!strcmp(testUtf8Local->name(),"UTF-8"))
		currentUtf8Locale = true;
	else
		currentUtf8Locale = false;

	QTextCodec *CodecUtf8;
	CodecUtf8 = QTextCodec::codecForName("UTF-8");

	QString infotag;
	infotag = QString::fromLatin1(xine_get_meta_info(m_xineStream, XINE_META_INFO_TITLE));

	if (currentUtf8Locale)
		m_trackTitle = infotag;
	else
		m_trackTitle = QString::fromLocal8Bit(infotag);

	if (CodecUtf8->heuristicContentMatch(infotag,infotag.length()) >= 0)
		m_trackTitle = QString::fromUtf8(infotag);

	if ((!m_trackTitle.isNull()) && (!m_trackTitle.isEmpty())) /* no meta? */
	{
		QString trackArtist=NULL;
		QString trackAlbum=NULL;
		QString trackComment=NULL;
		trackArtist = QString::fromLatin1(xine_get_meta_info(m_xineStream, XINE_META_INFO_ARTIST));
		trackAlbum = QString::fromLatin1(xine_get_meta_info(m_xineStream, XINE_META_INFO_ALBUM));
		trackComment = QString::fromLatin1(xine_get_meta_info(m_xineStream, XINE_META_INFO_COMMENT));
		if (currentUtf8Locale)
		{
			m_trackArtist = trackArtist;
			m_trackAlbum = trackAlbum;
			m_trackComment = trackComment;
		}
		else
		{
			m_trackArtist = QString::fromLocal8Bit(trackArtist);
			m_trackAlbum = QString::fromLocal8Bit(trackAlbum);
			m_trackComment = QString::fromLocal8Bit(trackComment);
		}
		if (CodecUtf8->heuristicContentMatch(trackArtist,trackArtist.length()) >= 0)
			m_trackArtist = QString::fromUtf8(trackArtist);
		if (CodecUtf8->heuristicContentMatch(trackAlbum,trackAlbum.length()) >= 0)
			m_trackAlbum = QString::fromUtf8(trackAlbum);
		if (CodecUtf8->heuristicContentMatch(trackComment,trackComment.length()) >= 0)
			m_trackComment = QString::fromUtf8(trackComment);

		m_trackYear = xine_get_meta_info(m_xineStream, XINE_META_INFO_YEAR);
		m_trackNumber = xine_get_meta_info(m_xineStream, XINE_META_INFO_TRACK_NUMBER);
	}
	else
	{
		m_trackArtist = QString::null;
		m_trackAlbum = QString::null;
		m_trackNumber = QString::null;
		m_trackYear = QString::null;
		m_trackComment = QString::null;
	}

	m_trackHasVideo = (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_VIDEO);
	if (m_trackHasVideo)
	{
		m_trackVideoCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_VIDEOCODEC);
		m_videoFrameWidth = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_WIDTH);
		m_videoFrameHeight = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_HEIGHT);
		m_trackVideoBitrate = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_VIDEO_BITRATE);
	}
	else
	{
		m_trackVideoCodec = QString::null;
		m_videoFrameWidth = 0;
		m_videoFrameHeight = 0;
		m_trackVideoBitrate = 0;
	}

	m_trackHasAudio = (bool)xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_AUDIO);
	if (m_trackHasAudio)
	{
		m_trackAudioCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_AUDIOCODEC);
		m_trackAudioBitrate = xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_AUDIO_BITRATE);
	}
	else
	{
		m_trackAudioCodec = QString::null;
		m_trackAudioBitrate = 0;
	}

	/*** we need a little delay for some meta info ***/
	QTimer::singleShot(1000, this, SLOT(slotGetInfoDelayed()));


	m_trackLength = getLengthInfo();
	if ((m_trackLength.isNull()) && (m_trackURL != m_logoFile))
	{
		debugOut("Wait for valid length information");
		m_lengthInfoTries = 0;
		m_lengthInfoTimer.start(1000); /* wait for available track length info */
	}

	if (m_trackTitle.isNull() || m_trackTitle.isEmpty())
	{
		/* no meta info */
		m_trackTitle = m_trackURL;
	}

	slotSetAudioChannel(0); //refresh channel info
	if (m_trackURL != m_logoFile)
		m_posTimer.start(500);
	setCursor(QCursor(Qt::ArrowCursor));

	if (m_trackURL != m_logoFile)
	{
		emit signalXinePlaying();
		if (hasSaveURL())
			emit signalXineStatus(i18n("Recording"));
		else
			emit signalXineStatus(i18n("Playing"));
	}
}

void KXineWidget::slotGetInfoDelayed()
{
	if (!m_xineStream)
		return;

	if (m_trackHasVideo)
		m_trackVideoCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_VIDEOCODEC);
	if (m_trackHasAudio)
		m_trackAudioCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_AUDIOCODEC);
}


/****** error processing ****/

void KXineWidget::sendXineError()
{
	QString error;
	int errCode = xine_get_error(m_xineStream);

	QString addInfo;
	QString audioCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_AUDIOCODEC);
	QString videoCodec = xine_get_meta_info(m_xineStream, XINE_META_INFO_VIDEOCODEC);
	if ((!audioCodec.isEmpty()) || (!videoCodec.isEmpty()))
	{
		if (!audioCodec.isEmpty())
			addInfo.append(QString("(") + i18n("Audio Codec") + ": " + audioCodec + ")");
		if (!videoCodec.isEmpty())
			addInfo.append(QString("(") + i18n("Video Codec") + ": " + videoCodec + ")");
	}
	else
		addInfo.append(QString("(") + m_trackURL + ")");

	switch (errCode)
	{
		case XINE_ERROR_NO_INPUT_PLUGIN:
		case XINE_ERROR_NO_DEMUX_PLUGIN:
		{
			error = i18n("No plugin found to handle this resource") + " " + addInfo;
			break;
		}
		case XINE_ERROR_DEMUX_FAILED:
		{
			error = i18n("Resource seems to be broken") + " (" + m_trackURL + ")";
			break;
		}
		case XINE_ERROR_MALFORMED_MRL:
		{
			error = i18n("Requested resource does not exist") + " (" + m_trackURL + ")";
			break;
		}
		case XINE_ERROR_INPUT_FAILED:
		{
			error = i18n("Resource can not be opened") + " (" + m_trackURL + ")";
			break;
		}
		default:
		{
			error = i18n("Generic error") + " (" + m_trackURL + ")";
			break;
		}
	}

	if (isQueueEmpty())
	{
		if (m_trackURL != m_logoFile)
		{
			emit signalXineStatus(i18n("Error"));
			emit signalXineError(error);
		}
		else
			errorOut("Can't find/play logo file!");
	}
	else
	{
		errorOut(error);
		errorOut(QString("Can't play: %1 - trying next").arg(m_trackURL));
		QTimer::singleShot(0, this, SLOT(slotPlay()));
	}
}

bool KXineWidget::isPlaying() const
{
	if (isXineReady())
		return ((xine_get_status(m_xineStream) == XINE_STATUS_PLAY) && (m_trackURL != m_logoFile));
	else
		return false;
}

QString KXineWidget::getXineLog() const
{
	QString logStr;
	int i = 0;
	QTextStream ts(&logStr, IO_WriteOnly);

	const char* const* log = xine_get_log(m_xineEngine, /* XINE_LOG_MSG*/ 0);
	while(log[i])
	{
		ts << QString::fromLocal8Bit(log[i]);
		i++;
	}

	return logStr;
}

void KXineWidget::showOSDMessage(const QString& message, uint duration, int priority)
{
	if ((!m_osd) || (!m_osdShow) || (isHidden()))
		return;

	static int prevOsdPriority = 0;
	if (m_osdTimer.isActive() && prevOsdPriority > priority)
		return;
	prevOsdPriority = priority;

	//debugOut(QString("OSD: draw text: %1").arg(message));
	xine_osd_clear(m_osd);
	xine_osd_draw_text(m_osd, 0, 0, message.local8Bit(), XINE_OSD_TEXT1);

	if (m_osdUnscaled)
		xine_osd_show_unscaled(m_osd, 0);
	else
		xine_osd_show(m_osd, 0);

	m_osdTimer.start(duration);
}

void KXineWidget::slotOSDHide()
{
	xine_osd_hide(m_osd, 0);
	m_osdTimer.stop();
}

#ifndef USE_QT_ONLY
/****************** postprocessing filter management ****************/

QStringList KXineWidget::getVideoFilterNames() const
{
	QStringList filters;
	const char* const* plugins = xine_list_post_plugins_typed(m_xineEngine, XINE_POST_TYPE_VIDEO_FILTER);

	for (int i = 0; plugins[i]; i++)
	{
		filters << plugins[i];
	}
	return filters;
}

QStringList KXineWidget::getAudioFilterNames() const
{
	QStringList filters;
	const char* const* plugins = xine_list_post_plugins_typed(m_xineEngine, XINE_POST_TYPE_AUDIO_FILTER);

	for (int i = 0; plugins[i]; i++)
	{
		filters << plugins[i];
	}
	return filters;
}
#endif

void KXineWidget::slotCreateVideoFilter(const QString& name, QWidget* parent)
{
#ifndef USE_QT_ONLY
	unwireVideoFilters();

	PostFilter* filter = new PostFilter(name, m_xineEngine, m_audioDriver, m_videoDriver, parent);
	connect(filter, SIGNAL(signalDeleteMe(PostFilter*)), this, SLOT(slotDeleteVideoFilter(PostFilter*)));
	m_videoFilterList.append(filter);

	wireVideoFilters();
#else
	parent = parent;
	warningOut(QString("Not implemented [CreateVideoFilter %1]").arg(name));
#endif
}

void KXineWidget::slotCreateAudioFilter(const QString& name, QWidget* parent)
{
#ifndef USE_QT_ONLY
	unwireAudioFilters();

	PostFilter* filter = new PostFilter(name, m_xineEngine, m_audioDriver, m_videoDriver, parent);
	connect(filter, SIGNAL(signalDeleteMe(PostFilter*)), this, SLOT(slotDeleteAudioFilter(PostFilter*)));
	m_audioFilterList.append(filter);

	wireAudioFilters();
#else
	parent = parent;
	warningOut(QString("Not implemented [CreateAudioFilter %1]").arg(name));
#endif
}


void KXineWidget::slotRemoveAllVideoFilters()
{
#ifndef USE_QT_ONLY
	unwireVideoFilters();
	while (m_videoFilterList.count())
		m_videoFilterList.removeLast();
	wireVideoFilters();
#else
	warningOut("Not implemented!");
#endif
}

void KXineWidget::slotRemoveAllAudioFilters()
{
#ifndef USE_QT_ONLY
	unwireAudioFilters();
	while (m_audioFilterList.count())
		m_audioFilterList.removeLast();
	wireAudioFilters();
#else
	warningOut("Not implemented!");
#endif
}

void KXineWidget::slotDeleteVideoFilter(PostFilter* filter)
{
#ifndef USE_QT_ONLY
	unwireVideoFilters();
	m_videoFilterList.remove(filter);
	wireVideoFilters();
#else
	filter = filter;
	warningOut("Not implemented!");
#endif
}

void KXineWidget::slotDeleteAudioFilter(PostFilter* filter)
{
#ifndef USE_QT_ONLY
	unwireAudioFilters();
	m_audioFilterList.remove(filter);
	wireAudioFilters();
#else
	filter = filter;
	warningOut("Not implemented!");
#endif
}

#ifndef USE_QT_ONLY
void KXineWidget::unwireVideoFilters()
{
	if (m_xineStream && m_videoDriver)
		xine_post_wire_video_port(xine_get_video_source(m_xineStream), m_videoDriver);
}

void KXineWidget::wireVideoFilters()
{
	if (!m_xineStream)
	{
		debugOut("wireVideoFilters() - xine stream not initialized, nothing happend.");
		return;
	}

	QPtrList<PostFilter> activeList;

	if (m_videoFilterList.count() && m_videoFiltersEnabled)
		activeList = m_videoFilterList;

	if (m_deinterlaceFilter && m_deinterlaceEnabled )
		activeList.insert (0, m_deinterlaceFilter);

	if (activeList.count())
	{
		xine_post_wire_video_port(activeList.at(activeList.count()-1)->getOutput(), m_videoDriver);

		for (uint i = activeList.count()-1; i >0; i--)
		{
			xine_post_wire(activeList.at( i-1 )->getOutput(), activeList.at(i)->getInput());
		}

		xine_post_wire( xine_get_video_source(m_xineStream), activeList.at(0)->getInput());
	}
}

void KXineWidget::unwireAudioFilters()
{
	if (m_xineStream && m_audioDriver)
		xine_post_wire_audio_port( xine_get_audio_source (m_xineStream), m_audioDriver);
}

void KXineWidget::wireAudioFilters()
{
	if (!m_xineStream)
	{
		debugOut("wireAudioFilters() - xine stream not initialized, nothing happend.");
		return;
	}

	QPtrList<PostFilter> activeList;

	if (m_audioFilterList.count() && m_audioFiltersEnabled)
		activeList = m_audioFilterList;

	if ( (xine_get_stream_info (m_xineStream, XINE_STREAM_INFO_HAS_AUDIO)) &&
	        (!xine_get_stream_info (m_xineStream, XINE_STREAM_INFO_HAS_VIDEO)) &&
	        m_visualPluginName )
	{
		if (!m_visualPlugin)
		{
			debugOut(QString("Init visual plugin: %1").arg(m_visualPluginName));
			m_visualPlugin = new PostFilter(m_visualPluginName, m_xineEngine, m_audioDriver, m_videoDriver, NULL);
		}

		activeList.insert (0, m_visualPlugin);
	}
	else
	{
		if (m_visualPlugin)
		{
			debugOut(QString("Dispose visual plugin: %1").arg(m_visualPluginName));
			delete m_visualPlugin;
			m_visualPlugin = NULL;
		}
	}

	if (activeList.count())
	{
		xine_post_wire_audio_port(activeList.at(activeList.count()-1)->getOutput(), m_audioDriver);

		for (uint i = activeList.count()-1; i >0; i--)
		{
			xine_post_wire(activeList.at(i-1)->getOutput(), activeList.at(i)->getInput());
		}

		xine_post_wire( xine_get_audio_source(m_xineStream), activeList.at(0)->getInput());
	}
}
#endif

void KXineWidget::slotEnableVideoFilters(bool enable)
{
#ifndef USE_QT_ONLY
	m_videoFiltersEnabled = enable;

	unwireVideoFilters();
	wireVideoFilters();
#else
	enable = enable;
	warningOut("Not implemented!");
#endif
}

void KXineWidget::slotEnableAudioFilters(bool enable)
{
#ifndef USE_QT_ONLY
	m_audioFiltersEnabled = enable;

	unwireAudioFilters();
	wireAudioFilters();
#else
	enable = enable;
	warningOut("Not implemented!");
#endif
}

#ifndef USE_QT_ONLY
QStringList KXineWidget::getAudioFilterConfig()
{
	QStringList configStrings;
	for (uint i=0; i<m_audioFilterList.count(); i++)
		configStrings << m_audioFilterList.at(i)->getConfig();
	return configStrings;
}

QStringList KXineWidget::getVideoFilterConfig()
{
	QStringList configStrings;
	for (uint i=0; i<m_videoFilterList.count(); i++)
		configStrings << m_videoFilterList.at(i)->getConfig();
	return configStrings;
}
#endif

/****  visual plugin **********/

QStringList KXineWidget::getVisualPlugins() const
{
	QStringList visuals;
	const char* const* plugins = xine_list_post_plugins_typed(m_xineEngine, XINE_POST_TYPE_AUDIO_VISUALIZATION);

	for (int i = 0; plugins[i]; i++)
	{
		visuals << plugins[i];
	}
	return visuals;
}

/**************** change visualization plugin *****************/

void KXineWidget::slotSetVisualPlugin(const QString& visual)
{
	if (m_visualPluginName == visual) return;
	debugOut(QString("New visualization plugin: %1").arg(visual));

#ifndef USE_QT_ONLY
	unwireAudioFilters();
	if(m_visualPlugin)
	{
		delete m_visualPlugin;
		m_visualPlugin = NULL;
	}

	if (visual == "none")
		m_visualPluginName = QString::null;
	else
		m_visualPluginName = visual;

	wireAudioFilters();
#else
	if (visual == "none")
		m_visualPluginName = QString::null;
	else
		m_visualPluginName = visual;

	if (m_xinePost)
	{
		xine_post_out_t *pp;

		pp = xine_get_audio_source(m_xineStream);
		xine_post_wire_audio_port(pp, m_audioDriver);
		xine_post_dispose(m_xineEngine, m_xinePost);
		m_xinePost = NULL;
	}

	if ( (xine_get_status(m_xineStream ) == XINE_STATUS_PLAY)
			&& (!xine_get_stream_info(m_xineStream, XINE_STREAM_INFO_HAS_VIDEO)) && (m_visualPluginName) )
	{
		m_xinePost = xine_post_init(m_xineEngine, m_visualPluginName, 0, &m_audioDriver, &m_videoDriver);
		m_postAudioSource = xine_get_audio_source(m_xineStream);
		m_postInput = (xine_post_in_t*)xine_post_input(m_xinePost, const_cast<char*>("audio in"));
		xine_post_wire(m_postAudioSource, m_postInput);
	}
#endif
}

/*****/

void KXineWidget::getAutoplayPlugins(QStringList& autoPlayList) const
{
	char** pluginIds = NULL;
	int i = 0;

	pluginIds = (char**)xine_get_autoplay_input_plugin_ids(m_xineEngine);

	while(pluginIds[i])
	{
		autoPlayList << pluginIds[i];

		autoPlayList << xine_get_input_plugin_description(m_xineEngine, pluginIds[i]);
		i++;
	}
}

bool KXineWidget::getAutoplayPluginURLS(const QString& plugin, QStringList& list)
{
	char** urls = NULL;
	int num;
	int i = 0;

	urls = xine_get_autoplay_mrls(m_xineEngine, plugin, &num);

	if (urls)
	{
		while (urls[i])
		{
			list << urls[i];
			i++;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void KXineWidget::slotSetVolume(int vol)
{
	if (!isXineReady()) return;
	if (m_softwareMixer)
	{
		//debugOut(QString("Set software amplification level: %1").arg(vol));
		if (m_volumeGain)
			xine_set_param(m_xineStream, XINE_PARAM_AUDIO_AMP_LEVEL, vol*2);
		else
			xine_set_param(m_xineStream, XINE_PARAM_AUDIO_AMP_LEVEL, vol);
	}
	else
	{
		//debugOut(QString("Set audio mixer volume: %1").arg(vol));
		xine_set_param(m_xineStream, XINE_PARAM_AUDIO_VOLUME, vol);
	}
	emit signalXineStatus(i18n("Volume") + ": " + QString::number(vol) +"%");
}

uint KXineWidget::getVolume() const
{
	if (!isXineReady()) return 0;
	uint vol;
	if (m_softwareMixer)
	{
		vol = xine_get_param(m_xineStream, XINE_PARAM_AUDIO_AMP_LEVEL);
		if (vol > 200)
		{
			// when amp is high > 100, xine_get_param sometimes returns incorrect amp level
			errorOut("Amp level returned weird results, set Amp to 100");
			vol = 100;
		}
		if (m_volumeGain) vol = vol/2;
	}
	else
	{
		vol = xine_get_param(m_xineStream, XINE_PARAM_AUDIO_VOLUME);
	}
	return vol;
}

void KXineWidget::slotToggleMute()
{
	int muteParam;
	if (m_softwareMixer)
		muteParam = XINE_PARAM_AUDIO_AMP_MUTE;
	else
		muteParam = XINE_PARAM_AUDIO_MUTE;

	if (xine_get_param(m_xineStream, muteParam))
	{
		xine_set_param(m_xineStream, muteParam, 0); /* mute off */
		emit signalXineStatus(i18n("Mute") + ": " + i18n("Off"));
	}
	else
	{
		xine_set_param(m_xineStream, muteParam, 1); /* mute on */
		emit signalXineStatus(i18n("Mute") + ": " + i18n("On"));
	}
}

bool KXineWidget::SoftwareMixing() const
{
	if (m_softwareMixer)
		return true;
	else
		return false;
}

void KXineWidget::mouseMoveEvent(QMouseEvent* mev)
{
	if (!m_xineReady) return;

	if (cursor().shape() == Qt::BlankCursor)
	{
		setCursor(QCursor(Qt::ArrowCursor));
	}

	x11_rectangle_t   rect;
	xine_event_t      event;
	xine_input_data_t input;

	rect.x = mev->x();
	rect.y = mev->y();
	rect.w = 0;
	rect.h = 0;

	xine_port_send_gui_data (m_videoDriver, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, (void*)&rect);

	event.type        = XINE_EVENT_INPUT_MOUSE_MOVE;
	event.data        = &input;
	event.data_length = sizeof(input);
	input.button      = 0;
	input.x           = rect.x;
	input.y           = rect.y;
	xine_event_send(m_xineStream, &event);
}


void KXineWidget::mousePressEvent(QMouseEvent* mev)
{
	if (!m_xineReady) return;
	int cur = cursor().shape();

	if (mev->button() == Qt::MidButton)
	{
		emit signalMiddleClick();
		mev->ignore();
		return;
	}

	if (mev->button() == Qt::RightButton)
	{
		if ( (cur == Qt::ArrowCursor) || (cur == Qt::BlankCursor) )
		{
			emit signalRightClick(mev->globalPos());
			mev->accept();
			return;
		}
	}

	if (mev->button() == Qt::LeftButton)
	{
		if ( (cur == Qt::ArrowCursor) || (cur == Qt::BlankCursor) )
		{
			emit signalLeftClick(mev->globalPos());
			mev->ignore();
			return;
		}

		x11_rectangle_t   rect;
		xine_event_t      event;
		xine_input_data_t input;

		rect.x = mev->x();
		rect.y = mev->y();
		rect.w = 0;
		rect.h = 0;

		xine_port_send_gui_data(m_videoDriver, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, (void*)&rect);

		event.type        = XINE_EVENT_INPUT_MOUSE_BUTTON;
		event.data        = &input;
		event.data_length = sizeof(input);
		input.button      = 1;
		input.x           = rect.x;
		input.y           = rect.y;
		xine_event_send (m_xineStream, &event);
		mev->accept(); /* don't send event to parent */
	}
}

void KXineWidget::mouseDoubleClickEvent(QMouseEvent* mev)
{
	emit signalDoubleClick();
	mev->ignore();
}

void KXineWidget::wheelEvent(QWheelEvent* e)
{
	int oldVal = getPosition();
	if (oldVal == 0) // no valid position
		return;

	float offset = log10( QABS(e->delta()) ) / 0.002;
	int newVal = 0;
	if (e->delta()>0)
		newVal = oldVal + int(offset);
	else
		newVal = oldVal - int(offset);
	if (newVal < 0) newVal = 0;
	slotSeekToPosition(newVal);
	e->accept();
}

void KXineWidget::playNextChapter() const
{
	xine_event_t xev;

	xev.type = XINE_EVENT_INPUT_NEXT;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::playPreviousChapter() const
{
	xine_event_t xev;

	xev.type = XINE_EVENT_INPUT_PREVIOUS;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotStop()
{
	m_posTimer.stop();
	if ( m_lengthInfoTimer.isActive() ) m_lengthInfoTimer.stop();
	//emit signalNewPosition(0, QTime());

	if ((m_logoFile.isNull()) && (isPlaying()))
		xine_stop(m_xineStream);
	else
	{
		appendToQueue(m_logoFile);
		QTimer::singleShot(0, this, SLOT(slotPlay()));
	}

	emit signalXineStatus(i18n("Stop"));
}

void KXineWidget::slotSetAudiocdDevice(const QString& device)
{
	debugOut(QString("Set AudioCD device to %1").arg(device));

	xine_cfg_entry_t config;
	xine_config_lookup_entry(m_xineEngine, "input.cdda_device", &config);
	if (m_cachedCDPath.isNull())
		m_cachedCDPath = config.str_value;
	config.str_value = (char*)device.latin1();
	xine_config_update_entry (m_xineEngine, &config);
}

void KXineWidget::slotSetVcdDevice(const QString& device)
{
	debugOut(QString("Set VCD device to %1").arg(device));

	xine_cfg_entry_t config;
	xine_config_lookup_entry(m_xineEngine, "input.vcd_device", &config);
	if (m_cachedVCDPath.isNull())
		m_cachedVCDPath = config.str_value;
	config.str_value = (char*)device.latin1();
	xine_config_update_entry (m_xineEngine, &config);
}

void KXineWidget::slotSetDvdDevice(const QString& device)
{
	debugOut(QString("Set DVD device to %1").arg(device));

	xine_cfg_entry_t config;
	xine_config_lookup_entry(m_xineEngine, "input.dvd_device", &config);
	if (m_cachedDVDPath.isNull())
		m_cachedDVDPath = config.str_value;
	config.str_value = (char*)device.latin1();
	xine_config_update_entry (m_xineEngine, &config);
}

QString KXineWidget::audiocdDevice() const
{
	xine_cfg_entry_t config;
	xine_config_lookup_entry(m_xineEngine, "input.cdda_device", &config);

	return QString(config.str_value);
}

QString KXineWidget::vcdDevice() const
{
	xine_cfg_entry_t config;
	xine_config_lookup_entry(m_xineEngine, "input.vcd_device", &config);

	return QString(config.str_value);
}

QString KXineWidget::dvdDevice() const
{
	xine_cfg_entry_t config;
	xine_config_lookup_entry(m_xineEngine, "input.dvd_device", &config);

	return QString(config.str_value);
}

void KXineWidget::setStreamSaveDir(const QString& dir)
{
	xine_cfg_entry_t config;

	if (!xine_config_lookup_entry(m_xineEngine, "misc.save_dir", &config)) return; /* older xine-lib */

	debugOut(QString("Set misc.save_dir to: %1").arg(dir));
	config.str_value = (char*)dir.latin1();
	xine_config_update_entry (m_xineEngine, &config);
}

const QString KXineWidget::getStreamSaveDir()
{
	xine_cfg_entry_t config;

	if (!xine_config_lookup_entry(m_xineEngine, "misc.save_dir", &config)) return QString::null; /* older xine-lib */

	return QString(config.str_value);
}

void KXineWidget::setBroadcasterPort(const uint port)
{
	debugOut(QString("Set broadcaster port to %1").arg(port));
	xine_set_param(m_xineStream, XINE_PARAM_BROADCASTER_PORT, port);
}

void KXineWidget::slotSpeedPause()
{
	if (m_currentSpeed == Pause)
	{
		slotSpeedNormal();
	}
	else if (m_trackURL != m_logoFile) // don't pause logo
	{
		xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
		m_posTimer.stop();
		if (m_currentSpeed != Undefined)
			emit signalXineStatus(i18n("Pause"));
		m_currentSpeed = Pause;
	}
}

void KXineWidget::slotSpeedNormal()
{
	xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
	m_posTimer.start(500);
	m_currentSpeed = Normal;
	emit signalXineStatus(i18n("Playing") + " ");
}

void KXineWidget::slotSpeedFaster()
{
	switch (m_currentSpeed)
	{
		case Fast1:
		{
			xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_FAST_4);
			m_currentSpeed = Fast2;
			emit signalXineStatus(i18n("Fast Forward %1").arg("x2"));
			break;
		}
		case Fast2:
		{
			slotSpeedNormal();
			break;
		}
		default:
		{
			xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_FAST_2);
			m_currentSpeed = Fast1;
			emit signalXineStatus(i18n("Fast Forward %1").arg("x1"));
			break;
		}
	}
}

void KXineWidget::slotSpeedSlower()
{
	switch (m_currentSpeed)
	{
		case Slow1:
		{
			xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_SLOW_4);
			m_currentSpeed = Slow2;
			emit signalXineStatus(i18n("Slow Motion %1").arg("x2"));
			break;
		}
		case Slow2:
		{
			slotSpeedNormal();
			break;
		}
		default:
		{
			xine_set_param(m_xineStream, XINE_PARAM_SPEED, XINE_SPEED_SLOW_2);
			m_currentSpeed = Slow1;
			emit signalXineStatus(i18n("Slow Motion %1").arg("x1"));
			break;
		}
	}
}

QString KXineWidget::getSupportedExtensions() const
{
	return xine_get_file_extensions(m_xineEngine);
}

void KXineWidget::slotSetAudioChannel(int ch)
{
	debugOut(QString("Switch to audio channel %1").arg(ch-1));
	xine_set_param(m_xineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, ch-1);
}


void KXineWidget::slotSetSubtitleChannel(int ch)
{
	debugOut(QString("Switch to subtitle channel %1").arg(ch-1));
	xine_set_param(m_xineStream, XINE_PARAM_SPU_CHANNEL, ch-1);
}


void KXineWidget::slotSetFileSubtitles(QString url)
{
	int pos;
	int time;
	int length;

	m_queue.prepend(url);

	int t = 0, ret = 0;
	while(((ret = xine_get_pos_length(m_xineStream, &pos, &time, &length)) == 0) && (++t < 5))
		xine_usec_sleep(100000);

	if ( ret == 0 )
	{
		debugOut("No valid stream position information");
		return;
	}

	if (isPlaying())
		xine_stop(m_xineStream);
	m_posTimer.stop();

	slotPlay();
	slotSeekToPosition(pos);
}

uint KXineWidget::getPosition() const
{
	if (!m_xineReady) return 0;

	int pos, time, length;

	int t = 0, ret = 0;
	while(((ret = xine_get_pos_length(m_xineStream, &pos, &time, &length)) == 0) && (++t < 5))
		xine_usec_sleep(100000);

	if ( ret == 0 )
	{
		debugOut("No valid stream position information");
		return 0;
	}

	return (uint)pos;
}

QTime KXineWidget::getPlaytime() const
{
	if (!m_xineReady) return QTime();

	int pos, time, length;

	int t = 0, ret = 0;
	while(((ret = xine_get_pos_length(m_xineStream, &pos, &time, &length)) == 0) && (++t < 5))
		xine_usec_sleep(100000);

	if ( ret == 0 )
	{
		debugOut("No valid stream position information");
		return QTime();
	}

	return msToTime(time);
}

void KXineWidget::slotSendPosition()
{
	if (!m_xineReady) return;

	int pos, time, length;

	int t = 0, ret = 0;
	while(((ret = xine_get_pos_length(m_xineStream, &pos, &time, &length)) == 0) && (++t < 5))
		xine_usec_sleep(100000);

	if ( ret == 0 )
	{
		debugOut("No valid stream position information");
		return;
	}

	emit signalNewPosition(pos, msToTime(time));
}

void KXineWidget::slotStartSeeking()
{
	debugOut("Seeking started");
	m_posTimer.stop();
}

void KXineWidget::slotSeekToPosition(int pos)
{
	if ((!isXineReady()) || (!isPlaying()) || (!isSeekable())) return;

	bool pause = false;
	if (m_currentSpeed == Pause)
		pause = true;

	xine_play(m_xineStream, pos, 0);

	double length = QTime().msecsTo(getLengthInfo());

	length = (length * pos)/65535;
	QTime time = QTime().addMSecs((int)length);
	emit signalNewPosition(pos, time);
	emit signalXineStatus(i18n("Position") + ": " + time.toString("h:mm:ss"));

	if (pause)
	{
		m_currentSpeed = Undefined;
		slotSpeedPause();
	}
}

void KXineWidget::slotSeekToTime(const QTime& postime)
{
	if ((!isXineReady()) || (!isPlaying()) || (!isSeekable())) return;

	bool pause = false;

	if (m_currentSpeed == Pause)
		pause = true;

	xine_play(m_xineStream, 0, QTime().msecsTo(postime));

	double length = QTime().msecsTo(getLengthInfo());
	double posfract = QTime().msecsTo(postime);
	posfract = (posfract / length) * 65535;
	emit signalNewPosition((int)posfract, postime);
	emit signalXineStatus(i18n("Position") + ": " + postime.toString("h:mm:ss"));

	if (pause)
	{
		m_currentSpeed = Undefined;
		slotSpeedPause();
	}
}

void KXineWidget::slotStopSeeking()
{
	debugOut("Seeking stopped");
	m_posTimer.start(500, false);
}

void KXineWidget::slotEject()
{
	xine_eject(m_xineStream);
}

void KXineWidget::slotEnableAutoresize(bool enable)
{
	m_autoresizeEnabled = enable;
	if (!m_autoresizeEnabled)
	{
		m_videoFrameHeight = 0;
		m_videoFrameWidth = 0;
	}
}

/***************************************
 *    tvtime deinterlacer plugin       *
 ***************************************/

#ifndef USE_QT_ONLY
void KXineWidget::createDeinterlacePlugin(const QString& config, QWidget* parent)
{
	m_deinterlaceFilter = new PostFilter(config.section(':',0,0), m_xineEngine, m_audioDriver, m_videoDriver, parent);
	if( !m_deinterlaceFilter->getInput() || !m_deinterlaceFilter->getOutput() )
	{
		delete m_deinterlaceFilter;
		m_deinterlaceFilter = NULL;
	}

	slotSetDeinterlaceConfig(config);
}

const QString KXineWidget::getDeinterlaceConfig() const
{
	if (m_deinterlaceFilter)
		return m_deinterlaceFilter->getConfig();

	return DEFAULT_TVTIME_CONFIG;
}
#endif

void KXineWidget::slotSetDeinterlaceConfig(const QString& config)
{
#ifndef USE_QT_ONLY
	if (m_deinterlaceFilter)
		m_deinterlaceFilter->setConfig(config);
#else
	warningOut(QString ("Not implemented [SetDeinterlaceConfig %1]").arg(config));
#endif
}

void KXineWidget::slotToggleDeinterlace()
{
#ifndef USE_QT_ONLY
	QString s;

	if (m_deinterlaceFilter)
	{
		m_deinterlaceEnabled = !m_deinterlaceEnabled;
		debugOut(QString("Deinterlace enabled: %1").arg(m_deinterlaceEnabled));
		if ( m_deinterlaceEnabled ) s = i18n("Deinterlace : on");
		else s = i18n("Deinterlace : off");
		showOSDMessage( s, 2000 );
		unwireVideoFilters();
		wireVideoFilters();
	}
	else
	{
		/* fallback - this method is deprecated */
		if (xine_get_param(m_xineStream, XINE_PARAM_VO_DEINTERLACE))
			xine_set_param(m_xineStream, XINE_PARAM_VO_DEINTERLACE, false);
		else
			xine_set_param(m_xineStream, XINE_PARAM_VO_DEINTERLACE, true);
	}
#else
	warningOut("Not implemented!");
#endif
}

/**************************/

void KXineWidget::slotAspectRatioAuto()
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_AUTO);
	emit signalXineStatus(i18n("Aspect Ratio") + ": " + i18n("Auto"));
}

void KXineWidget::slotAspectRatio4_3()
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_4_3);
	emit signalXineStatus(i18n("Aspect Ratio") + ": " + i18n("4:3"));
}

void KXineWidget::slotAspectRatioAnamorphic()
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_ANAMORPHIC);
	emit signalXineStatus(i18n("Aspect Ratio") + ": " + i18n("16:9"));
}

void KXineWidget::slotAspectRatioSquare()
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_SQUARE);
	emit signalXineStatus(i18n("Aspect Ratio") + ": " + i18n("1:1"));
}

void KXineWidget::slotAspectRatioDVB()
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_DVB);
	emit signalXineStatus(i18n("Aspect Ratio") + ": " + i18n("2.11:1"));
}

void KXineWidget::slotZoomOut()
{
	if ((m_currentZoom - 5) >= 100)
	{
		m_currentZoom -= 5;
		xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_X, m_currentZoom);
		xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_Y, m_currentZoom);
		emit signalXineStatus(i18n("Zoom") + ": " + QString::number(m_currentZoom) + "%");
	}
}

void KXineWidget::slotZoomIn()
{
	if ((m_currentZoom + 5) <= XINE_VO_ZOOM_MAX)
	{
		m_currentZoom += 5;
		xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_X, m_currentZoom);
		xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_Y, m_currentZoom);
		emit signalXineStatus(i18n("Zoom") + ": " + QString::number(m_currentZoom) + "%");
	}
}

void KXineWidget::slotZoomOff()
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_X, 100);
	xine_set_param(m_xineStream, XINE_PARAM_VO_ZOOM_Y, 100);
	m_currentZoom = 100;
	emit signalXineStatus(i18n("Zoom") + ": " + QString::number(m_currentZoom) + "%");
}

QTime KXineWidget::getLengthInfo()
{
	int pos, time, length;

	int t = 0, ret = 0;
	while(((ret = xine_get_pos_length(m_xineStream, &pos, &time, &length)) == 0) && (++t < 5))
		xine_usec_sleep(100000);

	if ( (ret != 0) && (length > 0) )
	{
		return msToTime(length);
	}

	return QTime();
}

void KXineWidget::slotEmitLengthInfo()
{
	QTime length = getLengthInfo();
	if (!(length.isNull()))
	{
		if ( m_trackURL!="DVB" ) m_lengthInfoTimer.stop();
		m_trackLength = length;
		emit signalLengthChanged();
	}
	else
	{
		if (m_lengthInfoTries > 10) // wait 10 seconds
			m_lengthInfoTimer.stop();
		else
		{
			debugOut("Wait for valid length information");
			m_lengthInfoTries ++;
		}
	}
}

void KXineWidget::globalPosChanged()
{
	QPoint g = mapToGlobal(QPoint(0,0));
	m_globalX = g.x();
	m_globalY = g.y();
}

const xine_t* const KXineWidget::getXineEngine()const
{
	return m_xineEngine;
}

/************ video settings ****************/

void KXineWidget::getVideoSettings(int& hue, int& sat, int& contrast, int& bright,
                                   int& avOffset, int& spuOffset) const
{
	hue = xine_get_param(m_xineStream, XINE_PARAM_VO_HUE);
	sat = xine_get_param(m_xineStream, XINE_PARAM_VO_SATURATION);
	contrast = xine_get_param(m_xineStream, XINE_PARAM_VO_CONTRAST);
	bright = xine_get_param(m_xineStream, XINE_PARAM_VO_BRIGHTNESS);
	avOffset = xine_get_param(m_xineStream, XINE_PARAM_AV_OFFSET);
	spuOffset = xine_get_param(m_xineStream, XINE_PARAM_SPU_OFFSET);
}

void KXineWidget::slotSetHue(int hue)
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_HUE, hue);
	emit signalXineStatus(i18n("Hue") + ": " + QString::number((hue*100)/65535) + "%");
}

void KXineWidget::slotSetSaturation(int sat)
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_SATURATION, sat);
	emit signalXineStatus(i18n("Saturation") + ": " + QString::number((sat*100)/65535) + "%");
}

void KXineWidget::slotSetContrast(int contrast)
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_CONTRAST, contrast);
	emit signalXineStatus(i18n("Contrast") + ": " + QString::number((contrast*100)/65535) + "%");
}

void KXineWidget::slotSetBrightness(int bright)
{
	xine_set_param(m_xineStream, XINE_PARAM_VO_BRIGHTNESS, bright);
	emit signalXineStatus(i18n("Brightness") + ": " + QString::number((bright*100)/65535) + "%");
}

void KXineWidget::slotSetAVOffset(int av)
{
	xine_set_param(m_xineStream, XINE_PARAM_AV_OFFSET, av);
	emit signalXineStatus(i18n("Audio/Video Offset") + ": " + QString::number(av/90) + i18n("msec"));
}

void KXineWidget::slotSetSpuOffset(int spu)
{
	xine_set_param(m_xineStream, XINE_PARAM_SPU_OFFSET, spu);
	emit signalXineStatus(i18n("Subtitle Offset") + ": " + QString::number(spu/90) + i18n("msec"));
}


/**************** equalizer *****************/

void KXineWidget::slotSetEq30(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_30HZ, -val);
}

void KXineWidget::slotSetEq60(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_60HZ, -val);
}

void KXineWidget::slotSetEq125(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_125HZ, -val);
}

void KXineWidget::slotSetEq250(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_250HZ, -val);
}

void KXineWidget::slotSetEq500(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_500HZ, -val);
}

void KXineWidget::slotSetEq1k(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_1000HZ, -val);
}

void KXineWidget::slotSetEq2k(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_2000HZ, -val);
}

void KXineWidget::slotSetEq4k(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_4000HZ, -val);
}

void KXineWidget::slotSetEq8k(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_8000HZ, -val);
}

void KXineWidget::slotSetEq16k(int val)
{
	xine_set_param(m_xineStream, XINE_PARAM_EQ_16000HZ, -val);
}

void KXineWidget::slotSetVolumeGain(bool gain)
{
	int amp = 0;

	if (gain)
	{
		if (m_softwareMixer)
			amp = getVolume()*2;
		else
			amp = 200;
	}
	else
	{
		if (m_softwareMixer)
			amp = getVolume();
		else
			amp = 100;
	}
	xine_set_param(m_xineStream, XINE_PARAM_AUDIO_AMP_LEVEL, amp);

	m_volumeGain = gain;
}

/*************** dvd menus ******************/

void KXineWidget::slotMenuToggle()
{
	xine_event_t xev;
	xev.type = XINE_EVENT_INPUT_MENU1;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotMenuTitle()
{
	xine_event_t xev;
	xev.type = XINE_EVENT_INPUT_MENU2;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotMenuRoot()
{
	xine_event_t xev;
	xev.type = XINE_EVENT_INPUT_MENU3;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotMenuSubpicture()
{
	xine_event_t xev;
	xev.type = XINE_EVENT_INPUT_MENU4;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotMenuAudio()
{
	xine_event_t xev;
	xev.type = XINE_EVENT_INPUT_MENU5;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotMenuAngle()
{
	xine_event_t xev;
	xev.type = XINE_EVENT_INPUT_MENU6;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotMenuPart()
{
	xine_event_t xev;
	xev.type = XINE_EVENT_INPUT_MENU7;
	xev.data = NULL;
	xev.data_length = 0;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotDVDMenuLeft()
{
	xine_event_t xev;
	xev.data = NULL;
	xev.data_length = 0;
	xev.type = XINE_EVENT_INPUT_LEFT;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotDVDMenuRight()
{
	xine_event_t xev;
	xev.data = NULL;
	xev.data_length = 0;
	xev.type = XINE_EVENT_INPUT_RIGHT;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotDVDMenuUp()
{
	xine_event_t xev;
	xev.data = NULL;
	xev.data_length = 0;
	xev.type = XINE_EVENT_INPUT_UP;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotDVDMenuDown()
{
	xine_event_t xev;
	xev.data = NULL;
	xev.data_length = 0;
	xev.type = XINE_EVENT_INPUT_DOWN;

	xine_event_send(m_xineStream, &xev);
}

void KXineWidget::slotDVDMenuSelect()
{
	xine_event_t xev;
	xev.data = NULL;
	xev.data_length = 0;
	xev.type = XINE_EVENT_INPUT_SELECT;

	xine_event_send(m_xineStream, &xev);
}


/******** mouse hideing at fullscreen ****/

void KXineWidget::startMouseHideTimer()
{
	m_mouseHideTimer.start(5000);
}

void KXineWidget::stopMouseHideTimer()
{
	m_mouseHideTimer.stop();
}

void KXineWidget::slotHideMouse()
{
	if (cursor().shape() == Qt::ArrowCursor)
	{
		setCursor(QCursor(Qt::BlankCursor));
	}
}


/************************************************************
 *   Take a Screenshot                                      *
 ************************************************************/

QImage KXineWidget::getScreenshot() const
{
	uchar *rgbPile = NULL;
	int width, height;
	double scaleFactor;

	getScreenshot(rgbPile, width, height, scaleFactor);

	if (!rgbPile) return QImage();

	QImage screenShot(rgbPile, width, height, 32, 0, 0, QImage::IgnoreEndian);
	if (scaleFactor >= 1.0)
		width = (int)((double) width * scaleFactor + 0.5);
	else
		height = (int)((double) height / scaleFactor + 0.5);

	debugOut(QString("Screenshot: scale picture from %1x%2 to %3x%4").arg(screenShot.width()).arg(screenShot.height()).arg(width).arg(height));
	screenShot = screenShot.smoothScale(width, height);

	delete []rgbPile;
	return screenShot;
}

void KXineWidget::getScreenshot(uchar*& rgb32BitData, int& videoWidth, int& videoHeight, double& scaleFactor) const
{

	uint8_t   *yuv = NULL, *y = NULL, *u = NULL, *v =NULL;

	int        width, height, ratio, format;
	// double     desired_ratio, image_ratio;

	if (!xine_get_current_frame(m_xineStream, &width, &height, &ratio, &format, NULL))
		return;

	yuv = new uint8_t[((width+8) * (height+1) * 2)];
	if (yuv == NULL)
	{
		errorOut("Not enough memory to make screenshot!");
		return;
	}

	xine_get_current_frame(m_xineStream, &width, &height, &ratio, &format, yuv);

	videoWidth = width;
	videoHeight = height;

	/*
	 * convert to yv12 if necessary
	 */

	switch (format)
	{
		case XINE_IMGFMT_YUY2:
		{
			uint8_t *yuy2 = yuv;

			yuv = new uint8_t[(width * height * 2)];
			if (yuv == NULL)
			{
				errorOut("Not enough memory to make screenshot!");
				return;
			}
			y = yuv;
			u = yuv + width * height;
			v = yuv + width * height * 5 / 4;

			yuy2Toyv12 (y, u, v, yuy2, width, height);

			delete [] yuy2;
		}
		break;
		case XINE_IMGFMT_YV12:
		y = yuv;
		u = yuv + width * height;
		v = yuv + width * height * 5 / 4;

		break;
		default:
		{
			warningOut(QString("Screenshot: Format %1 not supportet!").arg((char*)&format));
			delete [] yuv;
			return;
		}
	}

	/*
	 * convert to rgb
	 */

	rgb32BitData = yv12ToRgb (y, u, v, width, height);


	// image_ratio = (double) height / (double) width;

	/*
	  switch (ratio)
	  {
	    case XINE_VO_ASPECT_ANAMORPHIC:
	      debugOut("Screenshot: got video aspect: anamorphic");
	      desired_ratio = 16.0 /9.0;
	      break;
	    case XINE_VO_ASPECT_DVB:
	      debugOut("Screenshot: got video aspect: 2.11:1");
	      desired_ratio = 2.11/1.0;
	      break;
	    case XINE_VO_ASPECT_4_3:
	      debugOut("Screenshot: got video aspect: 4:3");
	      desired_ratio = 4.0 / 3.0;
	      break;
	    default:
	      warningOut(QString("Screenshot: Unknown aspect ratio: %1 - using 4:3").arg(ratio));
	    case XINE_VO_ASPECT_SQUARE:
	      debugOut("Screenshot: got video aspect: 1:1");
	      desired_ratio = image_ratio;
	      break;
	  }
	*/

	debugOut(QString("Screenshot: using scale factor: %1").arg(m_videoAspect));
	scaleFactor = m_videoAspect;

	delete [] yuv;
}


/************************************************
 *             HELPER FUNCTIONS                 *
 ************************************************/

QTime KXineWidget::msToTime(int msec)
{
	QTime t;
	t = t.addMSecs(msec);
	return t;
}

#ifdef USE_QT_ONLY
QString KXineWidget::i18n(const char *text)
{
	return QApplication::tr(text);
}
#endif

void KXineWidget::debugOut (QString qsDebug)
{
#ifdef USE_QT_ONLY
	QString qsDebugging = QString ("Debug: ") + qsDebug +"\n";
	printf ((const char *)qsDebugging);
#else
	kdDebug() << "KXineWidget: " << (const char *)qsDebug << "\n";
#endif
}
void KXineWidget::errorOut (QString qsError)
{
#ifdef USE_QT_ONLY
	QString qsErroring = QString ("Error: ") + qsError+ "\n";
	printf ((const char *)qsErroring);
#else
	kdError() << "KXineWidget: " << (const char *)qsError << "\n";
#endif
}
void KXineWidget::warningOut (QString qsWarning)
{
#ifdef USE_QT_ONLY
	QString qsWarninging = QString ("Warning: ") + qsWarning + "\n";
	printf ((const char *)qsWarninging);
#else
	kdWarning() << "KXineWidget: " << (const char *)qsWarning << "\n";
#endif
}


/************************************************************
 *   Helpers to convert yuy and yv12 frames to rgb          *
 *   code from gxine modified for 32bit output              *
 *   Copyright (C) 2000-2003 the xine project               *
 ************************************************************/

void KXineWidget::yuy2Toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height)
{

	int    i, j, w2;

	w2 = width / 2;

	for (i = 0; i < height; i += 2)
	{
		for (j = 0; j < w2; j++)
		{
			/*
			 * packed YUV 422 is: Y[i] U[i] Y[i+1] V[i]
			 */
			*(y++) = *(input++);
			*(u++) = *(input++);
			*(y++) = *(input++);
			*(v++) = *(input++);
		}

		/*
		 * down sampling
		 */

		for (j = 0; j < w2; j++)
		{
			/*
			 * skip every second line for U and V
			 */
			*(y++) = *(input++);
			input++;
			*(y++) = *(input++);
			input++;
		}
	}
}


uchar* KXineWidget::yv12ToRgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height)
{
	/*
	 *   Create rgb data from yv12
	 */

#define clip_8_bit(val)              \
{                                    \
   if (val < 0)                      \
      val = 0;                       \
   else                              \
      if (val > 255) val = 255;      \
}

	int     i, j;

	int     y, u, v;
	int     r, g, b;

	int     sub_i_uv;
	int     sub_j_uv;

	int     uv_width, uv_height;

	uchar *rgb;

	uv_width  = width / 2;
	uv_height = height / 2;

	rgb = new uchar[(width * height * 4)]; //qt needs a 32bit align
	if (!rgb)
	{
		//    kdError(555) << "Not enough memory!" << endl;
		return NULL;
	}

	for (i = 0; i < height; ++i)
	{
		/*
		 * calculate u & v rows
		 */
		sub_i_uv = ((i * uv_height) / height);

		for (j = 0; j < width; ++j)
		{
			/*
			 * calculate u & v columns
			 */
			sub_j_uv = ((j * uv_width) / width);

			/***************************************************
			 *
			 *  Colour conversion from
			 *    http://www.inforamp.net/~poynton/notes/colour_and_gamma/ColorFAQ.html#RTFToC30
			 *
			 *  Thanks to Billy Biggs <vektor@dumbterm.net>
			 *  for the pointer and the following conversion.
			 *
			 *   R' = [ 1.1644         0    1.5960 ]   ([ Y' ]   [  16 ])
			 *   G' = [ 1.1644   -0.3918   -0.8130 ] * ([ Cb ] - [ 128 ])
			 *   B' = [ 1.1644    2.0172         0 ]   ([ Cr ]   [ 128 ])
			 *
			 *  Where in xine the above values are represented as
			 *
			 *   Y' == image->y
			 *   Cb == image->u
			 *   Cr == image->v
			 *
			 ***************************************************/

			y = src_y[(i * width) + j] - 16;
			u = src_u[(sub_i_uv * uv_width) + sub_j_uv] - 128;
			v = src_v[(sub_i_uv * uv_width) + sub_j_uv] - 128;

			r = (int)((1.1644 * (double)y) + (1.5960 * (double)v));
			g = (int)((1.1644 * (double)y) - (0.3918 * (double)u) - (0.8130 * (double)v));
			b = (int)((1.1644 * (double)y) + (2.0172 * (double)u));

			clip_8_bit (r);
			clip_8_bit (g);
			clip_8_bit (b);

			rgb[(i * width + j) * 4 + 0] = b;
			rgb[(i * width + j) * 4 + 1] = g;
			rgb[(i * width + j) * 4 + 2] = r;
			rgb[(i * width + j) * 4 + 3] = 0;

		}
	}

	return rgb;
}


