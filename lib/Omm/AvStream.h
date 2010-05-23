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
        Log::instance()->avstream().trace(Poco::format("queue put waiting in thread %s ...",
            Poco::Thread::current()->name()));
        
        if (!_putSemaphore.tryWait(_putTimeout)) {
            Log::instance()->avstream().warning(Poco::format("queue put timed out in thread %s.",
                Poco::Thread::current()->name()));
            return false;
        }
        
        _lock.lock();
        _queue.push(element);
        _lock.unlock();
        
        _getSemaphore.set();
        
        Log::instance()->avstream().trace(Poco::format("queue put success in thread %s, size: %s",
            Poco::Thread::current()->name(), Poco::NumberFormatter::format(_queue.size())));
        return true;
    }
    
    
    T get()
    {
        Log::instance()->avstream().trace(Poco::format("queue get waiting in thread %s ...",
            Poco::Thread::current()->name()));
        
        if (!_getSemaphore.tryWait(_getTimeout)) {
            Log::instance()->avstream().warning(Poco::format("queue get timed out in thread %s.",
                Poco::Thread::current()->name()));
            return false;
        }
        
        _lock.lock();
        T ret = _queue.front();
        _queue.pop();
        _lock.unlock();
        
        _putSemaphore.set();
        
        Log::instance()->avstream().trace(Poco::format("queue get success in thread %s, size: %s",
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
    StreamQueue(Node* pNode, int size = 20, int putTimeout = 500, int getTimeout = 500);
    
    Node* getNode();
    
private:
    Node*       _pNode;
};


// FIXME: access to StreamInfo may be locked here and there
class StreamInfo
{
    friend class Stream;
    friend class Demuxer;
    friend class Frame;
    
public:
    StreamInfo();
    
    bool isAudio();
    bool isVideo();
    
    bool findCodec();
    
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
    std::string             _streamName;
    AVStream*               _pAvStream;
    AVCodecContext*         _pAvCodecContext;
    AVCodec*                _pAvCodec;
};


class Stream
{
    friend class Demuxer;
    friend class Frame;
    
public:
    // TODO: in and out stream ctors (only in streams allocate a queue)
    Stream(Node* node, const std::string name = "avstream");
    ~Stream();
    
    Frame* getFrame();
    void putFrame(Frame* pFrame);
    
    Node* getNode();
    
    // TODO: extend this to handle more than one queue (tee stream)
    // getQueue(int queueNumber = 0)
    // addQueue(StreamQueue* pQueue, int queueNumber = 0)
    // removeQueue(int queueNumber = 0)
    // _pStreamQueue -> std::vector<StreamQueue*>
    std::string getName();
    StreamInfo* getInfo();
    StreamQueue* getQueue();
    void setName(const std::string& name);
    void setInfo(StreamInfo* pInfo);
    void setQueue(StreamQueue* pQueue);
    
    Frame* allocateVideoFrame(PixelFormat targetFormat);
    Frame* decodeFrame(Frame* pFrame);
    
private:
    Frame* decodeAudioFrame(Frame* pFrame);
    Frame* decodeVideoFrame(Frame* pFrame);
    
    // _pNode of a Stream and of a StreamQueue can differ, as StreamQueue is shared between in and out streams
    Node*               _pNode;
    // _pStreamInfo and _pStreamQueue can be shared between in and out streams or are 0 if not attached
    StreamInfo*         _pStreamInfo;
    // _pStreamQueue always belongs to the input stream of a node
    StreamQueue*        _pStreamQueue;
};


class Node : Poco::Runnable
{
public:
    Node(const std::string& name = "avstream node");
    
    std::string getName();
    void setName(const std::string& name);
    
    void start();
    void stop();
    
    void attach(Node* node, int outStreamNumber = 0, int inStreamNumber = 0);
    void detach(int outStreamNumber = 0);
    
    Node* getDownstreamNode(int outStreamNumber = 0);
    
protected:
    virtual bool init() {}
    virtual void run() {}
    
//     void setStop(bool stop);
//     bool doStop();
    
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
    bool init();
    
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
    virtual bool init();
    virtual void run();
};


class Frame
{
    friend class Stream;
    
public:
    Frame(const Frame& frame);
    Frame(Stream* pStream);
    Frame(Stream* pStream, int dataSize);
    Frame(Stream* pStream, char* data, int dataSize);
    
    // copies *pAvPacket (and it's payload) into Frame and deletes *pAvPacket (and it's payload)
    Frame(Stream* pStream, AVPacket* pAvPacket);
    // copies *pAvFrame (and it's payload) into Frame and deletes *pAvFrame (and it's payload)
    Frame(Stream* pStream, AVFrame* pAvFrame);
    ~Frame();
    
    char* data();
    int size();
    int paddedSize();
    
    char* planeData(int plane);
    int planeSize(int plane);
    
    void printInfo();
    
    Stream* getStream();
    
    Frame* decode();
    
    // width and height = -1 means, leave the size of the frame as it is
    Frame* convert(PixelFormat targetFormat, int targetWidth = -1, int targetHeight = -1);
    
    void writePpm(const std::string& fileName);
    void write(Overlay* overlay);
    
    AVPacket* copyPacket(AVPacket* pAvPacket, int padSize);
    AVFrame* allocateFrame(PixelFormat format);
    AVFrame* copyFrame(AVFrame* pAvFrame);
    
private:
    // Frame must be a dynamic structure with three different "faces", determined at runtime.
    // face 1: simple buffer
    char*               _data;
    int                 _size;
    int                 _paddedSize;
    // face 2: packet coming from the demuxer
    AVPacket*           _pAvPacket;
    // face 3: decoded frame
    AVFrame*            _pAvFrame;
    
    // reference to the stream this frame is contained in
    Stream*             _pStream;
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
    friend class Clock;
    
public:
    Sink(const std::string& name = "sink");
    virtual ~Sink() {}
    
    static Sink* loadPlugin(const std::string& libraryPath, const std::string& className = "SinkPlugin");
    virtual int eventLoop() { return 0; }
};


class AudioSink : public Sink
{
    AudioSink(const std::string& name = "audio sink");
};


class VideoSink : public Sink
{
public:
    VideoSink(const std::string& name = "video sink");
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
