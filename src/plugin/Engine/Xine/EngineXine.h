/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef ENGINEXINE_H
#define ENGINEXINE_H

#include <Omm/UpnpAvRenderer.h>

#include <xine.h>
#include <xine/xineutils.h>


class XineVideo
{
public:
    virtual int displayWidth() = 0;
    virtual int displayHeight() = 0;
    virtual std::string driverName() = 0;
    virtual int visualType() = 0;
    virtual void* visual() = 0;

    virtual void initVisual(int width, int height) = 0;
    virtual void closeVisual() = 0;

};


class XineAudio
{
};


class XineEngine : public Omm::Av::Engine
{
public:
    XineEngine();
    ~XineEngine();

    virtual void createPlayer();

    std::string getEngineId() { return _engineId; }

    virtual void setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo = Omm::Av::ProtocolInfo());
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void next();
    virtual void previous();

    virtual void setSpeed(int nom, int denom);

    virtual void seekByte(Poco::UInt64 byte);
    virtual void seekPercentage(float percentage);
    virtual void seekSecond(Omm::r8 second);

    virtual Poco::UInt64 getPositionByte();
    virtual float getPositionPercentage();
    virtual Omm::r8 getPositionSecond();

    virtual Omm::r8 getLengthSeconds();

    virtual TransportState getTransportState();

    /*
      Rendering Control
    */
    virtual void setVolume(const std::string& channel, float vol);
    virtual float getVolume(const std::string& channel);

    // TODO: implement setMute() and getMute()
    virtual void setMute(const std::string& channel, bool mute) {}
    virtual bool getMute(const std::string& channel) {}

protected:

    static void FrameOutputCallback(void* p, int video_width, int video_height, double video_aspect,
                          int* dest_x, int* dest_y, int* dest_width, int* dest_height,
                          double* dest_aspect, int* win_x, int* win_y);

    xine_video_port_t   *_videoDriver;
    xine_t              *_xineEngine;
    static int           videoFrameWidth;
    static int           videoFrameHeight;
    static int           _globX, _globY;
    static double        _pixel_aspect;
    double               res_v, res_h;
    int                  fullscreen;

private:
    void init();
    void close();

    bool isSeekable();
    void savePosition();

    void initOSD();

    x11_visual_t         _x11Visual;
    fb_visual_t          _fbVisual;

    xine_audio_port_t*   _audioDriver;
    xine_stream_t*       _xineStream;
    char*                _audioDriverName;
    int                  _audioDeviceNum;

//    XineVideo*		 _pVideo;

    std::string          _mrl;

    xine_event_queue_t*  eventQueue;

    bool                 _pause;
    int                  _posStream;
    int                  _posTime;
    int                  _lengthStream;
    int                  _currentZoom;
    int                  _seekOff;
};

#endif
