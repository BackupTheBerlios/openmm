/***************************************************************************
                           videowindow.cpp 
                           -------------------
    begin                : Fre Apr 18 2003
    revision             : $Revision: 1.81 $
    last modified        : $Date: 2006/01/13 05:04:19 $ by $Author: qdvdauthor $
    copyright            : (C) 2003-2004 by J. Kofler
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


#define USE_QT_ONLY 1

#ifndef USE_QT_ONLY

#include <klocale.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kapplication.h>

#else

#include <qmessagebox.h>
#include <qapplication.h>

#endif

#include <qdir.h>
#include <qcursor.h>
#include <qimage.h>

#include <string.h>
#include <stdlib.h>
#include <xine/xineutils.h>

#include "qxinewidget.h"

#ifndef USE_QT_ONLY
#include "qxinewidget.moc"
#endif

#include <X11/keysym.h>
#ifdef HAVE_XTEST
#include <X11/extensions/XTest.h>
#endif


#define TIMER_EVENT_PLAYBACK_FINISHED 100
#define TIMER_EVENT_NEW_CHANNELS      101
#define TIMER_EVENT_NEW_TITLE         102
#define TIMER_EVENT_NEW_INFO          103
#define TIMER_EVENT_NEW_PROGRESS_INFO 104
#define TIMER_EVENT_CHANGE_CURSOR     105
#define TIMER_EVENT_GRAB_KEYBOARD     106
#define TIMER_EVENT_NEW_MRL_REFERENCE 107
#define TIMER_EVENT_NEW_XINE_MESSAGE  108
#define TIMER_EVENT_RESTART_PLAYBACK  200
#define TIMER_EVENT_RESIZE_PARENT     300


QXineWidget::QXineWidget(QWidget *parent, void *pXineEngine, const char *name, const QString& prefAudio, const QString& prefVideo, bool verbose, bool startManual)
             : JAMStreamPlayer(parent,name), QThread()
{
	initMe (prefAudio, prefVideo, verbose, startManual);
	if (pXineEngine)	{
		xineEngine = (xine_t *)pXineEngine;
		m_bOwnXineEngine = false;
	}
}

QXineWidget::QXineWidget(QWidget *parent, const char *name, const QString& prefAudio, const QString& prefVideo, bool verbose, bool startManual)
             : JAMStreamPlayer(parent,name), QThread()
{
	initMe (prefAudio, prefVideo, verbose, startManual);
}

void QXineWidget::initMe (const QString& prefAudio, const QString& prefVideo, bool verbose, bool startManual)
{
  if (!XInitThreads ())
  {
    QApplication::beep();
    messageBoxError(0, i18n("XInitThreads failed!"));
    return;
  }
  xineEngine = NULL; xineStream = NULL; audioDriver = NULL; videoDriver = NULL;
  eventQueue = NULL; xinePost = NULL;  xineRunning = false;
  xineDisplay = NULL; eventThread = 0; currentZoom = 100;
  startXineManual = startManual;
  visualPluginName = QString::null;
  devicePath = QString::null;
  xineVerbose = verbose;
  autoresizeEnabled = false; videoFrameWidth = 0; videoFrameHeight = 0;
  m_bInitialising = false;	// that'll make sure we initialise it only once
  m_bIsXineInitialized = false; // This is a flag showing that initialising process is done.
  m_bOwnXineEngine = true;	// The default is that we create a new xineEngine ...

  
  preferedAudio = prefAudio;
  preferedVideo = prefVideo;

  setPaletteBackgroundColor(QColor(0,0,0));

  playMode = NORMAL_PLAY;

  //F.J.Cruz
//   connect(&posTimer, SIGNAL(timeout()), this, SLOT(slotGetPositionByTime()));
  connect(&posTimer, SIGNAL(timeout()), this, SLOT(slotGetPosition()));
  connect(&lengthInfoTimer, SIGNAL(timeout()), this, SLOT(slotEmitLengthInfo()));
  connect(&screensaverTimer, SIGNAL(timeout()), this, SLOT(slotFakeKeyEvent()));
  connect(&mouseHideTimer, SIGNAL(timeout()), this, SLOT(slotHideMouse()));

  setMouseTracking( true );
  setUpdatesEnabled( false ); //prevent qt overwrites videowin black
}

QXineWidget::~QXineWidget()
{
  /* "careful" shutdown, maybe xine initialization was not successful */
  xineRunning = false;

  /* stop all timers */
  posTimer.stop();
  screensaverTimer.stop();
  mouseHideTimer.stop();

  if (xineStream)
  {
    xine_close(xineStream);
  }

  if (running())
  {

    XEvent ev;
    ev.type = Expose;
    ev.xexpose.display = xineDisplay;
    ev.xexpose.window = winId();
    ev.xexpose.x = x();
    ev.xexpose.y = y();
    ev.xexpose.width = width();
    ev.xexpose.height = height();
    ev.xexpose.count = 0;

    XSendEvent( x11Display(), winId(), False, ExposureMask, &ev );  /* send a fake expose event */
    XFlush( x11Display() );

    if( !wait(1000) )  /* wait a second for thread exiting */
    {
       warningOut("XEvent thread don't exit. Terminating it...");
       terminate();
    }
  }

  debugOut("Shut down xine engine\n");

  if (xineDisplay)
     XLockDisplay( xineDisplay );

  if (xinePost)
  {
    debugOut(QString("Dispose visual plugin: %1").arg(visualPluginName));
    postAudioSource = xine_get_audio_source (xineStream);
    xine_post_wire_audio_port (postAudioSource, audioDriver);
    xine_post_dispose (xineEngine, xinePost);
  }  
  if (eventQueue)
     xine_event_dispose_queue(eventQueue);
  if (xineStream)   
     xine_dispose(xineStream);
  if (audioDriver)   
     xine_close_audio_driver(xineEngine, audioDriver);
  if (videoDriver)   
     xine_close_video_driver(xineEngine, videoDriver);
  if ( (xineEngine) && (m_bOwnXineEngine) )
     xine_exit(xineEngine);
  xineEngine = NULL;

  if (xineDisplay)
    XUnlockDisplay( xineDisplay ); 

 /*
  * FIXME
  * Removing XCloseDisplay() seems to be a workaround for the crash after closing on some systems...
  * We're looking for a better solution...
  */   
 /* if (xineDisplay)
    XCloseDisplay(xineDisplay); */  /* close xine display */
  xineDisplay = NULL;

  debugOut("xine closed\n");
}

/* Helper functions ... */
QString QXineWidget::i18n(const char *text)
{
#ifdef USE_QT_ONLY
	return tr (text);
#else
	return ::i18n(text);
#endif
}

void QXineWidget::debugOut (QString qsDebug)
{
#ifdef USE_QT_ONLY
	qsDebug = qsDebug;
//	QString qsDebugging = QString ("Debug: ") + qsDebug + QString ("\n");
//	printf ((const char *)qsDebugging);
#else
	kdDebug(555) << (const char *)qsDebug << "\n";
#endif
}
void QXineWidget::errorOut (QString qsError)
{
#ifdef USE_QT_ONLY
	QString qsErroring = QString ("Error: ") + qsError;
//	printf ((const char *)qsErroring);
	qsErroring = qsErroring;
#else
	kdError(555) << (const char *)qsError << "\n";
#endif
}
void QXineWidget::warningOut (QString qsWarning)
{
#ifdef USE_QT_ONLY
	QString qsWarninging = QString ("Warning: ") + qsWarning + "\n";
//	printf ((const char *)qsWarninging);
	qsWarninging = qsWarninging;
#else
	kdWarning(555) << (const char *)qsWarning << "\n";
#endif
}

void QXineWidget::postEvent(QObject *pObject,QEvent *pEvent)
{
#ifdef USE_QT_ONLY
	QApplication::postEvent (pObject, pEvent);
#else
	KApplication::kApplication()->postEvent(pObject, pEvent);
#endif
}

void QXineWidget::messageBoxError(QWidget *pWidget, QString qsMessage)
{
#ifdef USE_QT_ONLY
    QMessageBox::about(pWidget, qsMessage, qsMessage);
#else
    KMessageBox::error(pWidget, qsMessage);
#endif
}


void QXineWidget::SaveXineConfig()
{
  if (!devicePath.isNull())
  {
    debugOut("Set CD/VCD/DVD path back");
    xine_cfg_entry_t config;

    xine_config_lookup_entry (xineEngine, "input.cdda_device", &config);
    if ( devicePath == config.str_value )
    {
      config.str_value = (char*)cachedCDPath.latin1();
      xine_config_update_entry (xineEngine, &config);
    }

    xine_config_lookup_entry (xineEngine, "input.vcd_device", &config);
    if ( devicePath == config.str_value )
    {
      config.str_value = (char*)cachedVCDPath.latin1();
      xine_config_update_entry (xineEngine, &config);
    }  

    xine_config_lookup_entry (xineEngine, "input.dvd_device", &config);
    if ( devicePath == config.str_value )
    {
      config.str_value = (char*)cachedDVDPath.latin1();
      xine_config_update_entry (xineEngine, &config);
    }  
  }  
    
  debugOut(QString("Save %1\n").arg(configFile));
  xine_config_save(xineEngine, configFile);
}  


/***************************************************
 *         CALLBACKS
 ***************************************************/

void QXineWidget::DestSizeCallback(void* p, int /*video_width*/, int /*video_height*/, double /*video_aspect*/,
                       int* dest_width, int* dest_height, double* dest_aspect)
{
  if (p == NULL) return;
  QXineWidget* vw = (QXineWidget*) p;
  
  *dest_width = vw->width();
  *dest_height = vw->height();
  *dest_aspect = vw->displayRatio;
}

void QXineWidget::FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y)

{
  if (p == NULL) return;
  QXineWidget* vw = (QXineWidget*) p;
  
  *dest_x = 0;
  *dest_y = 0 ;
  *dest_width = vw->width();
  *dest_height = vw->height();
  *win_x = vw->globX;
  *win_y = vw->globY;
  *dest_aspect = vw->displayRatio;

 /* correct size with video aspect */
  if (video_aspect >= vw->displayRatio)
    video_width  = (int) ( (double) (video_width * video_aspect / vw->displayRatio + 0.5) );
   else
    video_height = (int) ( (double) (video_height * vw->displayRatio / video_aspect) + 0.5);

 /*** set the size of main window (KMP) to fit the size of video stream ***/
  if (vw->autoresizeEnabled)
  {
    if ( (video_width != vw->videoFrameWidth) || (video_height != vw->videoFrameHeight) )
    {
      if ( (vw->parentWidget()) && (!vw->parentWidget()->isFullScreen()) && (vw->posTimer.isActive()) && (video_width > 0) && (video_height > 0) )
      {
        vw->videoFrameWidth = video_width;
        vw->videoFrameHeight = video_height;

        vw->newParentSize = vw->parentWidget()->size() - QSize((vw->width() - video_width), vw->height() - video_height);
        debugOut (QString("Resize video window to: %1x%2 ...").arg(vw->newParentSize.width()).arg(vw->newParentSize.height()));
        debugOut(QString("...with video frame: %1x%2, video aspect ratio: %3").arg(video_width).arg(video_height).arg(video_aspect));

        /* we should not do a resize() inside a xine thread,
           but post an event to the main thread */
	vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_RESIZE_PARENT ) );
      }
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

void QXineWidget::XineEventListener(void *p, const xine_event_t* xineEvent)
{

  if (p == NULL) return;
  QXineWidget* vw = (QXineWidget*) p;

 // KApplication::kApplication()->lock(); /* lock QT event thread */
  
  switch (xineEvent->type)
  {
    case XINE_EVENT_UI_PLAYBACK_FINISHED:
       {
         debugOut("xine event: playback finished");
         vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_PLAYBACK_FINISHED ) );
         break; 
       }
    case XINE_EVENT_UI_CHANNELS_CHANGED:  /* new channel informations */
      {
        debugOut("xine event: channels changed");
        int i,j,channels;
        char* lang = new char[128];     /* more than enough */
        QString num;

        vw->audioCh.clear();
        vw->subCh.clear();

        /*** get audio channels  ***/
        channels = QMAX(6, xine_get_stream_info(vw->xineStream, XINE_STREAM_INFO_MAX_AUDIO_CHANNEL));
        for(i = 0; i < channels; i++)
        {
          if (xine_get_audio_lang(vw->xineStream, i, lang))
            vw->audioCh << lang;
           else
            vw->audioCh << vw->i18n("Ch ") + num.setNum(i+1);
        }     

        /*** get subtitle channels ***/
    
        channels = QMAX(6, xine_get_stream_info(vw->xineStream, XINE_STREAM_INFO_MAX_SPU_CHANNEL));
        for(j = 0; j < channels; j++)
        {
          if (xine_get_spu_lang(vw->xineStream, j, lang))
            vw->subCh << lang;
           else
            vw->subCh << vw->i18n("Ch ") + num.setNum(j+1);
        }    

        delete lang;  /* or not? */

        vw->currentAudio = xine_get_param(vw->xineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL);
        vw->currentSub = xine_get_param(vw->xineStream, XINE_PARAM_SPU_CHANNEL);
 
        if (vw->currentAudio > (i-1))
        {
          vw->slotSetAudioChannel(0);
          vw->currentAudio = -1;
        }
          
        if (vw->currentSub > (j-1))
        {
          vw->slotSetSubtitleChannel(0);
          vw->currentSub = -1;
         }

         vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_CHANNELS ) );
         break;
      }
    case XINE_EVENT_UI_SET_TITLE:  /* set new title */
       {
        debugOut("xine event: ui set title");
        xine_ui_data_t* xd = (xine_ui_data_t*)xineEvent->data;
        vw->currentTitle = QString::fromLocal8Bit( (char*)xd->str );
        vw->trackInfoRow = vw->currentTitle + " ** " + vw->GetStreamInfo() + vw->extraInfo;

        vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_TITLE ) );
        vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_INFO ) );
        break;
       }
     case XINE_EVENT_PROGRESS:
       {
        debugOut("xine event: progress info");
        xine_progress_data_t* pd = (xine_progress_data_t*)xineEvent->data;

        QString perc;
        perc.setNum(pd->percent);
        perc.append(" %");
        
        vw->currentProgress = pd->description;
        vw->currentProgress.append("  ");
        vw->currentProgress.append(perc);
        
        if (pd->percent != 100)
          vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_PROGRESS_INFO ) );
         else
          vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_INFO ) );

        break;
        }
     case XINE_EVENT_DROPPED_FRAMES:
      {
        debugOut("xine event: dropped frames");
        xine_dropped_frames_t* dropped = (xine_dropped_frames_t*)xineEvent->data;
        
        warningOut(QString("skipped frames: %1% discarded frames: %2%").arg( dropped->skipped_frames/10).arg(dropped->discarded_frames/10));

        break;
       }  
     case XINE_EVENT_SPU_BUTTON:
       {
         debugOut("xine event: spu button");
         xine_spu_button_t* button = (xine_spu_button_t*)xineEvent->data;

         if (button->direction == 1) /* enter a button */
         {          
           debugOut("DVD Menu: Mouse entered button");
           vw->DVDButtonEntered = true;
         }  
         else
         {
           debugOut("DVD Menu: Mouse leaved button");
           vw->DVDButtonEntered = false;
         }  

         vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_CHANGE_CURSOR ) );
         break;
       }  
     case XINE_EVENT_UI_NUM_BUTTONS:
       {
         debugOut("xine event: ui num buttons");
         xine_ui_data_t *buttons = (xine_ui_data_t *)xineEvent->data;
         if(buttons->num_buttons >= 1)
           vw->DVDMenuEntered = true; 
          else
           vw->DVDMenuEntered = false;
    
         debugOut(QString("DVD Menu: Num buttons=%1").arg(buttons->num_buttons));
         vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_GRAB_KEYBOARD ) );
         break;
       }
     case XINE_EVENT_MRL_REFERENCE:
       {
         debugOut("xine event: mrl reference");
         xine_mrl_reference_data_t* mrldata = (xine_mrl_reference_data_t*)xineEvent->data;
         vw->newMrlReference = mrldata->mrl;

         vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_MRL_REFERENCE ) );
         break;
       }
     case XINE_EVENT_UI_MESSAGE:
       {
         debugOut("xine event: xine message");
         
         xine_ui_message_data_t *data = (xine_ui_message_data_t *)xineEvent->data;
         QString message;

         /* some code take from the xine-ui - Copyright (C) 2000-2003 the xine project */
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
               delete d;
               break;
             }
           case XINE_MSG_GENERAL_WARNING:
            {
              message = vw->i18n("General Warning: \n");

              if(data->explanation)
                message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);
              else
                message = message + vw->i18n("No Informations available.");

              break;
            }  
          case XINE_MSG_UNKNOWN_HOST:
            {
              message = vw->i18n("The host you're trying to connect is unknown.\nCheck the validity of the specified hostname. ");
                if(data->explanation)
                  message = message + "(" + ((char *) data + data->parameters) + ")";
              break;
            }
          case XINE_MSG_UNKNOWN_DEVICE:
            {
              message = vw->i18n("The device name you specified seems invalid. ");
              if(data->explanation)
                message = message + "(" + ((char *) data + data->parameters) + ")";
              break;
            }
          case XINE_MSG_NETWORK_UNREACHABLE:
            {
              message = vw->i18n("The network looks unreachable.\nCheck your network setup and the server name. ");
              if(data->explanation)
                message = message + "(" + ((char *) data + data->parameters) + ")";
              break;
            }
          case XINE_MSG_CONNECTION_REFUSED:
            {
              message = vw->i18n("The connection was refused.\nCheck the host name. ");
              if(data->explanation)
                message = message + "(" + ((char *) data + data->parameters) + ")";
              break;
            }
          case XINE_MSG_FILE_NOT_FOUND:
            {
              message = vw->i18n("The specified file or url was not found. Please check it. ");
              if(data->explanation)
                message = message + "(" + ((char *) data + data->parameters) + ")";
              break;
            }  
          case XINE_MSG_READ_ERROR:
            {
              message = vw->i18n("The source can't be read.\nMaybe you don't have enough rights for this, or source doesn't contain data (e.g: no disc in drive). ");
              if(data->explanation)
                message = message + "(" + ((char *) data + data->parameters) + ")";
              break;
            }  
          case XINE_MSG_LIBRARY_LOAD_ERROR:
            {
              message = vw->i18n("A problem occur while loading a library or a decoder: ");
              if(data->explanation)
                message = message + ((char *) data + data->parameters);
              break;
            }  
          case XINE_MSG_ENCRYPTED_SOURCE:
            {
              message = vw->i18n("The source seems encrypted, and can't be read. ");
              if (vw->currentMRL.contains("dvd:/"))
                message = message + vw->i18n("\nYour DVD is probably crypted. According to your country laws, you can or can't use libdvdcss to be able to read this disc. ");
              if(data->explanation)
                message = message + "(" + ((char *) data + data->parameters) + ")";
              break;
            }
          default:
            {
              message = vw->i18n("Unkown error: \n");
              if(data->explanation)
                message = message + ((char *) data + data->explanation) + " " + ((char *) data + data->parameters);
              break;
            }
          }

         vw->xineMessage = message;
         vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_XINE_MESSAGE ) );
         break;
       }
     default:
       {
        // debugOut("xine event: unhandled type %1").arg(xineEvent->type);
         break;
       }    
   }

 //  KApplication::kApplication()->unlock();
}


void QXineWidget::timerEvent( QTimerEvent* tevent )
{
  switch ( tevent->timerId() )
  {
    case TIMER_EVENT_PLAYBACK_FINISHED:
    {
      emit signalPlaybackFinished();
      posTimer.stop();
      break;
    }
    case TIMER_EVENT_NEW_CHANNELS:
    {
      emit signalNewChannels( audioCh, subCh, currentAudio, currentSub);
      break;
    }
    case TIMER_EVENT_NEW_TITLE:
    {
       emit signalNewTitle( currentTitle );
       break;
    }
    case TIMER_EVENT_NEW_INFO:
    {
       emit signalNewInfo( trackInfoRow );
       break;
    }
    case TIMER_EVENT_NEW_PROGRESS_INFO:
    {
       emit signalNewInfo( currentProgress );
       break;
    }
    case TIMER_EVENT_CHANGE_CURSOR:
    {
      if (DVDButtonEntered)
        setCursor(QCursor(Qt::PointingHandCursor));
       else
        setCursor(QCursor(Qt::ArrowCursor));
      break;
    }      
    case TIMER_EVENT_GRAB_KEYBOARD:
    {
      if (DVDMenuEntered)
        grabKeyboard();
       else
        releaseKeyboard();
      break;  
    }      
    case TIMER_EVENT_NEW_MRL_REFERENCE:
    {
       emit signalNewMrlReference( newMrlReference );
       break;
    }
    case TIMER_EVENT_NEW_XINE_MESSAGE:
    {
       emit signalNewXineMessage();
       break;
    }
    case TIMER_EVENT_RESTART_PLAYBACK:
    {
      PlayMRL( currentMRL, currentTitle, false );
      slotChangePosition( savedPos );
      break;
    }
    case TIMER_EVENT_RESIZE_PARENT:
    {
//printf ("QXineWidget::timerEvent <%d><%d>\n", newParentSize.width(), newParentSize.height());
      parentWidget()->resize( newParentSize );
      break;
    }    
    default: break;
  }
}      


/******************* new video driver *********************/

void QXineWidget::VideoDriverChangedCallback(void* p, xine_cfg_entry_t* entry)
{
  if (p == NULL) return;
  QXineWidget* vw = (QXineWidget*) p;

  int pos, time, length;
  
  debugOut(QString("New video driver: %1").arg(entry->enum_values[entry->num_value]));

  bool playing = false;
  if (xine_get_status(vw->xineStream) == XINE_STATUS_PLAY)
  {
    playing = true;
    vw->savedPos = 0;
    if ( xine_get_pos_length(vw->xineStream, &pos, &time, &length) )
     vw->savedPos = pos;
  }   

  xine_close(vw->xineStream);

  if (vw->xinePost)
  {
     debugOut(QString("Dispose visual plugin: %1").arg(vw->visualPluginName));
     vw->postAudioSource = xine_get_audio_source (vw->xineStream);
     xine_post_wire_audio_port (vw->postAudioSource, vw->audioDriver);
     xine_post_dispose (vw->xineEngine, vw->xinePost);
     vw->xinePost = NULL;
  }
  
  xine_event_dispose_queue(vw->eventQueue);
  xine_dispose(vw->xineStream);
  xine_close_video_driver(vw->xineEngine, vw->videoDriver);
  vw->videoDriver = NULL;

  vw->videoDriver = xine_open_video_driver(vw->xineEngine,
                           entry->enum_values[entry->num_value], XINE_VISUAL_TYPE_X11,
                           (void *) &(vw->visual));


  if (!vw->videoDriver)
  {
     QApplication::beep();
     vw->trackInfoRow = vw->i18n("Error: Can't init new Video Driver %1 - using %2!").arg(entry->enum_values[entry->num_value]).arg(vw->videoDriverName);
     vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_INFO ) );
     playing = false;
     vw->videoDriver = xine_open_video_driver(vw->xineEngine,
                           vw->videoDriverName, XINE_VISUAL_TYPE_X11,
                           (void *) &(vw->visual));
  }
  else
  {
     vw->videoDriverName = entry->enum_values[entry->num_value];
     vw->trackInfoRow = vw->i18n("Using Video Driver: ") + vw->videoDriverName;
     vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_INFO ) );
  }

  vw->xineStream = xine_stream_new(vw->xineEngine, vw->audioDriver, vw->videoDriver);
  vw->eventQueue = xine_event_new_queue (vw->xineStream);
  xine_event_create_listener_thread(vw->eventQueue, &QXineWidget::XineEventListener, p);

  if (playing)
    vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_RESTART_PLAYBACK ) );
}


/*********************** new audio driver *************************/

void QXineWidget::AudioDriverChangedCallback(void* p, xine_cfg_entry_t* entry)
{
  if (p == NULL) return;
  QXineWidget* vw = (QXineWidget*) p;

  int pos, time, length;

  debugOut(QString("New audio driver: %1").arg(entry->enum_values[entry->num_value]));

  bool playing = false;
  if (xine_get_status(vw->xineStream) == XINE_STATUS_PLAY)
  {
    playing = true;
    vw->savedPos = 0;
    if ( xine_get_pos_length(vw->xineStream, &pos, &time, &length) )
     vw->savedPos = pos;
  }

  xine_close(vw->xineStream);

  if (vw->xinePost)
  {
     debugOut(QString("Dispose visual plugin: %1").arg(vw->visualPluginName));
     vw->postAudioSource = xine_get_audio_source (vw->xineStream);
     xine_post_wire_audio_port (vw->postAudioSource, vw->audioDriver);
     xine_post_dispose (vw->xineEngine, vw->xinePost);
     vw->xinePost = NULL;
  }

  xine_event_dispose_queue(vw->eventQueue);
  xine_dispose(vw->xineStream);
  xine_close_audio_driver(vw->xineEngine, vw->audioDriver);
  vw->audioDriver = NULL;

  vw->audioDriver = xine_open_audio_driver(vw->xineEngine, entry->enum_values[entry->num_value], NULL);

  if (!vw->audioDriver)
  {
     QApplication::beep();
     vw->trackInfoRow = vw->i18n("Error: Can't init new Audio Driver %1 - using %2!").arg(entry->enum_values[entry->num_value]).arg(vw->audioDriverName);
     vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_INFO ) );
     playing = false;
     vw->audioDriver = xine_open_audio_driver(vw->xineEngine, vw->audioDriverName, NULL);
  }
  else
  {
     vw->audioDriverName = entry->enum_values[entry->num_value];
     vw->trackInfoRow = vw->i18n("Using Audio Driver: ") + vw->audioDriverName;
     vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_NEW_INFO ) );
  }

  vw->xineStream = xine_stream_new(vw->xineEngine, vw->audioDriver, vw->videoDriver);
  vw->eventQueue = xine_event_new_queue (vw->xineStream);
  xine_event_create_listener_thread(vw->eventQueue, &QXineWidget::XineEventListener, p);

  if (playing)
    vw->postEvent( vw, new QTimerEvent( TIMER_EVENT_RESTART_PLAYBACK ) );
}
    

/**********************************************
 *      EVENT LOOP
 *********************************************/

void QXineWidget::run()
{
  if (!m_bIsXineInitialized)	{
    if (!taskInitXine ())	{
      emit signalQuit();
      return;
    }	
  }

  debugOut("Start event loop...\n");

  XEvent event;

  while( xineRunning )
  {
     XNextEvent ( xineDisplay, &event );
     XLockDisplay( xineDisplay );

     if (event.type == Expose)
     {
       if (event.xexpose.count == 0)
       {
         xine_gui_send_vo_data (xineStream, XINE_GUI_SEND_EXPOSE_EVENT, &event);
       }
     }

     XUnlockDisplay( xineDisplay );
  }

  debugOut("Exiting event loop...\n");
}



/**********************************************************
 *           INIT XINE ENGINE
 *********************************************************/

void QXineWidget::polish()
{
  if (!startXineManual)  /* start xine engine automatically? */
  {
    if (!InitXine()) emit signalQuit(); /* no success */
  }
}

bool QXineWidget::InitXine()
{
	if (m_bInitialising)
		return true;
	m_bInitialising = true;
	// Obsolete but what the heck ...
	if (m_bIsXineInitialized)
		return true;
	/**** INIT XINE DISPLAY  ****/

  if (!XInitThreads ())
  {
    QApplication::beep();
    messageBoxError(0, i18n("XInitThreads failed!"));
    return false;
  }
  
//  xineDisplay = XOpenDisplay( getenv("DISPLAY") );
  xineDisplay = XOpenDisplay( NULL );

  if (!xineDisplay)
  {
    QApplication::beep();
    messageBoxError(0, i18n("Failed to connect to X-Server!"));
    return false;
  }

  xineScreen = DefaultScreen( xineDisplay );
  xineWindow = winId();

/**** INIT XINE ENGINE ****/
  XLockDisplay( xineDisplay );
  XSelectInput( xineDisplay, xineWindow, ExposureMask );

/* determine display aspect ratio  */
  double resHor = DisplayWidth( xineDisplay, xineScreen )*1000 / DisplayWidthMM( xineDisplay, xineScreen );
  double resVer = DisplayHeight( xineDisplay, xineScreen )*1000 / DisplayHeightMM( xineDisplay, xineScreen );

  displayRatio = resVer / resHor;
  if (QABS( displayRatio - 1.0 ) < 0.01) displayRatio   = 1.0;

  debugOut(QString("Display aspect ratio (v/h): %1\n").arg(displayRatio));
  XUnlockDisplay( xineDisplay );

  debugOut(QString("Using xine version %1\n").arg(xine_get_version_string()));

//  start ();
//  return true;
//}
//bool QXineWidget::taskInitXine()
//{
  if (m_bOwnXineEngine)	{	// If we supply a XineEngine already, we won't need to create one here ...
	  xineEngine = xine_new();
 
	  if (!xineEngine)
	  {
	    QApplication::beep();
	    messageBoxError(0, i18n("Can't init xine Engine!"));
	    return false;
	  }

	  if (xineVerbose)
	   xine_engine_set_param( xineEngine, XINE_ENGINE_PARAM_VERBOSITY, 99 );

	/* load configuration */

	  configFile = QDir::homeDirPath();
	  configFile.append("/.qdvdauthor/xine_config.ini");

	  bool bSave = false;
	  if (QFile::exists(configFile))
		xine_config_load (xineEngine, configFile);
	  else
		bSave = true;

	   debugOut("Post-init xine engine\n");
	   xine_init(xineEngine);

	  if (bSave)	{
	    warningOut(QString ("No config file found, will create \n%1\n").arg(configFile));
		// First we check if the directory exists or if we should create it first ...
		QString qsDirPath = QDir::homeDirPath() + QString ("/.qdvdauthor");
		QDir theDir(qsDirPath);
		if (!theDir.exists())
			theDir.mkdir (qsDirPath);
		xine_config_save  (xineEngine, configFile);
		}
    }
    start ();	/* Start the initialisation thread */
    return true;
}

bool QXineWidget::taskInitXine()
{
  static bool bInitialising = true;
    
  if (xineRunning) 
    return true;
  else if (!bInitialising)
    return true;
  bool cmdlAudioEnum = false, cmdlVideoEnum = false;

  GlobalPosChanged(); /* get global pos of the window */

/** set xine parameters **/

  const char* const* drivers = NULL;
  char** audioChoices = NULL;
  int i = 0;

  drivers = xine_list_audio_output_plugins (xineEngine);
  audioChoices = new char*[15];
  for(i = 0; i < 15; i++) audioChoices[i] = new char[10];
  audioChoices[0] = const_cast<char*>("auto");
  i = 0;
  while(drivers[i])
  {
    audioChoices[i+1] = (char*)drivers[i];
    if (preferedAudio == drivers[i]) cmdlAudioEnum = true;
    i++;
  }  
  audioChoices[i+1] = NULL;
  if (preferedAudio == "auto") cmdlAudioEnum = true;
  
  char* audioInfo = new char[200];
  strcpy(audioInfo, i18n("Audiodriver to use (default: auto)").local8Bit() );
  i = xine_config_register_enum(xineEngine, "gui.audiodriver", 0,
                   audioChoices, audioInfo, NULL, 10, &QXineWidget::AudioDriverChangedCallback, this);

  if (cmdlAudioEnum)
    audioDriverName = preferedAudio;
   else 
    audioDriverName = audioChoices[i];

  debugOut(QString("Use audio driver %1\n").arg(audioDriverName));

  i = 0;
  char** videoChoices = NULL;
  drivers = xine_list_video_output_plugins (xineEngine);
  videoChoices = new char*[15];
  for(i = 0; i < 15; i++) videoChoices[i] = new char[10];
  i = 0;
  videoChoices[0] = const_cast<char*>("auto");
  while(drivers[i])
  {
    videoChoices[i+1] = (char*)drivers[i];
    if (preferedVideo == drivers[i]) cmdlVideoEnum = true;
    i++;
  }
  videoChoices[i+1] = NULL;
  if (preferedVideo == "auto") cmdlVideoEnum = true;

  char* videoInfo = new char[200];
  strcpy( videoInfo, i18n("Videodriver to use (default: auto)").local8Bit() );
  i = xine_config_register_enum(xineEngine, "gui.videodriver", 0,
                   videoChoices, videoInfo, NULL, 10, &QXineWidget::VideoDriverChangedCallback, this);

  if (cmdlVideoEnum)
    videoDriverName = preferedVideo;
   else 
    videoDriverName = videoChoices[i];

  debugOut(QString("Use video driver %1\n").arg(videoDriverName));

/* init video driver */
  visual.display          = xineDisplay;
  visual.screen           = xineScreen;
  visual.d                = xineWindow;
  visual.dest_size_cb     = &QXineWidget::DestSizeCallback;
  visual.frame_output_cb  = &QXineWidget::FrameOutputCallback;
  visual.user_data        = (void*)this;

  debugOut("Init video driver\n");

  videoDriver = xine_open_video_driver(xineEngine,
               videoDriverName,  XINE_VISUAL_TYPE_X11,
               (void *) &(visual));
  if (!videoDriver)
  {
    QApplication::beep();
    messageBoxError(0, i18n("Can't init Video Driver!") + " (" + videoDriverName + ")");
    return false;
  }

/* init audio driver */  
  debugOut("Init audio driver\n");
  
  audioDriver = xine_open_audio_driver (xineEngine, audioDriverName, NULL);
  if (!audioDriver)
  {
    QApplication::beep();
    messageBoxError(0, i18n("Can't init Audio Driver!")+ " (" + audioDriverName + ")");
    return false;
  }

// debugOut("Open stream\n");
  
  xineStream  = xine_stream_new(xineEngine, audioDriver, videoDriver);
  if (!xineStream)
  {
    QApplication::beep();
    messageBoxError(0, i18n("Can't create a new xine Stream!"));
    return false;
  }
  
/** event handling **/

  eventQueue = xine_event_new_queue (xineStream);
  xine_event_create_listener_thread(eventQueue, &QXineWidget::XineEventListener, (void*)this);

/* set path to logo file */
  setLogoFile();

/** get a keycode for a faked key event to hold down screensaver **/

#ifdef HAVE_XTEST
int a,b,c,d;
  haveXTest = XTestQueryExtension(x11Display(), &a, &b, &c, &d);
  if (haveXTest)
    xTestKeycode = XKeysymToKeycode(x11Display(), XK_Shift_L);
  
#endif

       
  debugOut("xine init successful\n");
  xineRunning = true;
  bInitialising = true;
 /** start event thread **/
//  start(); 

//  parentWidget()->polish(); /* make sure gui will build correctly */
  m_bIsXineInitialized = true;

  return true;
}

void QXineWidget::setLogoFile()
{
	// virtual function, Reimplemented in VideoWindow
}

/************************************************
 *              PLAY MRL
 ************************************************/

void QXineWidget::play(QString mrl)
{
    PlayMRL(mrl, "", false);
}


bool QXineWidget::PlayMRL(const QString& mrl, const QString& title, bool returnInfo)
{
  lengthInfoTimer.stop();
  
 // if (xine_get_status(xineStream) == XINE_STATUS_PLAY)
 //   xine_stop(xineStream);

  emit signalNewInfo(i18n("Opening..."));
  setCursor(QCursor(Qt::WaitCursor));

  currentMRL = mrl;

 /* check for external subtitle file or save url */
  extraInfo = QString::null;
  QString ref;
  
  for (int i = 1; i <= mrl.contains('#'); i++)
  {
    ref = mrl.section('#', i, i);
    if (ref.section(':', 0, 0) == "subtitle")
      extraInfo = extraInfo + " ** " + i18n("Subtitles: ") + ref.section(':', 1);
    if (ref.section(':', 0, 0) == "save")
      extraInfo = extraInfo + " ** " + i18n("Save as: ") + ref.section(':', 1);
  }
 
  debugOut(QString("Playing: %1").arg(currentMRL.local8Bit()));

  if (!xine_open(xineStream, currentMRL.local8Bit() )) /** pass mrl local 8Bit encoded **/
  {
    SendXineError( returnInfo );
    setCursor(QCursor(Qt::ArrowCursor));
    return false;
  }  

/**** use visualization ? ****/

  if ( (xine_get_stream_info (xineStream, XINE_STREAM_INFO_HAS_AUDIO)) &&
       (!xine_get_stream_info (xineStream, XINE_STREAM_INFO_HAS_VIDEO)) )
  {

    if (visualPluginName && (!xinePost))
    {
      debugOut(QString("Init visual plugin: %1").arg(visualPluginName));
      xinePost = xine_post_init (xineEngine, visualPluginName, 0,
          &audioDriver,
          &videoDriver);

      postAudioSource = xine_get_audio_source (xineStream);
      postInput = (xine_post_in_t*)xine_post_input (xinePost, const_cast<char*>("audio in"));
      xine_post_wire (postAudioSource, postInput);
    }
  }
  else
  {
    if (xinePost)
    {
      debugOut(QString("Dispose visual plugin: %1").arg(visualPluginName));
      postAudioSource = xine_get_audio_source (xineStream);
      xine_post_wire_audio_port (postAudioSource, audioDriver);
      xine_post_dispose (xineEngine, xinePost);
      xinePost = NULL;
    }
  }
  
/*** play ***/

  if (!xine_play(xineStream, 0,0))
  {
    SendXineError( returnInfo );
    setCursor(QCursor(Qt::ArrowCursor));
    return false;
  }

/* do the stream have chapters ? */
  if (xine_get_stream_info(xineStream, XINE_STREAM_INFO_HAS_CHAPTERS))
    emit signalHasChapters(true);
   else
    emit signalHasChapters(false);


/** information requirement **/

  currentTitle = title;

  if ( (title.contains('.')) || (title.contains(":/")) ) /* tags was still read? */
  {
    QString metaTitle = QString::fromLocal8Bit( xine_get_meta_info(xineStream, XINE_META_INFO_TITLE) );
    if ( (metaTitle) && (!metaTitle.isEmpty()) )  //usable meta info
    {
      QString metaArtist = QString::fromLocal8Bit(xine_get_meta_info(xineStream, XINE_META_INFO_ARTIST));
      QString metaAlbum = QString::fromLocal8Bit(xine_get_meta_info(xineStream, XINE_META_INFO_ALBUM));
      QString metaTrack = ""; /* not implemented in xine-lib */
      QString metaInfo = metaString;
      metaInfo.replace( "artist", metaArtist );
      metaInfo.replace( "title",  metaTitle );
      metaInfo.replace( "album",  metaAlbum );
      metaInfo.replace( "track",  metaTrack );;
      currentTitle = metaInfo;
      if ((metaInfo != title) && (returnInfo))
      {
        emit signalMetaInfo(metaInfo);
      }
    }
    if (returnInfo)
    {
      QString length = GetLengthInfo();
      if (!length.isNull())
        emit signalLengthInfo( length );
       else
        lengthInfoTimer.start( 1000 ); /* wait for available track length info */
    }      
  }

  trackInfoRow = currentTitle;

  QString streamInfo = GetStreamInfo();
  if (returnInfo)
  {
     emit signalStreamInfo(streamInfo);
  }

  streamInfo = streamInfo + extraInfo;
                                                                                                        
  trackInfoRow.append( streamInfo.prepend(" ** ") );
  emit signalNewInfo(trackInfoRow);
   
  slotSetAudioChannel(0); //refresh channel info
  
  posTimer.start(500);

  setCursor(QCursor(Qt::ArrowCursor));
                
  emit signalShowOSD(currentTitle);

  return true;
}


/****** error processing ****/

void QXineWidget::SendXineError( bool returnInfo )
{
  QString error;
  int errCode = xine_get_error(xineStream);

  switch (errCode)
  {
    case XINE_ERROR_NO_INPUT_PLUGIN:    
    case XINE_ERROR_NO_DEMUX_PLUGIN:
    {
      error = i18n("No plugin found to handle this resource");
      break;
    }
    case XINE_ERROR_DEMUX_FAILED:
    {
      error = i18n("Resource seems to be broken");
      break;
    }
    case XINE_ERROR_MALFORMED_MRL:
    {
      error = i18n("Requested resource does not exist");
      break;
    }
    case XINE_ERROR_INPUT_FAILED:
    {
      error = i18n("Resource can not be opened");
      break;
    }
    default:
    {
      error = i18n("Generic error");
      break;
    }  
  }

  emit signalNewInfo( i18n("Error: ") + error );

  if (returnInfo)
  {
      emit signalStreamInfo( error );
      emit signalLengthInfo( i18n("*Error*") );
  }
}


/****  visual plugin **********/


void QXineWidget::GetVisualPlugins(QStringList& visuals) const
{
  const char* const* plugins = xine_list_post_plugins_typed(xineEngine, XINE_POST_TYPE_AUDIO_VISUALIZATION);

  for (int i = 0; plugins[i]; i++)
  {
    visuals << plugins[i];
  }
}   


/**************** change visualization plugin *****************/

void QXineWidget::SetVisualPlugin(const QString& visual)
{
  if ( visualPluginName == visual ) return;
  
  debugOut(QString("New visualization plugin: %1").arg(visual));

  if (visual == "none")
    visualPluginName = QString::null;
   else
    visualPluginName = visual;

  if (xinePost)
  {
    xine_post_out_t *pp;

    pp = xine_get_audio_source (xineStream);
    xine_post_wire_audio_port (pp, audioDriver);
    xine_post_dispose (xineEngine, xinePost);
    xinePost = NULL;
  }



  if ( (xine_get_status( xineStream ) == XINE_STATUS_PLAY)
       && (!xine_get_stream_info(xineStream, XINE_STREAM_INFO_HAS_VIDEO)) && (visualPluginName) )
  {

    xinePost = xine_post_init (xineEngine, visualPluginName, 0, &audioDriver, &videoDriver);
    postAudioSource = xine_get_audio_source(xineStream);
    postInput = (xine_post_in_t*)xine_post_input(xinePost, const_cast<char*>("audio in"));
    xine_post_wire( postAudioSource, postInput );
  }
}


/*****/


void QXineWidget::GetAutoplayPlugins(QStringList& autoPlayList) const
{
  char** pluginIds = NULL;
  int i = 0;

  pluginIds = (char**)xine_get_autoplay_input_plugin_ids(xineEngine);

  while(pluginIds[i])
  {
    autoPlayList << pluginIds[i];
    
    autoPlayList << xine_get_input_plugin_description(xineEngine, pluginIds[i]);    
    i++;
  }
}

 
bool QXineWidget::GetAutoplayPluginMrl(const QString& plugin, QStringList& list)
{
  char** mrls = NULL;
  int num;
  int i = 0;

  mrls = xine_get_autoplay_mrls(xineEngine, plugin, &num);

  if (mrls)
  {
    while (mrls[i])
      {
        list << mrls[i];
        i++;
      }  

    return true;
   } 
   else 
   {
     QString error(i18n("Error: No "));
     error.append(plugin);
     error.append(i18n(", or wrong path to device."));
     signalNewInfo(error);
     return false;
   }
}


void QXineWidget::slotSetVolume(int vol)
{
  xine_set_param(xineStream, XINE_PARAM_AUDIO_VOLUME, vol);
}


void QXineWidget::mouseMoveEvent(QMouseEvent* mev)
{
   if (!xineRunning) return;

 //  debugOut("mouse move event");
   
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

    xine_gui_send_vo_data (xineStream,
               XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO,
                (void*)&rect);

    event.type        = XINE_EVENT_INPUT_MOUSE_MOVE;
    event.data        = &input;
    event.data_length = sizeof(input);
    input.button      = 0; 
    input.x           = rect.x;
    input.y           = rect.y;
    xine_event_send (xineStream, &event);
}


void QXineWidget::mousePressEvent(QMouseEvent* mev)
{
   if (!xineRunning) return;
   int cur = cursor().shape();

 //  debugOut("mouse press event");

   if (mev->button() == Qt::MidButton)
     emit signalToggleFullscreen();

   if (mev->button() == Qt::RightButton)
   {
     if ( (cur == Qt::ArrowCursor) || (cur == Qt::BlankCursor) )
     {
       emit signalShowContextMenu(mev->globalPos());
       return;
     }  
   }  

   if (mev->button() == Qt::LeftButton)
   {
     if ( (cur == Qt::ArrowCursor) || (cur == Qt::BlankCursor) )
     {
       emit signalShowFullscreenPanel(mev->globalPos());
       return;
     }  

     x11_rectangle_t   rect;
     xine_event_t      event;
     xine_input_data_t input;

     rect.x = mev->x();
     rect.y = mev->y();
     rect.w = 0;
     rect.h = 0;

     xine_gui_send_vo_data (xineStream,
                XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO,
                   (void*)&rect);

     event.type        = XINE_EVENT_INPUT_MOUSE_BUTTON;
     event.data        = &input;
     event.data_length = sizeof(input);
     input.button      = 1;
     input.x           = rect.x;
     input.y           = rect.y;
     xine_event_send (xineStream, &event);
   }
}


void QXineWidget::PlayNextChapter() const
{

  xine_event_t xev;

  xev.type = XINE_EVENT_INPUT_NEXT;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
}  


void QXineWidget::PlayPreviousChapter() const
{

  xine_event_t xev;

  xev.type = XINE_EVENT_INPUT_PREVIOUS;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
}  


void QXineWidget::slotStopPlayback()
{
  posTimer.stop();
  xine_stop(xineStream);
}

void QXineWidget::SetDevice(const QString& device)
{
  debugOut(QString("Set CD/VCD/DVD device to %1\n").arg(device));
  devicePath = device;

  xine_cfg_entry_t config;

  xine_config_lookup_entry (xineEngine, "input.cdda_device", &config);
  cachedCDPath = config.str_value;
  config.str_value = (char*)device.latin1();
  xine_config_update_entry (xineEngine, &config);

  xine_config_lookup_entry (xineEngine, "input.vcd_device", &config);
  cachedVCDPath = config.str_value;
  config.str_value = (char*)device.latin1();
  xine_config_update_entry (xineEngine, &config);

  xine_config_lookup_entry (xineEngine, "input.dvd_device", &config);
  cachedDVDPath = config.str_value;
  config.str_value = (char*)device.latin1();
  xine_config_update_entry (xineEngine, &config);
}


void QXineWidget::SetStreamSaveDir(const QString& dir)
{
  xine_cfg_entry_t config;

  if (!xine_config_lookup_entry (xineEngine, "misc.save_dir", &config)) return; /* older xine-lib */

  debugOut(QString("Set misc.save_dir to: %1").arg(dir));
  config.str_value = (char*)dir.latin1();
  xine_config_update_entry (xineEngine, &config);
}  


const QString QXineWidget::GetStreamSaveDir()
{
  xine_cfg_entry_t config;

  if (!xine_config_lookup_entry (xineEngine, "misc.save_dir", &config)) return QString::null; /* older xine-lib */

  return QString( config.str_value );
}


void QXineWidget::SetBroadcasterPort(const uint port)
{
  debugOut(QString("Set broadcaster port to ").arg(port));
  xine_set_param(xineStream, XINE_PARAM_BROADCASTER_PORT, port);
}


void QXineWidget::slotSpeedPause()
{
	if (!xineStream)
		return;
  xine_set_param(xineStream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
  posTimer.stop();
  signalNewInfo("Pause.");
}


void QXineWidget::slotSpeedNormal()
{
	if (!xineStream)
		return;
  xine_set_param(xineStream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
  posTimer.start(500);
  signalNewInfo(trackInfoRow);
}

void QXineWidget::slotSpeed(int iSpeed)
{
	if (!xineStream)
		return;
	// Possible speeding values :
	// XINE_SPEED_NORMAL / XINE_SPEED_PAUSE / XINE_SPEED_SLOW_4 / XINE_SPEED_SLOW_2
	// XINE_SPEED_NORMAL / XINE_SPEED_FAST_2 / XINE_SPEED_FAST_4
  xine_set_param(xineStream, XINE_PARAM_SPEED, iSpeed);
  signalNewInfo(trackInfoRow);
}

void QXineWidget::slotPlayBackwards()
{
/*
 * set xine to a trick mode for fast forward, backwards playback,
 * low latency seeking. Please note that this works only with some
 * input plugins. mode constants see below.
 *
 * returns 1 if OK, 0 on error (use xine_get_error for details)

int  xine_trick_mode (xine_stream_t *stream, int mode, int value);

trick modes 
#define XINE_TRICK_MODE_OFF                0
#define XINE_TRICK_MODE_SEEK_TO_POSITION   1
#define XINE_TRICK_MODE_SEEK_TO_TIME       2
#define XINE_TRICK_MODE_FAST_FORWARD       3
#define XINE_TRICK_MODE_FAST_REWIND        4
*/
	xine_trick_mode(xineStream, XINE_TRICK_MODE_FAST_REWIND, XINE_SPEED_NORMAL);
}

QString QXineWidget::GetSupportedExtensions() const
{
	if (!xineEngine)
		return QString(); 
	return xine_get_file_extensions( xineEngine );
}


void QXineWidget::slotSetAudioChannel(int ch)
{
  xine_set_param(xineStream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, ch-1);
}  


void QXineWidget::slotSetSubtitleChannel(int ch)
{
  xine_set_param(xineStream, XINE_PARAM_SPU_CHANNEL, ch-1);
}


void QXineWidget::slotSetFileSubtitles(QString newMRL)
{
  int pos;
  int time;
  int length;
  
  QString oldMRL = currentMRL;
  
  if ( !xine_get_pos_length(xineStream, &pos, &time, &length) )
  {
    debugOut("No valid stream position information\n");
    return;
  }    
  
  if (xine_get_status(xineStream) == XINE_STATUS_PLAY)
    xine_stop(xineStream);
    
  posTimer.stop();
  
  if(!PlayMRL(newMRL, currentTitle, true))
     PlayMRL(oldMRL, currentTitle, true);
     
  slotChangePosition(pos);
}


void QXineWidget::slotTogglePlayMode()
{
  switch (playMode)
 {
   case NORMAL_PLAY:
   {
     playMode = REPEAT_PLAY;
     break;
   }
   case REPEAT_PLAY:
   {
     playMode = PERCENT_PLAY;
     break;
   }
   case PERCENT_PLAY:
   {
     playMode = NORMAL_PLAY;
     break;
   }
 }
}


void QXineWidget::slotGetPosition()
{
  if (!xineRunning) return;

  int pos;
  int time;
  int length;

  double perc;

  QString t;
  
  if ( !xine_get_pos_length(xineStream, &pos, &time, &length) )
  {
    debugOut("No valid stream position information\n");
    return;
  }  
  
  switch (playMode)
  { 
    case PERCENT_PLAY:
    {
      perc = pos/65535.0;
      time = (int) (perc);
      if ( (time < 0) || (time > 100) )
      {
        emit signalNewPosition( pos , "  --%  " );
        return;
      }  
      t = t.setNum(time);
      t = QString(" %1%2 ").arg(t).arg("%");
      emit signalNewPosition( pos, t );
      break;
    }  
    case REPEAT_PLAY:
    {
      time = length - time;      
    }
    case NORMAL_PLAY:
    {
      if (time < 0)
      {
        emit signalNewPosition( pos , "--:--:--.---" );
        return;
      } 
      emit signalNewPosition( pos, msToTimeString(time) );
      emit signalNewPosition( (long)time );
      break;
    }
  }
}


void QXineWidget::slotChangePosition(int pos)
{
  if (!xineRunning) return;

  int pause = !xine_get_param(xineStream, XINE_PARAM_SPEED);
  if ( (xine_get_status(xineStream) == XINE_STATUS_PLAY) && (xine_get_stream_info(xineStream, XINE_STREAM_INFO_SEEKABLE)) )
  {
    posTimer.stop();
    xine_play(xineStream, pos, 0);
    posTimer.start(500,false);
  }
  
  if (pause)
    slotSpeedPause();
}

void QXineWidget::slotChangePositionByTime(int pos)
{
  if (!xineRunning) return;

  int pause = !xine_get_param(xineStream, XINE_PARAM_SPEED);
  if ( (xine_get_status(xineStream) == XINE_STATUS_PLAY) && (xine_get_stream_info(xineStream, XINE_STREAM_INFO_SEEKABLE)) )
  {
    posTimer.stop();
    xine_play(xineStream,0, pos);// in MSec
    posTimer.start(500,false);
  }

  if (pause)
    slotSpeedPause();
}

void QXineWidget::slotEject()
{
  xine_eject(xineStream);
}

void QXineWidget::slotEnableAutoresize(bool enable)
{
  autoresizeEnabled = enable;
  if (!autoresizeEnabled)
  {
    videoFrameHeight = 0;
    videoFrameWidth = 0;
  }  
}


void QXineWidget::slotToggleDeinterlace()
{
  if (xine_get_param(xineStream, XINE_PARAM_VO_DEINTERLACE))
    xine_set_param(xineStream, XINE_PARAM_VO_DEINTERLACE, false);
   else
    xine_set_param(xineStream, XINE_PARAM_VO_DEINTERLACE, true);
}


void QXineWidget::slotAspectRatioAuto()
{
  xine_set_param(xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_AUTO);
}  


void QXineWidget::slotAspectRatio4_3()
{
  xine_set_param(xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_4_3);
}


void QXineWidget::slotAspectRatio16_9()
{
  xine_set_param(xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_ANAMORPHIC);
}


void QXineWidget::slotAspectRatioSquare()
{
  xine_set_param(xineStream, XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_SQUARE);
}


void QXineWidget::slotZoomOut()
{
  if ((currentZoom - 5) >= 100)
  {
    currentZoom -= 5;
    xine_set_param(xineStream, XINE_PARAM_VO_ZOOM_X, currentZoom);
    xine_set_param(xineStream, XINE_PARAM_VO_ZOOM_Y, currentZoom);
  }
}


void QXineWidget::slotZoomIn()
{
  if ((currentZoom + 5) <= XINE_VO_ZOOM_MAX)
  {
    currentZoom += 5;
    xine_set_param(xineStream, XINE_PARAM_VO_ZOOM_X, currentZoom);
    xine_set_param(xineStream, XINE_PARAM_VO_ZOOM_Y, currentZoom);
  }
}


void QXineWidget::slotZoomOff()
{
  xine_set_param(xineStream, XINE_PARAM_VO_ZOOM_X, 100);
  xine_set_param(xineStream, XINE_PARAM_VO_ZOOM_Y, 100);
  currentZoom = 100;
}


QString QXineWidget::GetStreamInfo()
{
  QString streamInfo;

  streamInfo = streamInfo + "(" + xine_get_meta_info(xineStream, XINE_META_INFO_INPUT_PLUGIN) + ") ";

  if (xine_get_stream_info(xineStream, XINE_STREAM_INFO_HAS_VIDEO))
  {
     streamInfo.append(xine_get_meta_info(xineStream, XINE_META_INFO_VIDEOCODEC));
     streamInfo.append(" (");
     streamInfo.append( QString::number(xine_get_stream_info(xineStream, XINE_STREAM_INFO_VIDEO_WIDTH)) );
     streamInfo.append("x");
     streamInfo.append( QString::number(xine_get_stream_info(xineStream, XINE_STREAM_INFO_VIDEO_HEIGHT)) );
     streamInfo.append(") ");
  }

  if (xine_get_stream_info(xineStream, XINE_STREAM_INFO_HAS_AUDIO))
  {
     streamInfo.append(xine_get_meta_info(xineStream, XINE_META_INFO_AUDIOCODEC));
     streamInfo.append(" (");
     streamInfo.append( QString::number(xine_get_stream_info(xineStream, XINE_STREAM_INFO_AUDIO_BITRATE)/1000) );
     streamInfo.append("kb)");
  }

  return streamInfo;
}


QString QXineWidget::GetLengthInfo()
{
  int pos, time, length;

  bool ok = xine_get_pos_length(xineStream, &pos, &time, &length);

  if ( (ok) && (length > 0) )
  {
    return msToTimeString(length);
  }

  return QString::null;
}  


void QXineWidget::slotEmitLengthInfo()
{
  QString length = GetLengthInfo();
  if (!length.isNull())
  {
    lengthInfoTimer.stop();
    emit signalLengthInfo( length );
  }  
}


void QXineWidget::slotSetScreensaverTimeout( int ssTimeout )
{
  screensaverTimeout = ssTimeout;

#ifdef HAVE_XTEST
  if (screensaverTimeout > 0)
    screensaverTimer.start(screensaverTimeout* 60000); //min
   else
    screensaverTimer.stop();
#endif    
}


int QXineWidget::GetScreensaverTimeout() const
{
  return screensaverTimeout;
}


void QXineWidget::GlobalPosChanged()
{
  QPoint g = mapToGlobal(QPoint(0,0));
  globX = g.x();
  globY = g.y();
//  debugOut(QString("QXineWidget: x: %1 y: %2").arg(globX).arg(globY));
}    


void QXineWidget::slotFakeKeyEvent()
{
#ifdef HAVE_XTEST

// debugOut("Fake key event...\n");

 if (haveXTest)
 {
   XTestFakeKeyEvent(x11Display(), xTestKeycode, true, CurrentTime);
   XTestFakeKeyEvent(x11Display(), xTestKeycode, false, CurrentTime);
   XSync(x11Display(), false);
 }  

#endif 

}


const xine_t* const QXineWidget::GetXineEngine()const
{
  return xineEngine;
}

  
/************ video settings ****************/

void QXineWidget::GetVideoSettings(int& hue, int& sat, int& contrast, int& bright,
                                   int& audioAmp, int& avOffset, int& spuOffset) const
{
  hue = xine_get_param(xineStream, XINE_PARAM_VO_HUE);
  sat = xine_get_param(xineStream, XINE_PARAM_VO_SATURATION);
  contrast = xine_get_param(xineStream, XINE_PARAM_VO_CONTRAST);
  bright = xine_get_param(xineStream, XINE_PARAM_VO_BRIGHTNESS);

  audioAmp = xine_get_param(xineStream, XINE_PARAM_AUDIO_AMP_LEVEL);
  avOffset = xine_get_param(xineStream, XINE_PARAM_AV_OFFSET);
  spuOffset = xine_get_param(xineStream, XINE_PARAM_SPU_OFFSET);
}


void QXineWidget::slotSetHue(int hue)
{
  xine_set_param(xineStream, XINE_PARAM_VO_HUE, hue);
}
  

void QXineWidget::slotSetSaturation(int sat)
{
  xine_set_param(xineStream, XINE_PARAM_VO_SATURATION, sat);
}
  

void QXineWidget::slotSetContrast(int contrast)
{
  xine_set_param(xineStream, XINE_PARAM_VO_CONTRAST, contrast);
}
  

void QXineWidget::slotSetBrightness(int bright)
{
  xine_set_param(xineStream, XINE_PARAM_VO_BRIGHTNESS, bright);
}
  

void QXineWidget::slotSetAudioAmp(int amp)
{
  xine_set_param(xineStream, XINE_PARAM_AUDIO_AMP_LEVEL, amp);
}
  

void QXineWidget::slotSetAVOffset(int av)
{
  xine_set_param(xineStream, XINE_PARAM_AV_OFFSET, av);
}
  

void QXineWidget::slotSetSpuOffset(int spu)
{
  xine_set_param(xineStream, XINE_PARAM_SPU_OFFSET, spu);
}



/**************** equalizer *****************/

void QXineWidget::slotSetEq30(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_30HZ, -val);
}


void QXineWidget::slotSetEq60(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_60HZ, -val);
}


void QXineWidget::slotSetEq125(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_125HZ, -val);
}


void QXineWidget::slotSetEq250(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_250HZ, -val);
}


void QXineWidget::slotSetEq500(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_500HZ, -val);
}


void QXineWidget::slotSetEq1k(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_1000HZ, -val);
}


void QXineWidget::slotSetEq2k(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_2000HZ, -val);
}


void QXineWidget::slotSetEq4k(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_4000HZ, -val);
}


void QXineWidget::slotSetEq8k(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_8000HZ, -val);
}


void QXineWidget::slotSetEq16k(int val)
{
  xine_set_param(xineStream, XINE_PARAM_EQ_16000HZ, -val);
}

/*************** dvd menus ******************/

void QXineWidget::slotMenu1()
{
  xine_event_t xev;
  xev.type = XINE_EVENT_INPUT_MENU1;
  xev.data = NULL;
  xev.data_length = 0;
  
  xine_event_send(xineStream, &xev);
}


void QXineWidget::slotMenu2()
{
  xine_event_t xev;
  xev.type = XINE_EVENT_INPUT_MENU2;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
  
}


void QXineWidget::slotMenu3()
{
  xine_event_t xev;
  xev.type = XINE_EVENT_INPUT_MENU3;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
}


void QXineWidget::slotMenu4()
{
  xine_event_t xev;
  xev.type = XINE_EVENT_INPUT_MENU4;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
}


void QXineWidget::slotMenu5()
{
  xine_event_t xev;
  xev.type = XINE_EVENT_INPUT_MENU5;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
}


void QXineWidget::slotMenu6()
{
  xine_event_t xev;
  xev.type = XINE_EVENT_INPUT_MENU6;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
}


void QXineWidget::slotMenu7()
{
  xine_event_t xev;
  xev.type = XINE_EVENT_INPUT_MENU7;
  xev.data = NULL;
  xev.data_length = 0;

  xine_event_send(xineStream, &xev);
}


void QXineWidget::keyPressEvent(QKeyEvent *e)
{  
  if (keyboardGrabber() == this)
  {
    xine_event_t xev;
    xev.data = NULL;
    xev.data_length = 0;

    switch(e->key())
    {
      case Qt::Key_Up:
        xev.type = XINE_EVENT_INPUT_UP;
        break;
      case Qt::Key_Down:
        xev.type = XINE_EVENT_INPUT_DOWN;
        break;
      case Qt::Key_Left:
        xev.type = XINE_EVENT_INPUT_LEFT;
        break;
      case Qt::Key_Right:
        xev.type = XINE_EVENT_INPUT_RIGHT;
        break;
      case Qt::Key_Return:
        xev.type = XINE_EVENT_INPUT_SELECT;
        break;
      default:
        break;
    }
    xine_event_send(xineStream, &xev);
      
    e->accept();
    return;
  }
  e->ignore();
}



/******** mouse hideing at fullscreen ****/


void QXineWidget::StartMouseHideTimer()
{
  mouseHideTimer.start(5000);
}
  

void QXineWidget::StopMouseHideTimer()
{
  mouseHideTimer.stop();
}
  

void QXineWidget::slotHideMouse()
{
  if (cursor().shape() == Qt::ArrowCursor)
  {
    setCursor(QCursor(Qt::BlankCursor));
  }  
}   


/************************************************************
 *   Take a Screenshot                                      *
 ************************************************************/

void QXineWidget::GetScreenshot(uchar*& rgb32BitData, int& videoWidth, int& videoHeight, double& scaleFactor)
{
  uint8_t   *yuv = NULL, *y = NULL, *u = NULL, *v =NULL;
  
  int        width, height, ratio, format;
  double     desired_ratio, image_ratio;
  if ( ! xineStream )
    return;

  if (!xine_get_current_frame (xineStream, &width, &height, &ratio, &format, NULL))
    return;

  yuv = new uint8_t[((width+8) * (height+1) * 2)];
  if (yuv == NULL)
    {
      errorOut("Not enough memory to make screenshot!");
      return;
    }  

  xine_get_current_frame (xineStream, &width, &height, &ratio, &format, yuv);

  videoWidth = width;
  videoHeight = height;

/*
 * convert to yv12 if necessary
 */

  switch (format) {
  case XINE_IMGFMT_YUY2:
    {
      uint8_t *yuy2 = yuv;

      yuv = new uint8_t[(width * height * 2)];
      if (yuv == NULL)
      {
//        errorOut("Not enough memory to make screenshot!");
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
      warningOut(QString("Screenshot: Format %1 not supported!\n").arg((char*)&format));
      delete [] yuv;
      return;
    }  
  }

  /*
   * convert to rgb
   */

  rgb32BitData = yv12ToRgb (y, u, v, width, height);


  image_ratio = (double) width / (double) height;


  switch (ratio) {
  case XINE_VO_ASPECT_ANAMORPHIC:  /* anamorphic  */
//  case XINE_VO_ASPECT_PAN_SCAN:  /* depreciated */
    desired_ratio = 16.0 /9.0;
    break;
  case XINE_VO_ASPECT_DVB:         /* 2.11:1 */
    desired_ratio = 2.11/1.0;
    break;
  case XINE_VO_ASPECT_SQUARE:      /* square pels */
//  case XINE_VO_ASPECT_DONT_TOUCH:  /* depreciated */
    desired_ratio = image_ratio;
    break; 
  default:
    warningOut(QString("Screenshot: Unknown aspect ratio: %1 - using 4:3").arg(ratio));
  case XINE_VO_ASPECT_4_3:         /* 4:3   */
    desired_ratio = 4.0 / 3.0;
    break;
  }

  scaleFactor = desired_ratio / image_ratio;

  delete [] yuv;
}

QString QXineWidget::msToTimeString( int iMSec )
{
  int hours;
  int min;
  int sec;
  int msec;
  int my_msec=iMSec;
  QString tmp;
  QString t;

    msec = iMSec;
    msec = msec/1000;  //sec
    hours = msec/3600;
    my_msec -= hours*3600*1000;
    t = t.setNum(hours);
    t = t.rightJustify (2, '0');
    t.append(":");

    msec = msec - (hours*3600);
    min = msec / 60;
    my_msec -= min*60*1000;
    tmp = tmp.setNum(min);
    tmp = tmp.rightJustify(2, '0');
    t.append(tmp);
    t.append(":");

    sec = msec - (min*60);
    my_msec -= sec*1000;
//    if(my_msec > 500)
//    	sec++;
    tmp = tmp.setNum(sec);
    tmp = tmp.rightJustify(2, '0');
    t.append(tmp);
    t.append(".");
    
    msec = (int)(iMSec/1000.0) * 1000;  //sec
    tmp = tmp.setNum(iMSec - msec);
    tmp = tmp.rightJustify(3, '0');
    t.append (tmp);
    
   return t;
}

int QXineWidget::timeStringToMs( QString timeString )
{
  int sec = 0;
  QStringList tokens = QStringList::split(':',timeString);
  
  sec += tokens[0].toInt()*3600; //hours
  sec += tokens[1].toInt()*60; //minutes
  sec += tokens[2].toInt(); //secs
  
  return sec*1000; //return millisecs
}

/************************************************************
 *   Helpers to convert yuy and yv12 frames to rgb          *
 *   code from gxine modified for 32bit output              *
 *   Copyright (C) 2000-2003 the xine project               *
 ************************************************************/

void QXineWidget::yuy2Toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input,
        int width, int height)
{

  int    i, j, w2;

  w2 = width / 2;

  for (i = 0; i < height; i += 2) {
    for (j = 0; j < w2; j++) {
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

    for (j = 0; j < w2; j++) {
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

uchar *QXineWidget::yv12ToRgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v,
           int width, int height)
{

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

  for (i = 0; i < height; ++i) {
    /*
     * calculate u & v rows
     */
    sub_i_uv = ((i * uv_height) / height);

    for (j = 0; j < width; ++j) {
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






