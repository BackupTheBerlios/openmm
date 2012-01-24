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

#include <stdint.h>

#include <Poco/Logger.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Runnable.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Notification.h>
#include <Poco/Mutex.h>
#include <Poco/RWLock.h>
#include <Poco/Condition.h>
#include <Poco/Timer.h>
#include <Poco/DateTime.h>
#include <Poco/NotificationCenter.h>

#undef CC_NONE

namespace Omm {
namespace AvStream {

class Tagger;
class Meta;
class Node;
class Demuxer;
class Frame;
class Stream;
class StreamInfo;
class StreamQueue;
class Overlay;
class Sink;
class Clock;


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


class RingBuffer
{
public:
    RingBuffer(int size);
    ~RingBuffer();

    /**
    NOTE: this is no generic implemenation of a ring buffer:
    1. read() and write() don't check if num > size
    2. it is not thread safe
    this is all done in the customer ByteQueue
    **/
    void read(char* buffer, int num);
    void write(const char* buffer, int num);

    void clear();

private:
    char*                   _ringBuffer;
    char*                   _readPos;
    char*                   _writePos;
    int                     _size;
};


/**
class ByteQueue - a blocking byte stream with a fixed size
**/
class ByteQueue
{
public:
    ByteQueue(int size);

    /**
    read() and write() block until num bytes have been read or written
    **/
    void read(char* buffer, int num);
    void write(const char* buffer, int num);

    /**
    readSome() and writeSome() read upto num bytes, return the number of bytes read
    and block if queue is empty / full
    **/
    int readSome(char* buffer, int num);
    int writeSome(const char* buffer, int num);

    int size();
    int level();
    void clear();

    bool full();
    bool empty();

private:
    RingBuffer              _ringBuffer;
    int                     _size;
    int                     _level;
    Poco::FastMutex         _lock;
    Poco::Condition         _writeCondition;
    Poco::Condition         _readCondition;
};


template<typename T>
class Queue
{
public:
    Queue(const std::string& name, int size) :
        _name(name),
        _size(size)
    {
    }


    void put(T element)
    {
        _queueLock.lock();
        if (_queue.size() == _size) {
            _putCondition.wait<Poco::FastMutex>(_queueLock);
        }
        _queue.push(element);
        _getCondition.broadcast();
        _queueLock.unlock();
    }


    T get()
    {
        _queueLock.lock();
        if (_queue.size() == 0) {
            _getCondition.wait<Poco::FastMutex>(_queueLock);
        }
        T ret = _queue.front();
        _queue.pop();
        _putCondition.broadcast();
        _queueLock.unlock();
        return ret;
    }


    T front()
    {
        Poco::ScopedLock<Poco::FastMutex> queueLock(_queueLock);
        if (_queue.size() == 0) {
            return T();
        }
        return _queue.front();
    }


    const int count()
    {
        Poco::ScopedLock<Poco::FastMutex> queueLock(_queueLock);
        return _queue.size();
    }


    const bool full()
    {
        Poco::ScopedLock<Poco::FastMutex> queueLock(_queueLock);
        return _queue.size() == _size;
    }


    const bool empty()
    {
        Poco::ScopedLock<Poco::FastMutex> queueLock(_queueLock);
        return _queue.size() == 0;
    }


    const std::string& getName()
    {
        Poco::ScopedLock<Poco::FastMutex> lock(_nameLock);
        return _name;
    }


    void clear()
    {
        while (_queue.size()) {
            get();
        }
    }

private:
    std::string             _name;
    Poco::FastMutex         _nameLock;
    std::queue<T>           _queue;
    Poco::FastMutex         _queueLock;
    int                     _size;

    Poco::Condition         _putCondition;
    Poco::Condition         _getCondition;
};


class StreamQueue : public Queue<Frame*>
{
public:
    StreamQueue(Node* pNode, int size = 100);

    Node* getNode();

private:
    Node*       _pNode;
};


class Meta
{
    friend class Tagger;
    friend class StreamInfo;
    friend class Demuxer;
    friend class Muxer;

public:
    virtual ~Meta();

    enum ContainerFormat {
        CF_UNKNOWN = 0,
        CF_AUDIO,
        CF_VIDEO,
        CF_IMAGE,
        CF_PLAYLIST
    };

    enum TagKey {
        TK_TITLE = 0,
        TK_ALBUM,
        TK_ARTIST,
        TK_AUTHOR,
        TK_GENRE,
        TK_TRACK
    };

    enum ColorCoding {
        CC_NONE = -1,
        CC_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
        CC_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
        CC_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
        CC_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
        CC_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
        CC_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
        CC_RGB32,     ///< packed RGB 8:8:8, 32bpp, (msb)8A 8R 8G 8B(lsb), in CPU endianness
        CC_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
        CC_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
        CC_RGB565,    ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), in CPU endianness
        CC_RGB555,    ///< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), in CPU endianness, most significant bit to 0
        CC_GRAY8,     ///<        Y        ,  8bpp
        CC_MONOWHITE, ///<        Y        ,  1bpp, 0 is white, 1 is black
        CC_MONOBLACK, ///<        Y        ,  1bpp, 0 is black, 1 is white
        CC_PAL8,      ///< 8 bit with CC_RGB32 palette
        CC_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG)
        CC_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG)
        CC_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG)
        CC_XVMC_MPEG2_MC,///< XVideo Motion Acceleration via common packet passing
        CC_XVMC_MPEG2_IDCT,
        CC_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
        CC_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
        CC_BGR32,     ///< packed RGB 8:8:8, 32bpp, (msb)8A 8B 8G 8R(lsb), in CPU endianness
        CC_BGR565,    ///< packed RGB 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), in CPU endianness
        CC_BGR555,    ///< packed RGB 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), in CPU endianness, most significant bit to 1
        CC_BGR8,      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
        CC_BGR4,      ///< packed RGB 1:2:1,  4bpp, (msb)1B 2G 1R(lsb)
        CC_BGR4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
        CC_RGB8,      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
        CC_RGB4,      ///< packed RGB 1:2:1,  4bpp, (msb)1R 2G 1B(lsb)
        CC_RGB4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
        CC_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 for UV
        CC_NV21,      ///< as above, but U and V bytes are swapped

        CC_RGB32_1,   ///< packed RGB 8:8:8, 32bpp, (msb)8R 8G 8B 8A(lsb), in CPU endianness
        CC_BGR32_1,   ///< packed RGB 8:8:8, 32bpp, (msb)8B 8G 8R 8A(lsb), in CPU endianness

        CC_GRAY16BE,  ///<        Y        , 16bpp, big-endian
        CC_GRAY16LE,  ///<        Y        , 16bpp, little-endian
        CC_YUV440P,   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
        CC_YUVJ440P,  ///< planar YUV 4:4:0 full scale (JPEG)
        CC_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
        CC_VDPAU_H264,///< H.264 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
        CC_VDPAU_MPEG1,///< MPEG-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
        CC_VDPAU_MPEG2,///< MPEG-2 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
        CC_VDPAU_WMV3,///< WMV3 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
        CC_VDPAU_VC1, ///< VC-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
        CC_RGB48BE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, big-endian
        CC_RGB48LE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, little-endian
        CC_VAAPI_MOCO, ///< HW acceleration through VA API at motion compensation entry-point, Picture.data[0] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers
        CC_VAAPI_IDCT, ///< HW acceleration through VA API at IDCT entry-point, Picture.data[0] contains a vaapi_render_state struct which contains fields extracted from headers
        CC_VAAPI_VLD,  ///< HW decoding through VA API, Picture.data[0] contains a vaapi_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
        CC_NB,        ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
    };

    static const Poco::Int64 invalidPts;

    virtual Frame* readFrame() {}
    // FIXME: this should be generic code, using the tags determined by Tagger::tag() and stored in Meta
    virtual void print(bool isOutFormat = false) {}
    virtual bool isStillImage() { return false; }
    virtual std::string getMime() { return ""; }

    ContainerFormat getContainerFormat();
    std::string getTag(TagKey key);
    int numberStreams();
    void addStream(StreamInfo* pStreamInfo);
    StreamInfo* streamInfo(int streamNumber);
    void setIsPlaylist(bool isPlaylist = true);

protected:
    Meta();

    std::string getTag(const std::string& key1);
    std::string getTag(const std::string& key1, const std::string& key2);
    std::string getTag(const std::string& key1, const std::string& key2, const std::string& key3);
    std::string getTag(const std::string& key1, const std::string& key2, const std::string& key3, const std::string& key4);
    std::string getTag(const std::string& key1, const std::string& key2, const std::string& key3, const std::string& key4, const std::string& key5);
    void setTag(const std::string& key, const std::string& value);

private:
    std::vector<StreamInfo*>            _streamInfos;
    std::map<std::string,std::string>   _tags;
    bool                                _isPlaylist;
};


class Tagger
{
public:
    virtual Meta* tag(const std::string& uri) = 0;
    virtual Meta* tag(std::istream& istr) = 0;
};


class StreamInfo
{
    friend class Stream;
    friend class Demuxer;
    friend class Muxer;
    friend class Frame;
    friend class Sink;

public:
    virtual ~StreamInfo();

    virtual bool isAudio() = 0;
    virtual bool isVideo() = 0;
    virtual bool findCodec() = 0;
    virtual bool findEncoder() = 0;
//     virtual StreamInfo* cloneOutStreamInfo(Meta* pMeta, int outStreamNumber) = 0;

    // audio parameters
    virtual int sampleSize() = 0;
    virtual unsigned int sampleRate() = 0;
    virtual int channels() = 0;

    // video parameters
    virtual int width() = 0;
    virtual int height() = 0;
    virtual Meta::ColorCoding pixelFormat() = 0;
    virtual int pictureSize() = 0;
    virtual float aspectRatio() = 0;
    virtual Frame* allocateVideoFrame(Meta::ColorCoding targetFormat) { return 0; }

    std::string getName();
    void setName(const std::string& name);
    void printInfo();
    Poco::Int64 newFrameNumber();
    Frame* decodeFrame(Frame* pFrame);

protected:
    StreamInfo(const std::string name = "avstream");

    virtual Frame* decodeAudioFrame(Frame* pFrame) { return 0; }
    virtual Frame* decodeVideoFrame(Frame* pFrame) { return 0; }

private:
    std::string             _streamName;
    Poco::Int64             _newFrameNumber;
    Poco::FastMutex         _lock;
};


class Stream
{
    friend class Demuxer;
    friend class Frame;
    friend class Sink;

public:
    // TODO: in and out stream ctors (only in streams allocate a queue)
    virtual ~Stream();

    Frame* firstFrame();
    Frame* getFrame();
    void putFrame(Frame* pFrame);

    void setNode(Node* pNode);
    Node* getNode();

    // TODO: extend this to handle more than one queue (tee stream)
    // getQueue(int queueNumber = 0)
    // addQueue(StreamQueue* pQueue, int queueNumber = 0)
    // removeQueue(int queueNumber = 0)
    // _pStreamQueue -> std::vector<StreamQueue*>
    StreamInfo* getInfo();
    StreamQueue* getQueue();
    void setInfo(StreamInfo* pInfo);
    void setQueue(StreamQueue* pQueue);

protected:
    Stream(Node* node);

private:
    // _pNode of a Stream and of a StreamQueue can differ, as StreamQueue is shared between in and out streams
    Node*               _pNode;
    // _pStreamInfo and _pStreamQueue can be shared between in and out streams or are 0 if not attached
    StreamInfo*         _pStreamInfo;
    // _pStreamQueue always belongs to the input stream of a node
    StreamQueue*        _pStreamQueue;

    Poco::FastMutex     _lock;
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


    Stream* getInStream(int inStreamNumber);
    Stream* getOutStream(int outStreamNumber);

protected:
    virtual bool init() { return true; }
    virtual void run() {}

    void initiateStop();
    void waitForStop();

    bool getStop();
    void setStop(bool stop);
    void reset();

    std::string                     _name;
    Poco::FastMutex                 _nameLock;
    std::vector<Stream*>            _inStreams;
    Poco::FastMutex                 _inStreamsLock;
    std::vector<Stream*>            _outStreams;
    Poco::FastMutex                 _outStreamsLock;
    bool                            _quit;
    Poco::FastMutex                 _quitLock;
    Poco::Thread                    _thread;
};


class Demuxer : public Node
{
public:
    Demuxer();
    void set(Meta* pMeta);
    void reset();

    int firstAudioStream();
    int firstVideoStream();

private:
    virtual bool init();
    virtual void run();

    Poco::Int64 correctPts(Poco::Int64 pts, Poco::Int64 lastPts, int lastDuration);

    Meta*       _pMeta;
    int         _firstAudioStream;
    int         _firstVideoStream;
};


/**
    @class Muxer
    mux one audio and one video stream
    container format is the same as input stream
*/
class Muxer : public Node
{
public:
//     Muxer();
    ~Muxer();

//     void set(const std::string& uri = "mux.ts", const std::string& format = "mpegts");
    void set(std::ostream& ostr, const std::string& format = "mpegts");
    void reset();

private:
//     virtual bool init();
//     virtual void run();

//     bool prepareOutStream(int streamNumber);

    Meta*           _pMeta;
    std::string     _uri;
};


class Frame
{
    friend class Stream;
    friend class StreamInfo;
    friend class Muxer;

public:
    Frame(Poco::Int64 number, StreamInfo* pStreamInfo, bool endOfStream = false);
    virtual ~Frame();

    virtual const char* data() { return 0; }
    virtual const int size() { return 0; }
    virtual char* planeData(int plane) { return 0; }
    virtual int planeSize(int plane) { return 0; }
    // width and height = -1 means, leave the size of the frame as it is
    virtual Frame* convert(Meta::ColorCoding targetFormat, int targetWidth = -1, int targetHeight = -1) { return 0; }
    virtual void write(Overlay* overlay) {}
    virtual void printInfo() {}
    virtual int streamIndex() { return -1; }
    virtual int duration() { return -1; }

    bool isEndOfStream();
    std::string getName();
    const Poco::Int64 getNumber();
    void setNumber(Poco::Int64 frameNumber);
    const Poco::Int64 getPts();
    void setPts(Poco::Int64 pts);

    StreamInfo* getStreamInfo();

    void writePpm(const std::string& fileName);

private:
    // Frame must be a dynamic structure with three different "faces", determined at runtime.
    // because the stream type is dynamic: audio streams decode audio packets, same goes for video.
    Poco::Int64         _number;
    Poco::Mutex         _numberLock;
    Poco::Int64         _pts;
    Poco::Mutex         _ptsLock;
    // reference to the stream this frame is contained in
    StreamInfo*         _pStreamInfo;
    Poco::FastMutex     _nameLock;
    bool                _endOfStream;
};


/*
sinks in warmup phase:
1. audio and video set start time to first succesfully decoded frame
2. clock waits until all registered sinks set their start time
3. clock decides wether to start with min(start times) or max(start times)
-> min(start times): set clock
-> max(start times): tell all sinks to discard old frames and set clock

sinks in running phase:
1. don't start before warmup phase is finished
...
*/

class Sink : public Node
{
    friend class Clock;

public:
    Sink(const std::string& name = "sink");
    virtual ~Sink() {}

    virtual void startPresentation() {}
    virtual void stopPresentation() {}
    virtual void waitPresentationStop() {}

    void currentTime(Poco::Int64 time);

    void registerStreamEventObserver(const Poco::AbstractObserver* pObserver);
    class EndOfStream : public Poco::Notification {};

protected:
    virtual bool checkInStream() {}
    virtual bool initDevice() {}
    virtual bool closeDevice() {}
    virtual void writeDecodedFrame(Frame* pDecodedFrame) {}
    virtual void stopSinkPresentation() {}

    void reset();

    Queue<Poco::Int64>      _timeQueue;
    bool                    _firstDecodeSuccess;
    Poco::Int64             _startTime;

private:
    virtual bool init();
    virtual void run();

    Poco::NotificationCenter    _streamEventNotifier;
};


class AudioSink : public Sink
{
    friend class Clock;

public:
    AudioSink(const std::string& name = "audio sink");
    virtual ~AudioSink();

    void setStartTime(Poco::Int64 startTime, bool toFirstFrame);

    // methods for the audio driver (public, so it can be called form C callbacks)
    bool audioAvailable();
    int audioRead(char* buffer, int size);
    void audioReadBlocking(char* buffer, int size);
    void initSilence(char* buffer, int size);
    void setVolume(float vol);
    virtual void setVolume(int channel, float vol) {}

    void halfByteQueue();
    void clearByteQueue();
    void reset();

protected:
    int channels();
    unsigned int sampleRate();
    unsigned int sampleSize();
    int frameCount(int bytes);
    int byteCount(int frames);
    int silence();

    ByteQueue                   _byteQueue;

private:
    virtual bool checkInStream();
    virtual void writeDecodedFrame(Frame* pDecodedFrame);
    virtual void stopSinkPresentation();
    void setVolume(char* buffer, int size);
    Poco::Int64 audioLength(Poco::Int64 bytes);

    Poco::Int64                 _audioTime;
    float                       _volume;
    Poco::FastMutex             _volumeLock;
    Clock*                      _pClock;
};


class Overlay;

class VideoSink : public Sink
{
    friend class Clock;
    friend class Overlay;

public:
    VideoSink(const std::string& name = "video sink",
         int width = 720, int height = 576, Meta::ColorCoding pixelFormat = Meta::CC_YUV420P,
         int overlayCount = 5, bool fullScreen = false);

    virtual void openWindow(bool fullScreen, int width, int height) {}
    void reset();

protected:
    virtual void displayFrame(Overlay* pOverlay) {}
    virtual void blankDisplay() {}
    int getWidth();
    int getHeight();
    Meta::ColorCoding getFormat();

    virtual int displayWidth();
    virtual int displayHeight();

    void displayRect(int& x, int& y, int& w, int& h);


    int                         _overlayCount;
    std::vector<Overlay*>       _overlayVector;
    Queue<Overlay*>             _overlayQueue;

    bool                        _timerQuit;
    bool                        _fullScreen;
    int                         _width;
    int                         _height;
    Meta::ColorCoding           _pixelFormat;

private:
    virtual bool checkInStream();
    void setStartTime(Poco::Int64 startTime);
    virtual void writeDecodedFrame(Frame* pDecodedFrame);
    virtual void startPresentation();
    virtual void waitPresentationStop();
    virtual void stopPresentation();
    virtual void stopSinkPresentation();

    void onTick(Poco::Int64 time);
    void timerThread();
    void setTimerStop(bool stop);
    bool getTimerStop();

    Poco::Thread                        _timerThread;
    Poco::RunnableAdapter<VideoSink>    _timerThreadRunnable;
    Poco::FastMutex                     _sinkLock;
    Poco::FastMutex                     _timerLock;

    int                                 _writeOverlayNumber;
    float                               _lastAspectRatio;
};


class Overlay
{
public:
    Overlay(VideoSink* pVideoSink, int width, int height);

    int getWidth();
    int getHeight();
    Meta::ColorCoding getFormat();

    uint8_t*        _data[4];
    int             _pitch[4];

    VideoSink*      _pVideoSink;
    Poco::Int64     _pts;
    int             _width;
    int             _height;
    float           _aspectRatio;
};


class Clock
{
public:
    Clock();

    /** attachAudioSink() / attachVideoSink()
        pSink: pointer to sink, that receives the timing signals
    **/
    void attachAudioSink(AudioSink* pAudioSink);
    void attachVideoSink(VideoSink* pVideoSink);

    void setStartTime(bool toFirstFrame = true);

    /** setTime()
        sets clock's current stream time to currentTime and notifies sinks
    **/
    void setTime(Poco::Int64 currentTime);

    // start clock
    void start();
    void stop();
    void reset();

private:
    void clockTick(Poco::Timer& timer);

    // TODO: should Clock be able to sync more than one stream?
    // -> store a stream time for each stream
    Poco::Int64             _streamTime;  // stream current time in stream time base units [sec]
    float                   _streamBase;  // stream refresh rate in kHz ( = 1 / (time base * 1000))
    Poco::Timestamp         _systemTime;

    Poco::Timer             _clockTimer;
    long                    _clockTick;
    long                    _clockTickStreamBase;

    std::vector<AudioSink*> _audioSinkVec;
    std::vector<VideoSink*> _videoSinkVec;

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
