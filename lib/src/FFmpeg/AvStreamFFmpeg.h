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
#ifndef AvStreamFFmpeg_INCLUDED
#define AvStreamFFmpeg_INCLUDED

#include "AvStream.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}


class Log
{
public:
    static Log* instance();
    Poco::Logger& ffmpeg();
    
private:
    Log();
    
    static Log*     _pInstance;
    Poco::Logger*   _pFfmpegLogger;
};


class FFmpegMeta : public Omm::AvStream::Meta
{
    friend class FFmpegTagger;
    
public:
    FFmpegMeta();
    virtual ~FFmpegMeta();
    
    virtual Omm::AvStream::Frame* readFrame();
    virtual void print(bool isOutFormat = false);
    virtual int numberStreams();
    
    static PixelFormat toFFmpegPixFmt(Meta::ColorCoding colorCoding);
    static Meta::ColorCoding fromFFmpegPixFmt(PixelFormat pixelFormat);

private:
    std::string errorMessage(int errorCode);

    AVFormatContext*    _pFormatContext; // contains pointers to ByteIOContext, AVInputFormat, codec ids, array of streams with AVCodecContext
    ByteIOContext*      _pIoContext;
    AVInputFormat*      _pInputFormat;  // contains the io access callbacks
    uint64_t            _frameNumber;
    
    bool                _useAvOpenInputStream;
    unsigned char*      _pIoBuffer;
//     long long           _totalRead;
//     long long           _totalReadCount;
};


class FFmpegTagger : public Omm::AvStream::Tagger
{
public:
    FFmpegTagger();
    ~FFmpegTagger();

    virtual Omm::AvStream::Meta* tag(const std::string& uri);
    virtual Omm::AvStream::Meta* tag(std::istream& istr);
    
private:
    AVInputFormat* probeInputFormat(std::istream& istr);
    ByteIOContext* initIo(std::istream& istr, bool isSeekable, unsigned char* pIoBuffer);
    static int IOOpen(URLContext* pUrlContext, const char* filename, int flags);
    static int IOClose(URLContext* pUrlContext);
    static int IORead(void *opaque, uint8_t *buf, int buf_size);
    static int64_t IOSeek(void *opaque, int64_t offset, int whence);
    
    const int            _tagBufferSize;
    const int            _IoBufferSize;
};


class FFmpegFrame : public Omm::AvStream::Frame
{
    friend class FFmpegMeta;
    friend class FFmpegStreamInfo;
    
public:
    FFmpegFrame(int64_t number, Omm::AvStream::StreamInfo* pStreamInfo);
    FFmpegFrame(int64_t number, Omm::AvStream::StreamInfo* pStreamInfo, int dataSize);
    FFmpegFrame(int64_t number, Omm::AvStream::StreamInfo* pStreamInfo, AVFrame* pAvFrame);
    virtual ~FFmpegFrame();

    virtual const char* data();
    virtual const int size();
    virtual char* planeData(int plane);
    virtual int planeSize(int plane);
    virtual Frame* convert(Omm::AvStream::Meta::ColorCoding targetFormat, int targetWidth = -1, int targetHeight = -1);
    virtual void write(Omm::AvStream::Overlay* overlay);
    virtual void printInfo();
    virtual int streamIndex();
    virtual int duration();
    
private:
    Poco::Mutex         _sizeLock;
    // face 1: audio/video packet coming from the demuxer (_data also points to _pAvPacket->data)
    AVPacket*           _pAvPacket;
    // face 2: _data points to decoded audio data
    char*               _data;
    int                 _size;
    // face 3: decoded video frame
    AVFrame*            _pAvFrame;
};


class FFmpegStreamInfo : public Omm::AvStream::StreamInfo
{
    friend class FFmpegFrame;
    friend class FFmpegTagger;
    
public:
    FFmpegStreamInfo();
    virtual ~FFmpegStreamInfo();
    
    virtual bool isAudio();
    virtual bool isVideo();
    virtual bool findCodec();
    virtual bool findEncoder();
//     virtual StreamInfo* cloneOutStreamInfo(Meta* pMeta, int outStreamNumber);
    
    // audio parameters
    virtual int sampleWidth();
    virtual unsigned int sampleRate();
    virtual int channels();
    
    // video parameters
    virtual int width();
    virtual int height();
    virtual Omm::AvStream::Meta::ColorCoding pixelFormat();
    virtual int pictureSize();
    virtual float aspectRatio();
    
private:
    virtual Omm::AvStream::Frame* allocateVideoFrame(Omm::AvStream::Meta::ColorCoding targetFormat);
    virtual Omm::AvStream::Frame* decodeAudioFrame(Omm::AvStream::Frame* pFrame);
    virtual Omm::AvStream::Frame* decodeVideoFrame(Omm::AvStream::Frame* pFrame);
    
    FFmpegFrame*            _pDecodedAudioFrame;
    FFmpegFrame*            _pDecodedVideoFrame;
    
    AVStream*               _pAvStream;
    AVCodecContext*         _pAvCodecContext;
    AVCodec*                _pAvCodec;
    struct SwsContext*      _pImgConvertContext;
    int                     _maxDecodedAudioFrameSize;
};


#endif