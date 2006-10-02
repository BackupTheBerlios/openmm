/***************************************************************************
                           kxinewidget.h -
		A KDE/QT API for the xine-lib. Define USE_QT_ONLY if you use QT only.
                             -------------------
    begin                : Fre Apr 18 2003
    revision             : $Revision: 1.21 $
    last modified        : $Date: 2005/07/11 17:52:33 $ by $Author: juergenk $
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

#ifndef KXINEWIDGET_H
#define KXINEWIDGET_H

/* define this if you want to use QT only - disables post plugin support (except visualization)! */
#define USE_QT_ONLY 1

/* Usage:
 *
 * KXineWidget m_xine = new KXineWidget(parent);
 * m_xine->appendToQueue("the_best_song_of_the_world.mp3");
 *
 * You should at least connect to signalXineFatal(const QString&)!
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* forward declaration */
class QWidget;
class QString;
class QStringList;
class QMouseEvent;
class QTimerEvent;
class QTime;

#include <qtimer.h>
#include <qthread.h>
#include <qptrlist.h>
#include <qdatetime.h>
#include <X11/Xlib.h>
#include <xine.h>

#include "jamstreamplayer.h"

#ifndef USE_QT_ONLY
#include "postfilter.h"
#else
 typedef int PostFilter; /* dummy type */
#endif

#define SUPPORTED_PROTOCOLS "file,http,mms,mmst,rtsp,rtp,tcp,pnm,cdda,vcd,vcdo,dvd,dvb,pvr,v4l,net,vdr,smb"

#define DEFAULT_TVTIME_CONFIG "tvtime:method=LinearBlend,enabled=1,pulldown=none,framerate_mode=half_top,judder_correction=0,use_progressive_frame_flag=1,chroma_filter=0,cheap_mode=1"

#define DEFAULT_OSD_FONT_SIZE 18
#define DEFAULT_OSD_DURATION  5000
#define OSD_MESSAGE_LOW_PRIORITY    1
#define OSD_MESSAGE_NORMAL_PRIORITY 2

class KXineWidget : public JAMStreamPlayer, private QThread
{

 Q_OBJECT

public:
  KXineWidget(QWidget *parent=0, const char *name=0,
  		const QString& pathToConfigFile = QString::null, const QString& pathToLogoFile = QString::null,
		const QString& audioDriver = QString::null, const QString& videoDriver = QString::null,
		bool startManual = false, bool verbose = false);
 ~KXineWidget();

  enum Speed { Normal = 0, Pause, Fast1, Fast2, Slow1, Slow2, Undefined };

  bool initXine(); /* call this only if you set startManual = true in the constructor */
  bool isXineReady() const { return m_xineReady; }
  bool isPlaying() const;
  Speed getSpeed() const { return m_currentSpeed; }
  QString getXineLog() const;

  void setQueue(const QStringList& urls) { m_queue = urls; }
  void appendToQueue(const QString& url) { m_queue.append(url); } /* file to play; call slotPlay() to play queue... */
  void clearQueue() { m_queue.clear(); }
  bool isQueueEmpty() const { return m_queue.empty(); }

  void showOSDMessage(const QString& message, uint duration = DEFAULT_OSD_DURATION /* ms */, int priority = OSD_MESSAGE_NORMAL_PRIORITY);

  void play(QString mrl);

  /* stream info */
  const QString& getURL() const { return m_trackURL; }
  const QString& getTitle() const { return m_trackTitle; }
  const QString& getArtist() const { return m_trackArtist; }
  const QString& getAlbum() const { return m_trackAlbum; }
  const QString& getTrackNumber() const { return m_trackNumber; } //may return an empty string with xine-lib 1.0
  const QString& getGenre() const { return m_trackGenre; }
  const QString& getYear() const { return m_trackYear; }
  const QString& getComment() const { return m_trackComment; }
  const QTime& getLength() const { return m_trackLength; }
  const QString& getVideoCodec() const { return m_trackVideoCodec; }
  QSize getVideoSize() const { return QSize(m_videoFrameWidth, m_videoFrameHeight); }
  uint getVideoBitrate() const { return m_trackVideoBitrate; }
  const QString& getAudioCodec() const { return m_trackAudioCodec; }
  uint getAudioBitrate() const { return m_trackAudioBitrate; }
  const QString& getSubtitleURL() const { return m_trackSubtitleURL; }
  const QString& getSaveURL() const { return m_trackSaveURL; }
  bool hasVideo() const { return m_trackHasVideo; }
  bool hasAudio() const { return m_trackHasAudio; }
  bool hasChapters() const { return m_trackHasChapters; }
  bool hasSubtitleURL() const { return !m_trackSubtitleURL.isNull(); }
  bool hasSaveURL() const { return !m_trackSaveURL.isNull(); }
  bool isSeekable() const { return m_trackIsSeekable; }

  uint getVolume() const; /* percent */
  bool SoftwareMixing() const;
  QTime getPlaytime() const;
  uint getPosition() const; /* 0..65535 */

  void savePosition(const int pos) { (pos > 0) ? m_savedPos = pos : m_savedPos = 0; }

  /* plugin handling */
  QStringList getAudioDrivers() const { return m_audioDriverList; }
  QStringList getVideoDrivers() const { return m_videoDriverList; }
  void getAutoplayPlugins(QStringList& pluginNames) const;
  bool getAutoplayPluginURLS(const QString& name, QStringList& urls);
  QStringList getVisualPlugins() const;

#ifndef USE_QT_ONLY
  void createDeinterlacePlugin(const QString& configString, QWidget* parent);
  const QString getDeinterlaceConfig() const;
  QStringList getVideoFilterNames() const;
  QStringList getAudioFilterNames() const;
  QStringList getAudioFilterConfig();
  QStringList getVideoFilterConfig();
#endif

  QString getSupportedExtensions() const;  /* get mime types xine can play */

  void playNextChapter() const;
  void playPreviousChapter() const;

  /* config stuff */
  void setStreamSaveDir(const QString& dir);
  const QString getStreamSaveDir();
  void setBroadcasterPort(const uint port);
  void getVideoSettings(int& hue, int& sat, int& contrast, int& bright,
                        int& avOffset, int& spuOffset) const;
 /* disc drives devices*/
  QString audiocdDevice() const;
  QString vcdDevice() const;
  QString dvdDevice() const;

 /* take a screenshot */
  QImage getScreenshot() const;
  void getScreenshot(uchar*& rgb32BitData, int& videoWidth, int& videoHeight, double& scaleFactor) const;

 /* get the xine engine, needed by the xine configuration dialog */
  const xine_t* const getXineEngine() const;
  void saveXineConfig();

  /* for fullscreen */
  void startMouseHideTimer();
  void stopMouseHideTimer();

  void globalPosChanged();  /* call this if global pos of parent was changed */

signals:
  void signalXineFatal(const QString& message);   /** initXine() not successfull! **/
  void signalXineError(const QString& message);
  void signalXineMessage(const QString& message);
  void signalXineStatus(const QString& status);
  void signalXineReady();
  void signalXinePlaying();

  void signalHasChapters(bool);
  void signalPlaybackFinished();
  void signalNewChannels(const QStringList& audio, const QStringList& sub, int currentAudio, int currentSub);
  void signalNewPosition(int, const QTime&);

  void signalTitleChanged();
  void signalVideoSizeChanged();
  void signalLengthChanged();

  void signalRightClick(const QPoint&);  /* right-click on video window */
  void signalLeftClick(const QPoint&);
  void signalMiddleClick();
  void signalDoubleClick();
  void signalSyncVolume();

public slots:
  virtual void polish();

  void slotPlay();
  void slotStop();

  void slotSetVolume(int);
  void slotToggleMute();
  void slotSpeedPause();
  void slotSpeedNormal();
  void slotSpeedFaster();
  void slotSpeedSlower();
  void slotSetVisualPlugin(const QString& name);
  void slotSetAudioChannel(int);
  void slotSetSubtitleChannel(int);
  void slotSetFileSubtitles(QString);
  void slotStartSeeking();
  void slotSeekToPosition(int pos);
  void slotSeekToTime(const QTime&);
  void slotStopSeeking();
  void slotEnableVideoFilters(bool enable);
  void slotEnableAudioFilters(bool enable);
  void slotEject();
  void slotEnableAutoresize(bool enable);
  void slotAspectRatioAuto();
  void slotAspectRatio4_3();
  void slotAspectRatioAnamorphic(); /* 16:9 */
  void slotAspectRatioSquare(); /* 1:1 */
  void slotAspectRatioDVB(); /* 2.11:1 */
  void slotZoomIn();
  void slotZoomOut();
  void slotZoomOff();
  void slotToggleDeinterlace();
  void slotSetDeinterlaceConfig(const QString&);
  void slotGetInfoDelayed();

  /* disc drive devices */
  void slotSetAudiocdDevice(const QString&);
  void slotSetVcdDevice(const QString&);
  void slotSetDvdDevice(const QString&);

  void slotSetHue(int hue);
  void slotSetSaturation(int sat);
  void slotSetContrast(int contrast);
  void slotSetBrightness(int bright);
  void slotSetAVOffset(int av);
  void slotSetSpuOffset(int spu);

  void slotSetEq30(int val);
  void slotSetEq60(int val);
  void slotSetEq125(int val);
  void slotSetEq250(int val);
  void slotSetEq500(int val);
  void slotSetEq1k(int val);
  void slotSetEq2k(int val);
  void slotSetEq4k(int val);
  void slotSetEq8k(int val);
  void slotSetEq16k(int val);
  void slotSetVolumeGain(bool gain);

 /* DVD Menus */
  void slotMenuToggle();
  void slotMenuTitle();
  void slotMenuRoot();
  void slotMenuSubpicture();
  void slotMenuAudio();
  void slotMenuAngle();
  void slotMenuPart();
  void slotDVDMenuLeft();
  void slotDVDMenuRight();
  void slotDVDMenuUp();
  void slotDVDMenuDown();
  void slotDVDMenuSelect();

 /***** postprocess filters ****/
  void slotCreateVideoFilter(const QString& name, QWidget* parent);
  void slotCreateAudioFilter(const QString& name, QWidget* parent);
  void slotDeleteVideoFilter(PostFilter* filter);
  void slotDeleteAudioFilter(PostFilter* filter);
  void slotRemoveAllVideoFilters();
  void slotRemoveAllAudioFilters();

protected slots:
  void slotSendPosition();
  void slotEmitLengthInfo();
  void slotHideMouse();
  void slotOSDHide();
  void slotNoRecentMessage();

protected:
  void mouseMoveEvent(QMouseEvent*);
  void mousePressEvent(QMouseEvent*);
  void timerEvent(QTimerEvent*);
  void mouseDoubleClickEvent(QMouseEvent*);
  void wheelEvent(QWheelEvent*);
  void initOSD();

private:
  virtual void run();  /* implement pure virtual QThread method */

 /********* callbacks and threads ************/
  static void destSizeCallback(void* p, int video_width, int video_height, double video_aspect,
                       int* dest_width, int* dest_height, double* dest_aspect);

  static void frameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y);

  static void videoDriverChangedCallback(void* p, xine_cfg_entry_t* entry);
  static void audioDriverChangedCallback(void* p, xine_cfg_entry_t* entry);
  static void audioMixerMethodChangedCallback(void* p, xine_cfg_entry_t* entry);
  static void showOSDMessagesChangedCallback(void* p, xine_cfg_entry_t* entry);
  static void sizeForOSDMessagesChangedCallback(void* p, xine_cfg_entry_t* entry);
  static void fontForOSDMessagesChangedCallback(void* p, xine_cfg_entry_t* entry);

  static void xineEventListener(void* p, const xine_event_t*);

protected:
  void sendXineError();
  QTime getLengthInfo();
#ifndef USE_QT_ONLY
  void wireVideoFilters();
  void unwireVideoFilters();
  void wireAudioFilters();
  void unwireAudioFilters();
#endif

/* HELPER FUNCTIONS */
public:
#ifdef USE_QT_ONLY
  static QString i18n(const char *text);
#endif
  static void debugOut(QString);
  static void errorOut(QString);
  static void warningOut(QString);
  static QTime msToTime(int msec);
  static void   yuy2Toyv12(uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height);
  static uchar *yv12ToRgb(uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height);

protected:
  QStringList m_queue;

  bool m_startXineManual;
  bool m_xineReady;
  QString m_logoFile;
  QString m_preferedAudio;
  QString m_preferedVideo;
  bool m_xineVerbose;
  QString m_configFilePath;
  QStringList m_audioDriverList;
  QStringList m_videoDriverList;

 /*x11*/
  Window m_xineWindow;
  Display* m_xineDisplay;
  int m_xineScreen;

 /*xine*/
  x11_visual_t m_x11Visual;
  xine_t* m_xineEngine;
  xine_audio_port_t* m_audioDriver;
  xine_video_port_t* m_videoDriver;
  xine_stream_t* m_xineStream;
  xine_event_queue_t* m_eventQueue;
  xine_osd_t* m_osd;
  bool m_osdUnscaled;
  bool m_osdShow;
  int m_osdSize;
  char* m_osdFont;
  char** m_audioChoices;
  char* m_audioInfo;
  char** m_videoChoices;
  char* m_videoInfo;
  char* m_mixerInfo;
  char* m_osdShowInfo;
  char** m_osdSizeOptions;
  char* m_osdSizeInfo;
  char* m_osdFontInfo;

#ifndef USE_QT_ONLY
 /*postfilter*/
  QPtrList<PostFilter> m_videoFilterList;
  bool m_videoFiltersEnabled;
  QPtrList<PostFilter> m_audioFilterList;
  bool m_audioFiltersEnabled;
  PostFilter* m_deinterlaceFilter;
  bool m_deinterlaceEnabled;
  PostFilter* m_visualPlugin;
#else
  xine_post_t* m_xinePost;
  xine_post_out_t* m_postAudioSource;
  xine_post_in_t* m_postInput;
#endif

  QString m_audioDriverName;
  QString m_videoDriverName;
  QString m_visualPluginName;

  int m_videoFrameWidth;
  int m_videoFrameHeight;
  double m_videoAspect;
  int m_globalX;
  int m_globalY;

  Speed m_currentSpeed;
  QString m_xineMessage;
  QString m_xineError;
  QString m_statusString;
  bool m_softwareMixer;
  bool m_volumeGain;
  double m_displayRatio;
  QString m_cachedCDPath, m_cachedVCDPath, m_cachedDVDPath;
  QSize m_newParentSize;
  int m_currentZoom;
  QStringList m_audioCh;
  QStringList m_subCh;
  int m_currentAudio, m_currentSub;
  int m_savedPos;
  bool m_autoresizeEnabled;
  bool m_DVDButtonEntered;
  QString m_newMRLReference;

  QTimer m_posTimer;
  QTimer m_lengthInfoTimer;
  uint m_lengthInfoTries;
  QTimer m_mouseHideTimer;
  QTimer m_osdTimer;
  QTimer m_recentMessagesTimer;

  QString m_trackURL;
  QString m_trackTitle;
  QString m_trackArtist;
  QString m_trackAlbum;
  QString m_trackNumber;
  QString m_trackGenre;
  QString m_trackYear;
  QString m_trackComment;
  QTime m_trackLength;
  QString m_trackVideoCodec;
  uint m_trackVideoBitrate;
  QString m_trackAudioCodec;
  uint m_trackAudioBitrate;
  QString m_trackSubtitleURL;
  QString m_trackSaveURL;
  bool m_trackHasVideo;
  bool m_trackHasAudio;
  bool m_trackHasChapters;
  bool m_trackIsSeekable;

/*  dvb  */
signals:
  void stopDvb();

public:
  QString TimeShiftFilename;

public slots:
  void setDvb( const QString &pipeName, const QString &chanName, int haveVideo );
  void setDvbCurrentNext( const QStringList &list );
  bool openDvb();
  bool playDvb();
  void slotPlayTimeShift();
  void dvbShowOSD();
  void dvbHideOSD();

private:
  int dvbHaveVideo;
  QTimer dvbOSDHideTimer;
  xine_osd_t *dvbOSD;
  QStringList dvbCurrentNext;
  unsigned int dvbColor[256];
  unsigned char dvbTrans[256];
  unsigned int rgb2yuv( unsigned int R, unsigned int G, unsigned int B );
  void initDvbPalette();
};



#endif
