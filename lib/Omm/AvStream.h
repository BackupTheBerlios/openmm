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
#ifndef AvStream_INCLUDED
#define AvStream_INCLUDED

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <Poco/Logger.h>
#include <Poco/Runnable.h>
#include <Poco/Semaphore.h>
#include <Poco/Mutex.h>
#include <Poco/Timer.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>
#include <libpostproc/postprocess.h>

//Forward declaration of ffmpeg structs
struct AVInputFormat;
struct AVFormatContext;
struct AVCodecContext;
struct AVRational;
struct SwsContext;
struct ReSampleContext;
struct AVPicture;
struct AVPacket;
struct AVPacketList;
struct AVStream;
}

namespace Omm {
namespace AvStream {

class Meta;
class Stream;
class Overlay;
class Sink;
class PresentationTimer;


class Log
{
public:
    static Log* instance();
    
    Poco::Logger& avstream();
    
private:
    Log();
    
    static Log*     _pInstance;
    Poco::Logger*   _pAvStreamLogger;
};


class AvStream
{
public:
    
    enum StreamType { SyncStreamT, AsyncStreamT };
    
    AvStream();
    ~AvStream();
    
    void set(std::istream& istr);
    void reset();
    
    void init(StreamType streamType = AsyncStreamT);
    void demux();
    
    Stream* audioStream();
    Stream* videoStream();
    
//     void stop();
    
private:
    Meta*                       _pMeta;
    std::vector<Stream*>        _streams;
    Stream*                     _pAudioStream;
    Stream*                     _pVideoStream;
};


class Demuxer : public Poco::Runnable
{
public:
    Demuxer(AvStream* pAvStream);
    
private:
    virtual void run();
    
    AvStream*       _pAvStream;
};


class Frame
{
    friend class Stream;
    friend class SyncStream;
    friend class AsyncStream;
    
public:
    Frame(const Frame& frame);
    Frame(Stream* pStream);
    Frame(Stream* pStream, int dataSize);
    Frame(Stream* pStream, char* data, int dataSize);
    Frame(Stream* pStream, AVPacket* pAvPacket);
    Frame(Stream* pStream, AVFrame* pAvFrame);
    ~Frame();
    
    char* data();
    int size();
    
    char* planeData(int plane);
    int planeSize(int plane);
    
    void printInfo();
    
    Stream* getStream();
    
    Frame* decode();
    Frame* convert(PixelFormat targetFormat);
    
    void writePpm(const std::string& fileName);
    void write(Overlay* overlay);
    
private:
    char*               _data;
    int                 _size;
    AVPacket*           _pAvPacket;
    AVFrame*            _pAvFrame;
    Stream*             _pStream;
//     Poco::FastMutex     _frameLock;
};


class Stream : public Poco::Runnable
{
    friend class AvStream;
    friend class Frame;
    
public:
    Stream();
    
    void open();
    void close();
    
    Frame* allocateVideoFrame(PixelFormat targetFormat);
    
    virtual bool put(Frame* frame) {}
    virtual Frame* get() {}
    
    void attachSink(Sink* pSink);
    
    bool isAudio();
    bool isVideo();
    
    void printInfo();
    std::string name();
    
    int width();
    int height();
    
    // audio parameters
    int sampleWidth();
    unsigned int sampleRate();
    int channels();
    
    // video parameters
    PixelFormat pixelFormat();
    int pictureSize();
    
protected:
    Frame* decodeAudioFrame(Frame* pFrame);
    Frame* decodeVideoFrame(Frame* pFrame);
    
    virtual void run();
    
    std::string             _name;
    AvStream::StreamType    _streamType;
    
    AVStream*               _pAvStream;
    AVCodecContext*         _pAvCodecContext;
    AVCodec*                _pAvCodec;
    
    Sink*                   _pSink;
    
    std::queue<Frame*>      _packetQueue;
};


class SyncStream : public Stream
{
public:
    SyncStream();
    
    virtual bool put(Frame* frame);
    virtual Frame* get();
    
};


class AsyncStream : public Stream
{
public:
    AsyncStream();
    
    virtual bool put(Frame* frame);
    virtual Frame* get();
    
private:
    Poco::Semaphore         _packetQueuePutSemaphore;
    Poco::Semaphore         _packetQueueGetSemaphore;
    Poco::FastMutex         _packetQueueLock;
};


class Meta
{
    friend class Tagger;
    friend class AvStream;
    friend class Visual;
    
public:
    Meta();
    ~Meta();
    
    void print();
    
private:
    AVFormatContext*    _pFormatContext; // contains pointers to ByteIOContext, AVInputFormat, codec ids, array of streams with AVCodecContext
    ByteIOContext*      _pIoContext;
    AVInputFormat*      _pInputFormat;  // contains the io access callbacks
};


class Tagger
{
public:
    Tagger();
    ~Tagger();
    
    // TODO: istr is currently ignored (hardcoded av_open_input_file())
    Meta* tag(std::istream& istr);
    
private:
    AVInputFormat* probeInputFormat(std::istream& istr);
    ByteIOContext* initIo(std::istream& istr);
    
    int                 _tagBufferSize;
    int                 _IoBufferSize;
    unsigned char*      _pIoBuffer;
};


class Overlay
{
public:
    uint8_t* _data[4];
    int      _pitch[4];
};


class Sink {
    friend class Stream;
    friend class SyncStream;
    friend class AsyncStream;
    friend class Clock;
    
public:
    Sink(bool triggerClock = false);
    virtual ~Sink() {}
    
    static Sink* loadPlugin(const std::string& libraryPath, const std::string& className = "SinkPlugin");
    
    virtual void open() = 0;
    virtual void close() = 0;
    
    // write frame into presentation buffer (video overlay or audio dsp buffer)
    virtual void writeFrame(Frame* pFrame) = 0;
    // show the content of presentation buffer
    virtual void presentFrame() = 0;
    
    virtual void pause() {}
    virtual void resume() {}
    
//     virtual int latency() { return 0; }
    virtual int eventLoop() { return 0; }
    
protected:
    Stream*     _pStream;
    
private:
    virtual void writeFrameQueued(Frame *pFrame);
    virtual void presentFrameQueued();
    
    Poco::Semaphore         _presentationSemaphore;
    Poco::FastMutex         _presentationLock;
    
    bool                    _triggerClock;
};


class Clock
{
public:
    static Clock* instance();
    
    void attachSink(Sink* pSink);
    
    // start automatic triggering with a timer, trigger intervall millisec milliseconds.
    void start(long millisec);
    void stop();
    
    // trigger the clock external manually
    void trigger(long millisecPassed);
    
private:
    Clock();
    
    void notify();
    void notifyTimed(Poco::Timer& timer);
    
    static Clock*               _pInstance;
    
    Poco::Timer                 _timer;
    
    Sink*                       _pSink;
    long                        _frameBaseSink;
    long                        _timeLeftSink;
};


class Scanner
{
};


class Transcoder
{
};



} // namespace AvStream
} // namespace Omm

#endif
