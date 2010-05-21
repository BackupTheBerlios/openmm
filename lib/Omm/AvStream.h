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
#include <Poco/Format.h>
#include <Poco/NumberFormatter.h>
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
class Node;
class Frame;
class Stream;
class StreamInfo;
class StreamQueue;
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


template<typename T>
class Queue
{
public:
    Queue(int size, int putTimeout, int getTimeout) :
        _size(size),
        _putTimeout(putTimeout),
        _getTimeout(getTimeout),
        _putSemaphore(size, size),
        _getSemaphore(0, size)
    {
    }
    
    
    bool put(T element)
    {
        Log::instance()->avstream().debug(Poco::format("queue put waiting in thread %s ...",
            Poco::Thread::current()->name()));
        
        if (!_putSemaphore.tryWait(_putTimeout)) {
            Log::instance()->avstream().error(Poco::format("queue put timed out in thread %s.",
                Poco::Thread::current()->name()));
            return false;
        }
        
        _lock.lock();
        _queue.push(element);
        _lock.unlock();
        
        _getSemaphore.set();
        
        Log::instance()->avstream().debug(Poco::format("queue put success in thread %s, size: %s",
            Poco::Thread::current()->name(), Poco::NumberFormatter::format(_queue.size())));
        return true;
    }
    
    
    T get()
    {
        Log::instance()->avstream().debug(Poco::format("queue get waiting in thread %s ...",
            Poco::Thread::current()->name()));
        
        if (!_getSemaphore.tryWait(_getTimeout)) {
            Log::instance()->avstream().error(Poco::format("queue get timed out in thread %s.",
                Poco::Thread::current()->name()));
            return false;
        }
        
        _lock.lock();
        T ret = _queue.front();
        _queue.pop();
        _lock.unlock();
        
        _putSemaphore.set();
        
        Log::instance()->avstream().debug(Poco::format("queue get success in thread %s, size: %s",
            Poco::Thread::current()->name(), Poco::NumberFormatter::format(_queue.size())));
        return ret;
    }
    
private:
    std::queue<T>           _queue;
    
    int                     _size;
    // wait _packetQueuePutTimeout ms for data to be put into the queue
    int                     _putTimeout;
    // wait _packetQueueGetTimeout ms for data to be fetched from the queue
    int                     _getTimeout;
    
    Poco::Semaphore         _putSemaphore;
    Poco::Semaphore         _getSemaphore;
    Poco::FastMutex         _lock;
};


class StreamQueue : public Queue<Frame*>
{
public:
    StreamQueue(Node* pNode, int size = 20, int putTimeout = 100, int getTimeout = 500);
    
    Node* node();
    
private:
    Node*       _pNode;
};


class StreamInfo
{
    friend class Stream;
    friend class Demuxer;
    friend class Frame;
    
public:
    StreamInfo();
    
    bool isAudio();
    bool isVideo();
    
    void findCodec();
    
    void printInfo();
    
    int width();
    int height();
    
    // audio parameters
    int sampleWidth();
    unsigned int sampleRate();
    int channels();
    
    // video parameters
    PixelFormat pixelFormat();
    int pictureSize();
    
private:
    AVStream*               _pAvStream;
    AVCodecContext*         _pAvCodecContext;
    AVCodec*                _pAvCodec;
};


class Stream
{
    friend class Demuxer;
    friend class Frame;
    
public:
    
    Stream(const std::string name = "avstream");
    ~Stream();
    
    std::string name();
    
    // TODO: extend this to handle more than one queue (tee stream)
    // getQueue(int queueNumber = 0)
    // addQueue(StreamQueue* pQueue, int queueNumber = 0)
    // removeQueue(int queueNumber = 0)
    // _pStreamQueue -> std::vector<StreamQueue*>
    StreamInfo* getInfo();
    StreamQueue* getQueue();
    void setInfo(StreamInfo* pInfo);
    void setQueue(StreamQueue* pQueue);
    
    Frame* allocateVideoFrame(PixelFormat targetFormat);
    Frame* decodeAudioFrame(Frame* pFrame);
    Frame* decodeVideoFrame(Frame* pFrame);
    
private:
    std::string         _name;
    StreamInfo*         _pStreamInfo;
    StreamQueue*        _pStreamQueue;
};


class Node : Poco::Runnable
{
public:
    Node(const std::string& name = "avstream node");
    
    void setName(const std::string& name);
    
    void start();
    void stop();
    
    void attach(Node* node, int outStreamNumber = 0, int inStreamNumber = 0);
    void detach(int outStreamNumber = 0);
    
protected:
    virtual void init() {}
    virtual void run() {}
    
    std::string                     _name;
    std::vector<Stream*>            _inStreams;
    std::vector<Stream*>            _outStreams;
    Poco::Thread                    _thread;
    Poco::FastMutex                 _lock;
    bool                            _quit;
};


class Demuxer : public Node
{
public:
    Demuxer();
    
    void set(std::istream& istr);
    void reset();
    
    int firstAudioStream();
    int firstVideoStream();
    
private:
    void init();
    
    virtual void run();
    
    Meta*       _pMeta;
    int         _firstAudioStream;
    int         _firstVideoStream;
};


class Decoder : public Node
{
public:
    Decoder();
    
private:
    virtual void init();
    virtual void run();
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


class Meta
{
    friend class Tagger;
    friend class Demuxer;
    
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


class Sink : public Node
{
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
    
    // each stream waits OMM_FRAME_PRESENTATION_TIMEOUT ms for data to be fully written to the sink
    // note: when blocked writing to the sink (e.g. audio sink), this is the maximum duration for one frame to be played
    const int               _presesentationTimeout;
    Poco::Semaphore         _presentationSemaphore;
    Poco::FastMutex         _presentationLock;
    
    bool                    _triggerClock;
};


class Clock : public Poco::Runnable
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
    
    void run();
    
    void notify();
    void notifyTimed(Poco::Timer& timer);
    
    static Clock*           _pInstance;
    
    Poco::Timer             _timer;
    Poco::Thread            _notifyThread;
    
    Sink*                   _pSink;
    long                    _frameBaseSink;
    long                    _timeLeftSink;
    
    Poco::Semaphore         _notifySemaphore;
    Poco::FastMutex         _clockLock;
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
