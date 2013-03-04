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

#ifndef UpnpAvRenderer_INCLUDED
#define UpnpAvRenderer_INCLUDED

#include <Poco/Timer.h>

#include "Sys/Visual.h"
#include "Upnp.h"
#include "UpnpTypes.h"
#include "UpnpAvObject.h"

namespace Omm {
namespace Av {


class MediaRenderer;
class DevAVTransportRendererImpl;
class DevRenderingControlRendererImpl;
class Visual;
class ProtocolInfo;

class Engine //: public Util::ConfigurablePlugin
{
    friend class MediaRenderer;

public:
    enum TransportState {Stopped, Ended, Playing, Transitioning, PausedPlayback, PausedRecording, Recording, NoMediaPresent};
    static const std::string StreamTypeOther;
    static const std::string StreamTypeAudio;
    static const std::string StreamTypeVideo;
    static const std::string StreamTypePicture;
    static const std::string StreamTypeText;

    Engine();

    std::string getEngineId();
    void setInstancedId(ui4 instanceId);
    virtual void setVisual(Sys::Visual* pVisual);

//    virtual void setOption(const std::string& key, const std::string& value);
    virtual void createPlayer()  = 0;

    // UPnP AV methods
    /*
      AVTransport
    */
    virtual bool preferStdStream() { return false; }

    void setUriEngine(const std::string& uri, const ProtocolInfo& protInfo = ProtocolInfo());
    void setAtomicUriEngine(const std::string& uri, const ProtocolInfo& protInfo = ProtocolInfo());
    virtual void setUri(const std::string& uri, const ProtocolInfo& protInfo = ProtocolInfo()) = 0;
    /// Set uri and protocol info of media item.
    virtual void setUri(std::istream& istr, const ProtocolInfo& protInfo = ProtocolInfo()) {}
    /// Set stream and protocol info of media item.
    virtual void play() = 0;

    virtual void setSpeed(int nom, int denom) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    void setDuration(r8 seconds);
    r8 getDuration();
    void setSize(Poco::UInt64 bytes);
    Poco::UInt64 getSize();

    void seekTrack(ui4 trackNumber);
    void nextTrack();
    void previousTrack();
    void seekTime(r8 second);
    virtual void seekByte(Poco::UInt64 byte) = 0;
    virtual void seekPercentage(float percentage) = 0;
    virtual void seekSecond(r8 second) = 0;

    virtual Poco::UInt64 getPositionByte() = 0;
    virtual float getPositionPercentage() = 0;
    virtual r8 getPositionSecond() = 0;
    virtual r8 getLengthSeconds() = 0;
    virtual std::string getStreamType() { return StreamTypeOther; }
    const std::string transportState();

    /*
      Rendering Control
    */
    virtual void setVolume(const std::string& channel, float vol) = 0;
    virtual float getVolume(const std::string& channel) = 0;

    virtual void setMute(const std::string& channel, bool mute) = 0;
    virtual bool getMute(const std::string& channel) = 0;

protected:
    virtual TransportState getTransportState() = 0;

    void transportStateChanged();
    /// When engine generates a "play", "stop", "pause" or "end of stream event",
    /// transportStateChanged() is triggered and generates the corresponding UPnP event
    /// via the LastChange mechanism.

    void endOfStream(Poco::Timer& timer);
    /// endOfStream is called by the engine implementation on end of track.
    /// this is different to entering transport state stopped, which is triggered by the user.

//    MediaRenderer*                      _pRenderer;
    std::string                         _engineId;
    ui4                                 _instanceId;
    Sys::Visual*                        _pVisual;
    DevAVTransportRendererImpl*         _pAVTransportImpl;
    DevRenderingControlRendererImpl*    _pRenderingControlImpl;
    std::vector<std::string>            _playlist;
    int                                 _trackNumberInPlaylist;
    Poco::Timer*                        _pEndOfStreamTimer;
    Omm::r8                             _duration;  // length of media in seconds
    Poco::UInt64                        _size;      // length of media in bytes
};


class StreamTypeNotification : public Poco::Notification
{
public:
    StreamTypeNotification(ui4 instanceId, const std::string& transportState, const std::string& streamType) : _instanceId(instanceId), _transportState(transportState), _streamType(streamType) {}

    ui4                 _instanceId;
    std::string         _transportState;
    std::string         _streamType;
};


class MediaRenderer : public Device
{
public:
    MediaRenderer();
    ~MediaRenderer();

    void addEngine(Engine* pEngine);
    Engine* getEngine(ui4 instanceId = 0);

private:
    // these pointers to service implementations are only temporarily needed when setting them in the engine.
    DevAVTransportRendererImpl*         _pAVTransportImpl;
    DevRenderingControlRendererImpl*    _pRenderingControlImpl;
};

} // namespace Av
} // namespace Omm

#endif
