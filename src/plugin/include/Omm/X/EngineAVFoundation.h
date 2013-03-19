/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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


#ifndef EnginAVFoundation_INCLUDED
#define EnginAVFoundation_INCLUDED

#include <Omm/Upnp.h>
#include <Omm/UpnpAvObject.h>
#include <Omm/UpnpAvRenderer.h>


class AVFoundationEngine : public Omm::Av::Engine
{
public:
    AVFoundationEngine();
    ~AVFoundationEngine();

    virtual void createPlayer();
    virtual void setFullscreen(bool on = true);

    /*
      AVTransport
    */
    virtual void setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo = Omm::Av::ProtocolInfo());
    virtual void play();

    /**
        void setSpeed(int nom, int denom)
        set speed to nom/denom
    */
    virtual void setSpeed(int nom, int denom);
    /**
        void pause()
        toggle pause
    */
    virtual void pause();
    virtual void stop();
    virtual void next();
    virtual void previous();

    virtual void seekByte(Poco::UInt64 byte);
    virtual void seekPercentage(float percentage);
    virtual void seekSecond(Omm::r8 second);

    virtual Poco::UInt64 getPositionByte();
    virtual float getPositionPercentage();
    virtual Omm::r8 getPositionSecond();
    virtual Omm::r8 getLengthSeconds();

    /*
      Rendering Control
    */
    virtual void setVolume(const std::string& channel, float vol);
    virtual float getVolume(const std::string& channel);
    virtual TransportState getTransportState();
    void transportStateChangedNotification();
    void volumeChangedNotification(float volume);

    virtual void setMute(const std::string& channel, bool mute);
    virtual bool getMute(const std::string& channel);

private:
//    void downloadImage();
//    void showImage();

    void*                       _pPlayer;
    std::string                 _imageBuffer;
    std::streamsize             _imageLength;
//    UIImageView*                _lastImageView;
//    UIView*                     _imageBackgroundView;
//    UIView*                     _parentView;

    std::string                 _urlString;
    Omm::Av::ProtocolInfo       _protInfo;
    Omm::Av::Mime               _mime;
    long long                   _startTime;
    float                       _length; // length of media in seconds

    Poco::FastMutex             _lock;
};


#endif
