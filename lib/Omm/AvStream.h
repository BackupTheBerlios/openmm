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
namespace Av {

class Meta;
class Stream;
class Overlay;
class Sink;


class AvStream
{
public:
    AvStream();
    ~AvStream();
    
    void set(std::istream& istr);
    void reset();
    
    void demux();
    
    Stream* audioStream();
    Stream* videoStream();
    
//     void stop();
    
private:
    void init();
    
    Meta*                       _pMeta;
    std::vector<Stream*>        _streams;
    Stream*                     _pAudioStream;
    Stream*                     _pVideoStream;
    
};


class Frame
{
    friend class Stream;
    
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
    char*       _data;
    int         _size;
    AVPacket*   _pAvPacket;
    AVFrame*    _pAvFrame;
    Stream*     _pStream;
};


class Stream
{
    friend class AvStream;
    friend class Frame;
    
public:
    void open();
    void close();
    
    Frame* allocateVideoFrame(PixelFormat targetFormat);
    
    void put(Frame* frame);
    Frame* get();
    
    bool isAudio();
    bool isVideo();
    
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
    
    void attachSink(Sink* pSink);
    
private:
    Frame* decodeAudioFrame(Frame* pFrame);
    Frame* decodeVideoFrame(Frame* pFrame);
    
    AVStream*               _pAvStream;
    AVCodecContext*         _pAvCodecContext;
    AVCodec*                _pAvCodec;
    Sink*                   _pSink;
    std::queue<Frame*>      _packetQueue;
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
    
public:
    virtual ~Sink() {}
    virtual void writeFrame(Frame* pFrame) = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void pause() {}
    virtual void resume() {}
    
    virtual int latency() { return 0; }
    virtual int eventLoop() { return 0; }
    
//     virtual void initGui() = 0;
//     virtual void showMainWindow() = 0;
    
protected:
    Stream*     _pStream;
};


class Scanner
{
};


class Transcoder
{
};



} // namespace Av
} // namespace Omm

#endif
