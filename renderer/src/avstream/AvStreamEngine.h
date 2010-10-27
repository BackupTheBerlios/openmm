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

#ifndef AvStreamEngine_INCLUDED
#define AvStreamEngine_INCLUDED

#include <Omm/UpnpAvRenderer.h>
#include <Omm/AvStream.h>


class AvStreamEngine : public Omm::Av::Engine
{
public:
    AvStreamEngine();
    ~AvStreamEngine();
    
    // virtual void setVideoDriver(string);
    // virtual void setAudioDriver(string);
    // virtual void setFullscreen(bool on);
    // virtual vector<string> getAudioAdapters();
    // virtual void setAudioAdapter(string);
    
    virtual void createPlayer();
    void destructPlayer();
    
    virtual void setFullscreen(bool on = true);
    std::string getEngineId() { return _engineId; }
    
    virtual bool preferStdStream();
    virtual void setUri(std::string mrl);
    virtual void setUri(std::istream& istr);
    virtual void load();
    virtual void pause();
    virtual void stop();
    virtual void seek(int seconds);
    virtual void next();
    virtual void previous();
    
    virtual void setSpeed(int nom, int denom);
    virtual void getPosition(float &seconds);
    virtual void getLength(float &seconds);
    
    /*
      Rendering Control
    */
    virtual void setVolume(int channel, float vol);
    virtual void getVolume(int channel, float &vol);
    
    virtual void endOfStream() {}
    
private:
    bool isSeekable();
    void savePosition();
    
    void endOfStream(Omm::AvStream::Sink::EndOfStream* eof);
    
    Poco::FastMutex             _actionLock;
    std::string                 _engineId;
    bool                        _isPlaying;
    
    Omm::AvStream::Tagger*      _pTagger;
    Omm::AvStream::Clock*       _pClock;
    Omm::AvStream::Demuxer*     _pDemuxer;
    Omm::AvStream::AudioSink*   _pAudioSink;
    Omm::AvStream::VideoSink*   _pVideoSink;
    
    std::ifstream               _file;
    Poco::Net::HTTPClientSession* _pSession;
    bool                        _isFile;
    std::string                 _uri;
};

#endif