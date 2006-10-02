/***************************************************************************
                           qxinewidget.h
                             -------------------
    begin                : Fre Apr 18 2003
    revision             : $Revision: 1.79 $
    last modified        : $Date: 2005/12/27 13:30:20 $ by $Author: qdvdauthor $
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

#ifndef QXINEWIDGET_H
#define QXINEWIDGET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qtimer.h>
#include <qevent.h>
#include <qthread.h>
#include <pthread.h>

#include <xine.h>

#include <X11/Xlib.h>

#include "jamstreamplayer.h"

class QXineWidget : public JAMStreamPlayer, protected QThread
{

 Q_OBJECT
   
public: 
  QXineWidget(QWidget *parent=0, void *p=NULL, const char *name=0, const QString& prefAudio = QString::null, const QString& prefVideo = QString::null, bool verbose = false, bool startManual = false);
  QXineWidget(QWidget *parent=0, const char *name=0, const QString& prefAudio = QString::null, const QString& prefVideo = QString::null, bool verbose = false, bool startManual = false);
 virtual ~QXineWidget();

  QString i18n(const char *text);
  static void debugOut (QString);
  static void errorOut (QString);
  static void warningOut (QString);
  static void postEvent(QObject *, QEvent *);
  void messageBoxError(QWidget *, QString);

  QString msToTimeString( int msec ); // msec to a h:mm:ss string
  int timeStringToMs( QString time ); // h:mm:ss string to msec
  void yuy2Toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height);
  uchar * yv12ToRgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height);
  
  bool taskInitXine(); /* will be started automatically */
  bool InitXine(); /* will be started automatically */
  bool m_bIsXineInitialized;
  bool m_bInitialising;	// that'll make sure we initialise it only once

  void GetAutoplayPlugins(QStringList&) const;
  bool GetAutoplayPluginMrl(const QString&, QStringList&);
  void GetVisualPlugins(QStringList&) const;

  void play(QString mrl);
  bool PlayMRL(const QString& mrl, const QString& title, bool returnInfo);
  virtual void setLogoFile();
  bool xineRunning;  /* is xine engine running */

  void PlayNextChapter() const;
  void PlayPreviousChapter() const;

  QString GetSupportedExtensions() const;  /* get mime types xine can play */
  
  int GetScreensaverTimeout() const; /* returns the timeout value in ms */
  
  void GetVideoSettings(int& hue, int& sat, int& contrast, int& bright, int& audioAmp,
                        int& avOffset, int& spuOffset) const;

  
 /* take a screenshot, will be stored in rgb32BitData */                       
  void GetScreenshot(uchar*& rgb32BitData, int& videoWidth, int& videoHeight, double& scaleFactor);

 /* get the xine engine, needed by the xine configuration dialog */
  const xine_t* const GetXineEngine() const;

  void StartMouseHideTimer();    /* for fullscreen */
  void StopMouseHideTimer();

  void SetDevice(const QString& device);
  void SetStreamSaveDir(const QString& dir);
  const QString GetStreamSaveDir();
  void SetBroadcasterPort(const uint port);
  void SetVisualPlugin(const QString&);
  
  QString GetLengthInfo();
  QString GetStreamInfo();

  xine_stream_t *GetStream () {return xineStream;}

  void SaveXineConfig();

  const QString& GetXineMessage() { return xineMessage; }

  void GlobalPosChanged();  /* call this if global pos of parent was changed */
  
signals:
  void signalQuit();   /** ask main widget to quit kaffeine, if InitXine() not successfull **/
  void signalNewInfo(const QString&);
  void signalNewTitle(const QString&);  
  void signalHasChapters(bool);
  void signalPlaybackFinished();
  void signalNewMrlReference(const QString&);
  void signalNewChannels(const QStringList&, const QStringList&, int, int);
  void signalNewPosition(int, const QString&);
  void signalNewPosition(long);
  void signalLengthInfo(const QString&);   /* length info now available */
  
  void signalStreamInfo(const QString&);

  void signalShowOSD(const QString&);  /* show the osd title */

  void signalShowContextMenu(const QPoint&);  /* right-click on video window */
  void signalShowFullscreenPanel(const QPoint&);
  void signalMetaInfo( const QString& );
  void signalNewXineMessage();
  void signalToggleFullscreen();
  
public slots:
  virtual void polish();
  void slotSetMetaString( const QString& metaStr ) { metaString = metaStr; }
  
  void slotSetVolume(int);
  virtual void slotStopPlayback();
  void slotSpeedPause();
  void slotSpeedNormal();
  	void slotSpeed(int);
  	void slotPlayBackwards();
  void slotSetAudioChannel(int);
  void slotSetSubtitleChannel(int);
  void slotSetFileSubtitles(QString);
  void slotChangePosition(int);
  void slotChangePositionByTime(int);
  void slotTogglePlayMode();
  void slotEject();
  void slotEnableAutoresize(bool enable);
  void slotToggleDeinterlace();
  void slotAspectRatioAuto();
  void slotAspectRatio4_3();
  void slotAspectRatio16_9();
  void slotAspectRatioSquare();
  void slotZoomIn();
  void slotZoomOut();
  void slotZoomOff();
  void slotSetScreensaverTimeout( int ssTimeout );

  void slotSetHue(int hue);
  void slotSetSaturation(int sat);
  void slotSetContrast(int contrast);
  void slotSetBrightness(int bright);
  void slotSetAudioAmp(int amp);
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

  void slotMenu1();
  void slotMenu2();
  void slotMenu3();
  void slotMenu4();
  void slotMenu5();
  void slotMenu6();
  void slotMenu7();


  
private slots:
  void slotGetPosition();
  void slotEmitLengthInfo();
  void slotHideMouse();
  void slotFakeKeyEvent();

protected:
 virtual void mouseMoveEvent(QMouseEvent*);
 virtual void mousePressEvent(QMouseEvent*);
 virtual void keyPressEvent(QKeyEvent *);
 virtual void timerEvent( QTimerEvent* );


  static void VideoDriverChangedCallback(void* p, xine_cfg_entry_t* entry);
  static void AudioDriverChangedCallback(void* p, xine_cfg_entry_t* entry);

private:
  virtual void run();  /* implement pure virtual QThread method */
  void initMe (const QString&, const QString&, bool, bool);

 /********* callbacks and threads ************/
  static void DestSizeCallback(void*, int, int, double, int *, int *, double *);

  static void FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y);

  static void XineEventListener(void* p, const xine_event_t*);

  void SendXineError( bool returnInfo );

protected:
 xine_t* xineEngine;
 xine_audio_port_t* audioDriver;
 xine_video_port_t* videoDriver;
 xine_stream_t* xineStream;
 QTimer posTimer;

 
private:
 //*****************
 bool m_bOwnXineEngine;
 
 bool startXineManual;
 
 enum PLAY_MODE { NORMAL_PLAY, REPEAT_PLAY, PERCENT_PLAY};
 PLAY_MODE playMode;

 QString preferedAudio;
 QString preferedVideo;
 QString devicePath;
 QString cachedCDPath,cachedVCDPath, cachedDVDPath;
 bool xineVerbose;

 QString xineMessage;

 pthread_t eventThread;

 Window xineWindow;
 Display* xineDisplay;
 int xineScreen;

 int videoFrameWidth;
 int videoFrameHeight;
 int globX;
 int globY;

 x11_visual_t visual;
 xine_event_queue_t* eventQueue;

 xine_post_t* xinePost;
 xine_post_out_t* postAudioSource;
 xine_post_in_t* postInput;

 double displayRatio;

 QSize newParentSize;

 int screensaverTimeout;
 int osdTimeout;
 int currentZoom;

 QTimer lengthInfoTimer;
 QTimer screensaverTimer;
 QTimer mouseHideTimer;

 QString configFile;
 QString currentMRL;
 QString currentTitle;
 QString currentProgress;
 QString newMrlReference;
 QString extraInfo;
 QString trackInfoRow;
// QString logoPath;
 QString metaString;

 QStringList audioCh;
 QStringList subCh;
 int currentAudio, currentSub;
 int savedPos;
 
 QString audioDriverName;
 QString videoDriverName;
 QString visualPluginName;

 int haveXTest, xTestKeycode;

 bool autoresizeEnabled;
 bool DVDMenuEntered;
 bool DVDButtonEntered;
   
};

#endif // QXINEWIDGET_H
