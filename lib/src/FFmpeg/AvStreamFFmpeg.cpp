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
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS

#include <Poco/ClassLibrary.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/Timestamp.h>

#include "AvStreamFFmpeg.h"

#define PRINT_LIB_VERSION(outstream,libname,LIBNAME,indent) \
version= libname##_version(); \
fprintf(outstream, "%slib%-10s %2d.%2d.%2d / %2d.%2d.%2d\n", indent? "  " : "", #libname, \
LIB##LIBNAME##_VERSION_MAJOR, LIB##LIBNAME##_VERSION_MINOR, LIB##LIBNAME##_VERSION_MICRO, \
version >> 16, version >> 8 & 0xff, version & 0xff);

static void print_all_lib_versions(FILE* outstream, int indent)
{
    unsigned int version;
    PRINT_LIB_VERSION(outstream, avutil,   AVUTIL,   indent);
    PRINT_LIB_VERSION(outstream, avcodec,  AVCODEC,  indent);
    PRINT_LIB_VERSION(outstream, avformat, AVFORMAT, indent);
//     PRINT_LIB_VERSION(outstream, avdevice, AVDEVICE, indent);
// #if CONFIG_AVFILTER
//     PRINT_LIB_VERSION(outstream, avfilter, AVFILTER, indent);
// #endif
// #if CONFIG_SWSCALE
//     PRINT_LIB_VERSION(outstream, swscale,  SWSCALE,  indent);
// #endif
// #if CONFIG_POSTPROC
//     PRINT_LIB_VERSION(outstream, postproc, POSTPROC, indent);
// #endif
}

void show_banner(void)
{
    std::cout <<  "libommavstream Copyright (c) 2010 Joerg Bakker." << std::endl;
    print_all_lib_versions(stdout, 1);
    std::cout << "  built on " __DATE__ " " __TIME__ ;
#ifdef __GNUC__
    std::cout << ", gcc: " __VERSION__  << std::endl;
#else
    std::cout << ", using a non-gcc compiler" << std::endl;
#endif
}


Log* Log::_pInstance = 0;

Log::Log()
{
    Poco::FormattingChannel* pFormatLogger = new Poco::FormattingChannel(new Poco::PatternFormatter("%H:%M:%S.%i %N[%P,%I] %q %s %t"));
    Poco::SplitterChannel* pSplitterChannel = new Poco::SplitterChannel;
    Poco::ConsoleChannel* pConsoleChannel = new Poco::ConsoleChannel;
    pSplitterChannel->addChannel(pConsoleChannel);
//     pSplitterChannel->addChannel(pFileChannel);
    pFormatLogger->setChannel(pSplitterChannel);
    pFormatLogger->open();
    _pFfmpegLogger = &Poco::Logger::create("FFMPEG", pFormatLogger, Poco::Message::PRIO_TRACE);
//     _pFfmpegLogger = &Poco::Logger::create("FFMPEG", pFormatLogger, Poco::Message::PRIO_ERROR);
}


Log*
Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Log::ffmpeg()
{
    return *_pFfmpegLogger;
}


FFmpegMeta::FFmpegMeta() :
_pFormatContext(0),
_pIoContext(0),
_pInputFormat(0),
_frameNumber(0),
        _useAvOpenInputStream(false),
_pIoBuffer(0)
// _totalRead(0),
// _totalReadCount(0)
{
    Log::instance()->ffmpeg().trace("ffmpeg::avformat_alloc_context() ...");
    _pFormatContext = avformat_alloc_context();
}


FFmpegMeta::~FFmpegMeta()
{
    if (_pFormatContext) {
        if (_useAvOpenInputStream) {
            Log::instance()->ffmpeg().trace("ffmpeg::av_close_input_stream() ...");
            // free AVFormatContext
            av_close_input_stream(_pFormatContext);
        }
        else {
            Log::instance()->ffmpeg().trace("ffmpeg::av_close_input_file() ...");
            av_close_input_file(_pFormatContext);
        }
    }
    if (_pInputFormat) {
//         Log::instance()->ffmpeg().trace("ffmpeg::av_freep(AVInputFormat*) ...");
//         av_freep(_pInputFormat);
    }
    if (_pIoContext) {
//         Log::instance()->ffmpeg().trace("ffmpeg::av_freep(ByteIOContext*) ...");
//         av_freep(_pIoContext);
    }
    if (_pIoBuffer) {
        delete _pIoBuffer;
        _pIoBuffer = 0;
    }
}


Omm::AvStream::Frame*
FFmpegMeta::readFrame()
{
    AVPacket packet;
    Log::instance()->ffmpeg().trace("ffmpeg::av_read_frame() ...");
    int ret = av_read_frame(_pFormatContext, &packet);
    _frameNumber++;
    if (ret < 0) {
        Log::instance()->ffmpeg().error("ffmpeg::av_read_frame() returns: " + Poco::NumberFormatter::format(ret) + " (" + errorMessage(ret) + ")");
        return new Omm::AvStream::Frame(_frameNumber, streamInfo(0), true);
    }
    FFmpegFrame* pFrame = new FFmpegFrame(_frameNumber, streamInfo(packet.stream_index));
    
    Log::instance()->ffmpeg().trace("av_dup_packet()");
    if (av_dup_packet(&packet) < 0) {
        Log::instance()->ffmpeg().warning("av_dup_packet() failed");
    }
    // allocate AVPacket struc
    Log::instance()->ffmpeg().trace("av_malloc()");
    pFrame->_pAvPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    // copy fields of AVPacket struc
    *(pFrame->_pAvPacket) = packet;
    pFrame->_data = (char*)packet.data;
    pFrame->_size = packet.size;
    
    pFrame->setPts(packet.pts);
    return pFrame;
}


void
FFmpegMeta::print(bool isOutFormat)
{
    Log::instance()->ffmpeg().trace("ffmpeg::dump_format() ...");
    if (isOutFormat) {
        dump_format(_pFormatContext, 0, _pFormatContext->filename, 1);
    }
    else {
        dump_format(_pFormatContext, 0, _pFormatContext->filename, 0);
        
        AVMetadataTag* tag = 0;
        Log::instance()->ffmpeg().trace("ffmpeg::av_metadata_get() ...");
        while ((tag = av_metadata_get(_pFormatContext->metadata, "", tag, AV_METADATA_IGNORE_SUFFIX))) {
            std::clog << tag->key << ", " << tag->value << std::endl;
        }
    }
}


int
FFmpegMeta::numberStreams()
{
    return _pFormatContext->nb_streams;
}


PixelFormat
FFmpegMeta::toFFmpegPixFmt(Meta::ColorCoding colorCoding)
{
    switch(colorCoding) {
        case Meta::CC_NONE:
            return PIX_FMT_NONE;
        case Meta::CC_YUV420P:
            return PIX_FMT_YUV420P;
        case Meta::CC_YUYV422:
            return PIX_FMT_YUYV422;
        case Meta::CC_RGB24:
            return PIX_FMT_RGB24;
        case Meta::CC_BGR24:
            return PIX_FMT_BGR24;
        case Meta::CC_YUV422P:
            return PIX_FMT_YUV422P;
        case Meta::CC_YUV444P:
            return PIX_FMT_YUV444P;
        case Meta::CC_RGB32:
            return PIX_FMT_RGB32;
        case Meta::CC_YUV410P:
            return PIX_FMT_YUV410P;
        case Meta::CC_YUV411P:
            return PIX_FMT_YUV411P;
        case Meta::CC_RGB565:
            return PIX_FMT_RGB565;
        case Meta::CC_RGB555:
            return PIX_FMT_RGB555;
        case Meta::CC_GRAY8:
            return PIX_FMT_GRAY8;
        case Meta::CC_MONOWHITE:
            return PIX_FMT_MONOWHITE;
        case Meta::CC_MONOBLACK:
            return PIX_FMT_MONOBLACK;
        case Meta::CC_PAL8:
            return PIX_FMT_PAL8;
        case Meta::CC_YUVJ420P:
            return PIX_FMT_YUVJ420P;
        case Meta::CC_YUVJ422P:
            return PIX_FMT_YUVJ422P;
        case Meta::CC_YUVJ444P:
            return PIX_FMT_YUVJ444P;
        case Meta::CC_XVMC_MPEG2_MC:
            return PIX_FMT_XVMC_MPEG2_MC;
        case Meta::CC_XVMC_MPEG2_IDCT:
            return PIX_FMT_XVMC_MPEG2_IDCT;
        case Meta::CC_UYVY422:
            return PIX_FMT_UYVY422;
        case Meta::CC_UYYVYY411:
            return PIX_FMT_UYYVYY411;
        case Meta::CC_BGR32:
            return PIX_FMT_BGR32;
        case Meta::CC_BGR565:
            return PIX_FMT_BGR565;
        case Meta::CC_BGR555:
            return PIX_FMT_BGR555;
        case Meta::CC_BGR8:
            return PIX_FMT_BGR8;
        case Meta::CC_BGR4:
            return PIX_FMT_BGR4;
        case Meta::CC_BGR4_BYTE:
            return PIX_FMT_BGR4_BYTE;
        case Meta::CC_RGB8:
            return PIX_FMT_RGB8;
        case Meta::CC_RGB4:
            return PIX_FMT_RGB4;
        case Meta::CC_RGB4_BYTE:
            return PIX_FMT_RGB4_BYTE;
        case Meta::CC_NV12:
            return PIX_FMT_NV12;
        case Meta::CC_NV21:
            return PIX_FMT_NV21;
        case Meta::CC_RGB32_1:
            return PIX_FMT_RGB32_1;
        case Meta::CC_BGR32_1:
            return PIX_FMT_BGR32_1;
        case Meta::CC_GRAY16BE:
            return PIX_FMT_GRAY16BE;
        case Meta::CC_GRAY16LE:
            return PIX_FMT_GRAY16LE;
        case Meta::CC_YUV440P:
            return PIX_FMT_YUV440P;
        case Meta::CC_YUVJ440P:
            return PIX_FMT_YUVJ440P;
        case Meta::CC_YUVA420P:
            return PIX_FMT_YUVA420P;
        case Meta::CC_VDPAU_H264:
            return PIX_FMT_VDPAU_H264;
        case Meta::CC_VDPAU_MPEG1:
            return PIX_FMT_VDPAU_MPEG1;
        case Meta::CC_VDPAU_MPEG2:
            return PIX_FMT_VDPAU_MPEG2;
        case Meta::CC_VDPAU_WMV3:
            return PIX_FMT_VDPAU_WMV3;
        case Meta::CC_VDPAU_VC1:
            return PIX_FMT_VDPAU_VC1;
        case Meta::CC_RGB48BE:
            return PIX_FMT_RGB48BE;
        case Meta::CC_RGB48LE:
            return PIX_FMT_RGB48LE;
        case Meta::CC_VAAPI_MOCO:
            return PIX_FMT_VAAPI_MOCO;
        case Meta::CC_VAAPI_IDCT:
            return PIX_FMT_VAAPI_IDCT;
        case Meta::CC_VAAPI_VLD:
            return PIX_FMT_VAAPI_VLD;
        case Meta::CC_NB:
            return PIX_FMT_NB;
    }
}


Omm::AvStream::Meta::ColorCoding
FFmpegMeta::fromFFmpegPixFmt(PixelFormat pixelFormat)
{
    switch(pixelFormat) {
        case PIX_FMT_NONE:
            return Meta::CC_NONE;
        case PIX_FMT_YUV420P:
            return Meta::CC_YUV420P;
        case PIX_FMT_YUYV422:
            return Meta::CC_YUYV422;
        case PIX_FMT_RGB24:
            return Meta::CC_RGB24;
        case PIX_FMT_BGR24:
            return Meta::CC_BGR24;
        case PIX_FMT_YUV422P:
            return Meta::CC_YUV422P;
        case PIX_FMT_YUV444P:
            return Meta::CC_YUV444P;
        case PIX_FMT_RGB32:
            return Meta::CC_RGB32;
        case PIX_FMT_YUV410P:
            return Meta::CC_YUV410P;
        case PIX_FMT_YUV411P:
            return Meta::CC_YUV411P;
        case PIX_FMT_RGB565:
            return Meta::CC_RGB565;
        case PIX_FMT_RGB555:
            return Meta::CC_RGB555;
        case PIX_FMT_GRAY8:
            return Meta::CC_GRAY8;
        case PIX_FMT_MONOWHITE:
            return Meta::CC_MONOWHITE;
        case PIX_FMT_MONOBLACK:
            return Meta::CC_MONOBLACK;
        case PIX_FMT_PAL8:
            return Meta::CC_PAL8;
        case PIX_FMT_YUVJ420P:
            return Meta::CC_YUVJ420P;
        case PIX_FMT_YUVJ422P:
            return Meta::CC_YUVJ422P;
        case PIX_FMT_YUVJ444P:
            return Meta::CC_YUVJ444P;
        case PIX_FMT_XVMC_MPEG2_MC:
            return Meta::CC_XVMC_MPEG2_MC;
        case PIX_FMT_XVMC_MPEG2_IDCT:
            return Meta::CC_XVMC_MPEG2_IDCT;
        case PIX_FMT_UYVY422:
            return Meta::CC_UYVY422;
        case PIX_FMT_UYYVYY411:
            return Meta::CC_UYYVYY411;
        case PIX_FMT_BGR32:
            return Meta::CC_BGR32;
        case PIX_FMT_BGR565:
            return Meta::CC_BGR565;
        case PIX_FMT_BGR555:
            return Meta::CC_BGR555;
        case PIX_FMT_BGR8:
            return Meta::CC_BGR8;
        case PIX_FMT_BGR4:
            return Meta::CC_BGR4;
        case PIX_FMT_BGR4_BYTE:
            return Meta::CC_BGR4_BYTE;
        case PIX_FMT_RGB8:
            return Meta::CC_RGB8;
        case PIX_FMT_RGB4:
            return Meta::CC_RGB4;
        case PIX_FMT_RGB4_BYTE:
            return Meta::CC_RGB4_BYTE;
        case PIX_FMT_NV12:
            return Meta::CC_NV12;
        case PIX_FMT_NV21:
            return Meta::CC_NV21;
        case PIX_FMT_RGB32_1:
            return Meta::CC_RGB32_1;
        case PIX_FMT_BGR32_1:
            return Meta::CC_BGR32_1;
        case PIX_FMT_GRAY16BE:
            return Meta::CC_GRAY16BE;
        case PIX_FMT_GRAY16LE:
            return Meta::CC_GRAY16LE;
        case PIX_FMT_YUV440P:
            return Meta::CC_YUV440P;
        case PIX_FMT_YUVJ440P:
            return Meta::CC_YUVJ440P;
        case PIX_FMT_YUVA420P:
            return Meta::CC_YUVA420P;
        case PIX_FMT_VDPAU_H264:
            return Meta::CC_VDPAU_H264;
        case PIX_FMT_VDPAU_MPEG1:
            return Meta::CC_VDPAU_MPEG1;
        case PIX_FMT_VDPAU_MPEG2:
            return Meta::CC_VDPAU_MPEG2;
        case PIX_FMT_VDPAU_WMV3:
            return Meta::CC_VDPAU_WMV3;
        case PIX_FMT_VDPAU_VC1:
            return Meta::CC_VDPAU_VC1;
        case PIX_FMT_RGB48BE:
            return Meta::CC_RGB48BE;
        case PIX_FMT_RGB48LE:
            return Meta::CC_RGB48LE;
        case PIX_FMT_VAAPI_MOCO:
            return Meta::CC_VAAPI_MOCO;
        case PIX_FMT_VAAPI_IDCT:
            return Meta::CC_VAAPI_IDCT;
        case PIX_FMT_VAAPI_VLD:
            return Meta::CC_VAAPI_VLD;
        case PIX_FMT_NB:
            return Meta::CC_NB;
    }
}


std::string
FFmpegMeta::errorMessage(int errorCode)
{
    std::string errMsg;
    switch (errorCode) {
//         case AVERROR_UNKNOWN:
//             errMsg = "unknown error";
//             break;
        case AVERROR_IO:
            errMsg = "I/O error";
            break;
        case AVERROR_NUMEXPECTED:
            errMsg = "Number syntax expected in filename";
            break;
        case AVERROR_INVALIDDATA:
            errMsg = "invalid data found";
            break;
        case AVERROR_NOMEM:
            errMsg = "not enough memory";
            break;
        case AVERROR_NOFMT:
            errMsg = "unknown format";
            break;
        case AVERROR_NOTSUPP:
            errMsg = "Operation not supported";
            break;
        case AVERROR_NOENT:
            errMsg = "No such file or directory";
            break;
        case AVERROR_EOF:
            errMsg = "End of file";
            break;
    }
    return errMsg;
}


// FIXME: for correct buffer sizes see libavformat/utils.c: av_open_input_file()
FFmpegTagger::FFmpegTagger() :
// _tagBufferSize(2048),
_tagBufferSize(8192),
_IoBufferSize(8192)
// _IoBufferSize(32768),
// _IoBufferSize(2048),
// _IoBufferSize(1024),
{
    // Initialize libavformat and register all the muxers, demuxers and protocols
    Log::instance()->ffmpeg().trace("ffmpeg::av_register_all() ...");
    av_register_all();
}


FFmpegTagger::~FFmpegTagger()
{
}


AVInputFormat*
FFmpegTagger::probeInputFormat(std::istream& istr)
{
    AVProbeData probeData;
    probeData.filename = "";
    Omm::AvStream::Log::instance()->avstream().debug("probing stream ...");
    
    probeData.buf_size = _tagBufferSize;
    unsigned char buffer[_tagBufferSize];
    probeData.buf = buffer;
    istr.read((char*)probeData.buf, _tagBufferSize);
    int bytes = istr.gcount();
    Omm::AvStream::Log::instance()->avstream().debug("read " + Poco::NumberFormatter::format(bytes) + " bytes from stream");
    if (istr.bad() || bytes == 0) {
        Omm::AvStream::Log::instance()->avstream().error("error reading probe data");
        return 0;
    }
    
    Omm::AvStream::Log::instance()->avstream().debug("detecting format ...");
    Log::instance()->ffmpeg().trace("ffmpeg::av_probe_input_format() ...");
    AVInputFormat* pInputFormat = av_probe_input_format(&probeData, 1 /*int is_opened*/);
    if (pInputFormat) {
        Omm::AvStream::Log::instance()->avstream().information("AV stream format: " + std::string(pInputFormat->name) + " (" + std::string(pInputFormat->long_name) + ")");
    }
    else {
        Omm::AvStream::Log::instance()->avstream().error("AV stream format unknown");
        return 0;
    }
    
    // this stops ffmpeg from seeking the stream, but av_probe_input_format() returns 0 when called after setting this flag.
//     pInputFormat->flags |= AVFMT_NOFILE;
    
    // TODO: reset the stream after probing (if seekable)
    // Seek back to 0
    // copied from url_fseek
//     long offset1 = 0 - (_pIoContext->pos - (_pIoContext->buf_end - _pIoContext->buffer));
//     if (offset1 >= 0 && offset1 <= (_pIoContext->buf_end - _pIoContext->buffer)) {
//             /* can do the seek inside the buffer */
//         _pIoContext->buf_ptr = _pIoContext->buffer + offset1;
//     } else {
//         if (!d->src->seek(0)) {
//             return false;
//         } else {
//             _pIoContext->pos = 0;
//             _pIoContext->buf_ptr = d->file_buffer;
//             _pIoContext->buf_end = d->file_buffer;
//         }
//     }
    
    return pInputFormat;
}


int
FFmpegTagger::IOOpen(URLContext* pUrlContext, const char* filename, int flags)
{
    Omm::AvStream::Log::instance()->avstream().debug("IOOpen file: " + std::string(filename) + " ...");
    std::ifstream* pInputStream = (std::ifstream*)pUrlContext->priv_data;
    if (!pInputStream) {
        Omm::AvStream::Log::instance()->avstream().error("IOOpen failed, std::istream not set");
        return -1;
    }
    pInputStream->open(filename);
    Omm::AvStream::Log::instance()->avstream().debug("IOOpen file: " + std::string(filename) + " success.");
    return 0;
}


int
FFmpegTagger::IOClose(URLContext* pUrlContext)
{
    Omm::AvStream::Log::instance()->avstream().debug("IOClose ...");
    std::ifstream* pInputStream = (std::ifstream*)pUrlContext->priv_data;
    if (!pInputStream) {
        Omm::AvStream::Log::instance()->avstream().error("IOClose failed, std::istream not set");
        return -1;
    }
    pInputStream->close();
    Omm::AvStream::Log::instance()->avstream().debug("IOClose success.");
    return 0;
}


static Poco::Timestamp _timestamp;
static Poco::Timestamp _startTimestamp;
static uint64_t _totalBytes = 0;

int
FFmpegTagger::IORead(void *opaque, uint8_t *buf, int buf_size)
{
    URLContext* pUrlContext = (URLContext*)opaque;
    
    std::istream* pInputStream = (std::istream*)pUrlContext->priv_data;
    if (!pInputStream) {
        Omm::AvStream::Log::instance()->avstream().error("IORead failed, std::istream not set");
        return -1;
    }
    
    pInputStream->read((char*)buf, buf_size);
    Poco::Timestamp::TimeDiff time = _timestamp.elapsed();
    _timestamp.update();
    Poco::Timestamp::TimeDiff startTime = _startTimestamp.elapsed();
    int bytes = pInputStream->gcount();
    _totalBytes += bytes;
    if (!pInputStream->good()) {
        Omm::AvStream::Log::instance()->avstream().error("IORead failed to read from std::istream");
        return -1;
    }
    Omm::AvStream::Log::instance()->avstream().trace("IORead() bytes read: " + Poco::NumberFormatter::format(bytes) + " in " + Poco::NumberFormatter::format(time/1000.0, 3) + " msec (" +  Poco::NumberFormatter::format(bytes*1000/time) + " kB/s), total : " + 
    Poco::NumberFormatter::format(_totalBytes/1000) + "kB in " + Poco::NumberFormatter::format(startTime/1000000) + " sec (" + Poco::NumberFormatter::format(_totalBytes*1000/startTime) + "kB/s)");
    return bytes;
}


int64_t
FFmpegTagger::IOSeek(void *opaque, int64_t offset, int whence)
{
    Omm::AvStream::Log::instance()->avstream().trace("IOSeek() offset: " + Poco::NumberFormatter::format(offset));
    
    URLContext* pUrlContext = (URLContext*)opaque;
//     Omm::AvStream::Log::instance()->avstream().trace("IOSeek() pUrlContext pointer: " + Poco::NumberFormatter::format(pUrlContext));
//     Omm::AvStream::Log::instance()->avstream().trace("IOSeek() URLProtocol name: " + std::string(pUrlContext->prot->name));
    
    std::istream* pInputStream = (std::istream*)pUrlContext->priv_data;
    if (!pInputStream) {
        Omm::AvStream::Log::instance()->avstream().error("IOSeek failed, std::istream not set");
        return -1;
    }
    
    pInputStream->seekg(offset);
//     pInputStream->seekg(offset, std::ios::cur);
    if (!pInputStream->good()) {
        Omm::AvStream::Log::instance()->avstream().error("IOSeek failed to seek std::istream");
//         return pInputStream->tellg();
//         return 0;
    }
    return pInputStream->tellg();
//     return offset;
//     totalRead = offset;
//     return totalRead;
}


ByteIOContext*
FFmpegTagger::initIo(std::istream& istr, bool isSeekable, unsigned char* pIoBuffer)
{
    static char streamName[] = "std::istream";
    
    // FIXME: deallocate URLContext after stream has finished.
    URLContext* pUrlContext = new URLContext;
    pUrlContext->is_streamed = 1;
//     pUrlContext->priv_data = 0;
    pUrlContext->priv_data = &istr;
    pUrlContext->filename = streamName;
    pUrlContext->prot = new URLProtocol;
    pUrlContext->prot->name = "OMM avio wrapper for std::iostream";
    pUrlContext->prot->next = 0;
    pUrlContext->prot->url_open = 0;
//     pUrlContext->prot->url_open = (int (*)(URLContext *, const char *, int))IOOpen;
    pUrlContext->prot->url_read = (int (*) (URLContext *, unsigned char *, int))FFmpegTagger::IORead;
    pUrlContext->prot->url_write = 0;
    if (isSeekable) {
        pUrlContext->prot->url_seek = (int64_t (*) (URLContext *, int64_t, int))FFmpegTagger::IOSeek;
    }
    else {
        pUrlContext->prot->url_seek = 0;
    }
    pUrlContext->prot->url_close = 0;
//     pUrlContext->prot->url_close = (int (*)(URLContext *))IOClose;
    
    Log::instance()->ffmpeg().trace("ffmpeg::av_alloc_put_byte() ...");
    ByteIOContext* pIoContext;
    if (isSeekable) {
        pIoContext = av_alloc_put_byte(pIoBuffer, _IoBufferSize, 0, pUrlContext, FFmpegTagger::IORead, 0, FFmpegTagger::IOSeek);
    }
    else {
        pIoContext = av_alloc_put_byte(pIoBuffer, _IoBufferSize, 0, pUrlContext, FFmpegTagger::IORead, 0, 0);
        pIoContext->is_streamed = 1;
    }
    pIoContext->max_packet_size = _IoBufferSize;
    
    return pIoContext;
}


Omm::AvStream::Meta*
FFmpegTagger::tag(const std::string& uri)
{
    FFmpegMeta* pMeta = new FFmpegMeta;
    int error;
    
    pMeta->_useAvOpenInputStream = false;
    
//     AVFormatParameters avFormatParameters;
//     memset(&avFormatParameters, 0, sizeof(avFormatParameters));
//     avFormatParameters.prealloced_context = 1;
//     pMeta->_pFormatContext->probesize = 5000;
//     pMeta->_pFormatContext->max_analyze_duration = 1000000;
    
    Log::instance()->ffmpeg().trace("ffmpeg::av_open_input_file() ...");
//     error = av_open_input_file(&pMeta->_pFormatContext, uri.c_str(), 0, 0, &avFormatParameters);
    error = av_open_input_file(&pMeta->_pFormatContext, uri.c_str(), 0, 0, 0);
    if (error < 0) {
        Omm::AvStream::Log::instance()->avstream().error("av_open_input_file() failed.");
        return 0;
    }
    
    Log::instance()->ffmpeg().trace("ffmpeg::av_find_stream_info() ...");
    error = av_find_stream_info(pMeta->_pFormatContext);
    if (error < 0) {
        Omm::AvStream::Log::instance()->avstream().error("av_find_stream_info() failed, could not find codec parameters");
        return 0;
    }
    
    for(int streamNr = 0; streamNr < pMeta->_pFormatContext->nb_streams; streamNr++) {
        FFmpegStreamInfo* pStreamInfo = new FFmpegStreamInfo;
        pStreamInfo->_pAvStream = pMeta->_pFormatContext->streams[streamNr];
        pMeta->addStream(pStreamInfo);
    }
    
    return pMeta;
}


Omm::AvStream::Meta*
FFmpegTagger::tag(std::istream& istr)
{
    FFmpegMeta* pMeta = new FFmpegMeta;
    int error;
    pMeta->_pInputFormat = probeInputFormat(istr);
    
    pMeta->_pIoBuffer = new unsigned char[_IoBufferSize];
    pMeta->_pIoContext = initIo(istr, false, pMeta->_pIoBuffer);
//     pMeta->_pIoContext = initIo(istr, true, pMeta->_pIoBuffer);

    pMeta->_useAvOpenInputStream = true;
    
    AVFormatParameters avFormatParameters;
    memset(&avFormatParameters, 0, sizeof(avFormatParameters));
    avFormatParameters.prealloced_context = 1;
    pMeta->_pFormatContext->probesize = 20000;
    pMeta->_pFormatContext->max_analyze_duration = 5000000;
//     pMeta->_pFormatContext->flags |= AVFMT_FLAG_NONBLOCK;

    Log::instance()->ffmpeg().trace("ffmpeg::av_open_input_stream() ...");
    // FIXME: av_open_input_stream needs to read several megabytes of a TS.
    error = av_open_input_stream(&pMeta->_pFormatContext, pMeta->_pIoContext, "std::istream", pMeta->_pInputFormat, &avFormatParameters);
//     error = av_open_input_stream(&pMeta->_pFormatContext, pMeta->_pIoContext, "std::istream", pMeta->_pInputFormat, 0);
//     error = av_open_input_file(&pMeta->_pFormatContext, "std::istream", pMeta->_pInputFormat, 0, 0);
    if (error < 0) {
        Omm::AvStream::Log::instance()->avstream().error("av_open_input_stream() failed");
        return 0;
    }
    Log::instance()->ffmpeg().trace("probesize: " + Poco::NumberFormatter::format(pMeta->_pFormatContext->probesize) +
    ", max_analyze_duration: " + Poco::NumberFormatter::format(pMeta->_pFormatContext->max_analyze_duration));
    
    Log::instance()->ffmpeg().trace("ffmpeg::av_find_stream_info() ...");
    error = av_find_stream_info(pMeta->_pFormatContext);
    if (error < 0) {
        Omm::AvStream::Log::instance()->avstream().error("av_find_stream_info() failed, could not find codec parameters");
        return 0;
    }
    
//     pMeta->_pFormatContext->flags |= AVFMT_FLAG_NONBLOCK;
    
//     if (pMeta->_pFormatContext->flags & AVFMT_FLAG_NONBLOCK) {
//         std::clog << "frame reading is set to NONBLOCK" << std::endl;
//     }
//     else {
//         std::clog << "frame reading is set to BLOCK" << std::endl;
//     }
    
    for(int streamNr = 0; streamNr < pMeta->_pFormatContext->nb_streams; streamNr++) {
        FFmpegStreamInfo* pStreamInfo = new FFmpegStreamInfo;
        pStreamInfo->_pAvStream = pMeta->_pFormatContext->streams[streamNr];
        pMeta->addStream(pStreamInfo);
    }
    
    return pMeta;
}


FFmpegStreamInfo::FFmpegStreamInfo() :
_pDecodedAudioFrame(0),
_pDecodedVideoFrame(0),
_pAvStream(0),
_pAvCodecContext(0),
_pAvCodec(0),
_maxDecodedAudioFrameSize((AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2),
_pImgConvertContext(0)
{
}


FFmpegStreamInfo::~FFmpegStreamInfo()
{
    if (_pAvCodecContext) {
        Log::instance()->ffmpeg().trace("ffmpeg::avcodec_flush_buffers() ...");
        avcodec_flush_buffers(_pAvCodecContext);
        Log::instance()->ffmpeg().trace("ffmpeg::avcodec_close() ...");
        avcodec_close(_pAvCodecContext);
    }
    if (_pDecodedVideoFrame) {
        delete _pDecodedVideoFrame;
        _pDecodedVideoFrame = 0;
    }
    if (_pDecodedAudioFrame) {
        delete _pDecodedAudioFrame;
        _pDecodedAudioFrame = 0;
    }
}


bool
FFmpegStreamInfo::isAudio()
{
    return _pAvStream->codec->codec_type == CODEC_TYPE_AUDIO;
}


bool
FFmpegStreamInfo::isVideo()
{
    return _pAvStream->codec->codec_type == CODEC_TYPE_VIDEO;
}


bool
FFmpegStreamInfo::findCodec()
{
    if (!_pAvStream || !_pAvStream->codec) {
        Omm::AvStream::Log::instance()->avstream().error("missing stream info in " + getName() + " while trying to find decoder");
        return false;
    }
    _pAvCodecContext = _pAvStream->codec;
    
    //////////// find decoders for audio and video stream ////////////
    Omm::AvStream::Log::instance()->avstream().debug("searching codec with codec id: " +
        Poco::NumberFormatter::format(_pAvCodecContext->codec_id));
    
    Log::instance()->ffmpeg().trace("ffmpeg::avcodec_find_decoder() ...");
    _pAvCodec = avcodec_find_decoder(_pAvCodecContext->codec_id);
    
    if(!_pAvCodec) {
        Omm::AvStream::Log::instance()->avstream().error("could not find decoder for codec id: " +
            Poco::NumberFormatter::format(_pAvCodecContext->codec_id));
        return false;
    }
    
    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
//     if(_pAvCodec->capabilities & CODEC_CAP_TRUNCATED) {
//         _pAvCodecContext->flags |= CODEC_FLAG_TRUNCATED;
//     }
    
    Log::instance()->ffmpeg().trace("ffmpeg::avcodec_open() ...");
    if(avcodec_open(_pAvCodecContext, _pAvCodec) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("could not open decoder for codec id: " +
            Poco::NumberFormatter::format(_pAvCodecContext->codec_id));
        return false;
    }
    Omm::AvStream::Log::instance()->avstream().information("found codec: " + std::string(_pAvCodec->name) + " (" + std::string(_pAvCodec->long_name) + ")");
    Omm::AvStream::Log::instance()->avstream().information("start time: " + Poco::NumberFormatter::format(_pAvStream->start_time) + ", duration: " +
        Poco::NumberFormatter::format(_pAvStream->duration));
    
    // time_base: fundamental unit of time (in seconds) in terms of which frame timestamps are represented. 
    // This is the fundamental unit of time (in seconds) in terms
    // of which frame timestamps are represented. For fixed-fps content,
    // time base should be 1/framerate and timestamp increments should be 1.
    Omm::AvStream::Log::instance()->avstream().information("time base numerator: " + Poco::NumberFormatter::format(_pAvStream->time_base.num) + ", denominator: " +Poco::NumberFormatter::format(_pAvStream->time_base.den));
    
    // r_frame_rate: Real base framerate of the stream. 
    Omm::AvStream::Log::instance()->avstream().information("base frame rate numerator: " + Poco::NumberFormatter::format(_pAvStream->r_frame_rate.num) + ", denominator: " + Poco::NumberFormatter::format(_pAvStream->r_frame_rate.den));
    
//     Omm::AvStream::Log::instance()->avstream().information(Poco::format("average frame rate numerator: %s, denominator: %s",
//         Poco::NumberFormatter::format(_pAvStream->avg_frame_rate.num),
//         Poco::NumberFormatter::format(_pAvStream->avg_frame_rate.den)));
    
    // reference dts (for timestamp generation): Timestamp corresponding to the last dts sync point
    // Initialized when AVCodecParserContext.dts_sync_point >= 0 and
    // a DTS is received from the underlying container. Otherwise set to
    // AV_NOPTS_VALUE by default.
    Omm::AvStream::Log::instance()->avstream().information("first dts: " + Poco::NumberFormatter::format(_pAvStream->first_dts) + ", current dts: " + Poco::NumberFormatter::format(_pAvStream->cur_dts) + ", reference dts: " + Poco::NumberFormatter::format(_pAvStream->reference_dts) + ", last IP pts: " + Poco::NumberFormatter::format(_pAvStream->last_IP_pts) + ", last IP duration: " +Poco::NumberFormatter::format(_pAvStream->last_IP_duration));
    
//     Omm::AvStream::Log::instance()->avstream().trace(Poco::format("_pStreamInfo->_pAvCodecContext->codec_id %s",
//         Poco::NumberFormatter::format(_pAvCodecContext->codec_id)));

    if (isAudio()) {
    //     _maxDecodedAudioFrameSize = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2;
        _pDecodedAudioFrame = new FFmpegFrame(0, this, _maxDecodedAudioFrameSize);
        // FIXME: set _data[_maxDecodedAudioFrameSize - 1] = 0 in base class
//         _pDecodedAudioFrame->data()[_maxDecodedAudioFrameSize - 1] = 0;
    }
    else if (isVideo()) {
        Log::instance()->ffmpeg().trace("ffmpeg::avcodec_alloc_frame() ...");
        _pDecodedVideoFrame = new FFmpegFrame(0, this, avcodec_alloc_frame());
    }
    
    return true;
    
    
//     if(_pVideoCodec->frame_rate > 1000 && _pVideoCodec->frame_rate_base == 1) {
//         _pVideoCodec->frame_rate_base = 1000;
//     }
}


bool
FFmpegStreamInfo::findEncoder()
{
    Omm::AvStream::Log::instance()->avstream().debug("searching encoder with codec id: " +
        Poco::NumberFormatter::format(_pAvCodecContext->codec_id));
    
    Log::instance()->ffmpeg().trace("ffmpeg::avcodec_find_encoder() ...");
    _pAvCodec = avcodec_find_encoder(_pAvCodecContext->codec_id);
    
    if(!_pAvCodec) {
        Omm::AvStream::Log::instance()->avstream().error("could not find encoder for codec id: " +
            Poco::NumberFormatter::format(_pAvCodecContext->codec_id));
        return false;
    }
    
    Log::instance()->ffmpeg().trace("ffmpeg::avcodec_open() ...");
    if(avcodec_open(_pAvCodecContext, _pAvCodec) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("could not open encoder for codec id: " +
            Poco::NumberFormatter::format(_pAvCodecContext->codec_id));
        return false;
    }
    Omm::AvStream::Log::instance()->avstream().information("found encoder: " + std::string(_pAvCodec->name) + " (" + std::string(_pAvCodec->long_name) + ")");
    
    return true;
}


// StreamInfo*
// FFmpegStreamInfo::cloneOutStreamInfo(Meta* pMeta, int outStreamNumber)
// {
//     FFmpegStreamInfo* pRes = new StreamInfo(_pStream);
//     
//     Log::instance()->ffmpeg().debug("ffmpeg::av_new_stream()");
//     pRes->_pAvStream = av_new_stream(pMeta->_pFormatContext, outStreamNumber);
//     if(!pRes->_pAvStream) {
//         Omm::AvStream::Log::instance()->avstream().error("could not allocate stream");
//         return pRes;
//     }
//     pRes->_pAvCodecContext = pRes->_pAvStream->codec;
//     if(!pRes->_pAvCodecContext) {
//         Omm::AvStream::Log::instance()->avstream().error("could not allocate codec context");
//         return pRes;
//     }
// //     pRes->_pAvStream->r_frame_rate = _pAvStream->r_frame_rate;
// //     pRes->_pAvStream->time_base = _pAvStream->time_base;
//     
//     pRes->_pAvCodecContext->codec_id = _pAvCodecContext->codec_id;
//     pRes->_pAvCodecContext->codec_type = _pAvCodecContext->codec_type;
// //     pRes->_pAvCodecContext->bit_rate = _pAvCodecContext->bit_rate;
//     if (isAudio()) {
//         pRes->_pAvCodecContext->sample_rate = _pAvCodecContext->sample_rate;
//         pRes->_pAvCodecContext->channels = _pAvCodecContext->channels;
//     }
//     else if (isVideo()) {
//         pRes->_pAvCodecContext->time_base = _pAvCodecContext->time_base;
//         pRes->_pAvCodecContext->pix_fmt = _pAvCodecContext->pix_fmt;
//         pRes->_pAvCodecContext->width = _pAvCodecContext->width;
//         pRes->_pAvCodecContext->height = _pAvCodecContext->height;
// //         pRes->_pAvCodecContext->gop_size = _pAvCodecContext->gop_size;
// //         pRes->_pAvCodecContext->me_method = _pAvCodecContext->me_method;
// //         pRes->_pAvCodecContext->max_b_frames = _pAvCodecContext->max_b_frames;
// //         pRes->_pAvCodecContext->mb_decision = _pAvCodecContext->mb_decision;
// //         pRes->_pAvCodecContext->flags = _pAvCodecContext->flags;
// //         pRes->_pAvCodecContext->sample_aspect_ratio = _pAvCodecContext->sample_aspect_ratio;
//     }
//     return pRes;
// }


int
FFmpegStreamInfo::sampleSize()
{
    Omm::AvStream::Log::instance()->avstream().debug("stream sample format: " +
        Poco::NumberFormatter::format((int)_pAvStream->codec->sample_fmt));
    
    // avcodec.h says sample_fmt is not used (always S16?)
    switch(_pAvStream->codec->sample_fmt) {
    case SAMPLE_FMT_U8:
        return 1; // beware unsigned!
        break;
    case SAMPLE_FMT_S16:
        return 2;
        break;
    case SAMPLE_FMT_S32:
        return 4;
        break;
    case SAMPLE_FMT_FLT:
        return -4;
        break;
    default:
        return 0;
    }
}


unsigned int
FFmpegStreamInfo::sampleRate()
{
//     Omm::AvStream::Log::instance()->avstream().debug("stream sample rate: " +
//         Poco::NumberFormatter::format((int)_pAvStream->codec->sample_rate));
    
    return _pAvStream->codec->sample_rate;
}


int
FFmpegStreamInfo::channels()
{
//     Omm::AvStream::Log::instance()->avstream().debug("stream channels: " +
//         Poco::NumberFormatter::format((int)_pAvStream->codec->channels));
    
    return _pAvStream->codec->channels;
}


int
FFmpegStreamInfo::width()
{
    if (_pAvCodecContext) {
        return _pAvCodecContext->width;
    }
    else {
        return 0;
    }
}


int
FFmpegStreamInfo::height()
{
    if (_pAvCodecContext) {
        return _pAvCodecContext->height;
    }
    else {
        return 0;
    }
}


Omm::AvStream::Meta::ColorCoding
FFmpegStreamInfo::pixelFormat()
{
    return FFmpegMeta::fromFFmpegPixFmt(_pAvCodecContext->pix_fmt);
}


int
FFmpegStreamInfo::pictureSize()
{
    return avpicture_get_size(_pAvCodecContext->pix_fmt, width(), height());
}


float
FFmpegStreamInfo::aspectRatio()
{
    float aspectRatio;
    if (_pAvStream->sample_aspect_ratio.num) {
        aspectRatio = av_q2d(_pAvStream->sample_aspect_ratio);
    }
    else if (_pAvCodecContext && _pAvCodecContext->sample_aspect_ratio.num) {
        aspectRatio = av_q2d(_pAvCodecContext->sample_aspect_ratio);
    }
    else {
        aspectRatio = 1.0;
    }
    if (aspectRatio < 0.0) {
        aspectRatio = 1.0;
    }
    return aspectRatio;
}


Omm::AvStream::Frame*
FFmpegStreamInfo::allocateVideoFrame(Omm::AvStream::Meta::ColorCoding targetFormat)
{
    // Determine required buffer size and allocate buffer
    Log::instance()->ffmpeg().trace("ffmpeg::avpicture_get_size() ...");
    int numBytes = avpicture_get_size(FFmpegMeta::toFFmpegPixFmt(targetFormat), width(), height());
    Log::instance()->ffmpeg().trace("ffmpeg::av_malloc() ...");
    uint8_t* buffer = (uint8_t *)av_malloc(numBytes);
    Log::instance()->ffmpeg().trace("ffmpeg::avcodec_alloc_frame() ...");
    AVFrame* pRes = avcodec_alloc_frame();
    Log::instance()->ffmpeg().trace("ffmpeg::avpicture_fill() ...");
    avpicture_fill((AVPicture *)pRes, buffer, FFmpegMeta::toFFmpegPixFmt(targetFormat), width(), height());
    
    return new FFmpegFrame(newFrameNumber(), this, pRes);
}


Omm::AvStream::Frame*
FFmpegStreamInfo::decodeAudioFrame(Omm::AvStream::Frame* pFrame)
{
    _pDecodedAudioFrame->_size = _maxDecodedAudioFrameSize;
    
    uint8_t* inBuffer = (uint8_t*)pFrame->data();
    int inBufferSize = pFrame->size();
    // TODO: does this while loop work for uncompressed packets ?
    // (it's only needed for e.g. .wav, where on packet contains multiple uncompressed frames
    while(inBufferSize > 0) {
        Log::instance()->ffmpeg().trace("ffmpeg::avcodec_decode_audio2() ...");
        int bytesConsumed = avcodec_decode_audio2(_pAvCodecContext,
            (int16_t*)_pDecodedAudioFrame->_data, &_pDecodedAudioFrame->_size,
            inBuffer, inBufferSize);
        
        Log::instance()->ffmpeg().debug("ffmpeg::avcodec_decode_audio2() frame size: " + Poco::NumberFormatter::format(inBufferSize) + ", bytes consumed: " + Poco::NumberFormatter::format(bytesConsumed) + ", decoded size: " +
            Poco::NumberFormatter::format(_pDecodedAudioFrame->size())
            );
        
        if (bytesConsumed < 0 || _pDecodedAudioFrame->size() == 0) {
            Omm::AvStream::Log::instance()->avstream().warning("decoding audio frame in stream " + getName() + " failed, discarding frame.");
            delete pFrame;
            return 0;
        }
        inBuffer += bytesConsumed;
        inBufferSize -= bytesConsumed;
    }
    return _pDecodedAudioFrame;
}


Omm::AvStream::Frame*
FFmpegStreamInfo::decodeVideoFrame(Omm::AvStream::Frame* pFrame)
{
//     _pDecodedVideoFrame->_number = pFrame->getNumber();

    int decodeSuccess;
    AVFrame* pDecodedAvFrame = static_cast<FFmpegFrame*>(_pDecodedVideoFrame)->_pAvFrame;
    Log::instance()->ffmpeg().trace("ffmpeg::avcodec_decode_video() ...");
    int bytesConsumed = avcodec_decode_video(_pAvCodecContext,
                                             pDecodedAvFrame, &decodeSuccess,
                                            (uint8_t*)pFrame->data(), pFrame->size());

    std::string success(decodeSuccess ? "success" : "failed");
    Log::instance()->ffmpeg().debug("ffmpeg::avcodec_decode_video() " + success +", bytes consumed: " + Poco::NumberFormatter::format(bytesConsumed) + ", linesize[0..2]: " + Poco::NumberFormatter::format(pDecodedAvFrame->linesize[0]) + "," + Poco::NumberFormatter::format(pDecodedAvFrame->linesize[1]) + "," + Poco::NumberFormatter::format(pDecodedAvFrame->linesize[2]));
    
    if (decodeSuccess <= 0 || bytesConsumed <= 0) {
        Omm::AvStream::Log::instance()->avstream().warning("decoding video frame in stream " + getName() +" failed, discarding frame.");
        // FIXME: when decoding of video failes, original frame is deleted? 
        return 0;
    }
    return _pDecodedVideoFrame;
}


FFmpegFrame::FFmpegFrame(int64_t number, Omm::AvStream::StreamInfo* pStreamInfo, int dataSize) :
Frame(number, pStreamInfo),
_pAvPacket(0),
_pAvFrame(0)
{
    Omm::AvStream::Log::instance()->avstream().trace("alloc " + getName() + ", size " + Poco::NumberFormatter::format(dataSize));
    _data = new char[dataSize];
    _size = dataSize;
}


FFmpegFrame::FFmpegFrame(int64_t number, Omm::AvStream::StreamInfo* pStreamInfo) :
Frame(number, pStreamInfo),
_pAvPacket(0),
_data(0),
_size(0),
_pAvFrame(0)
{
}


FFmpegFrame::FFmpegFrame(int64_t number, Omm::AvStream::StreamInfo* pStreamInfo, AVFrame* pAvFrame) :
Frame(number, pStreamInfo),
_pAvPacket(0),
_pAvFrame(pAvFrame)
{
}


FFmpegFrame::~FFmpegFrame()
{
    Omm::AvStream::Log::instance()->avstream().trace("frame dtor ...");
    if (isEndOfStream()) {
        Log::instance()->ffmpeg().trace("video frame dtor, end of stream frame");
    }
    else if (_pAvPacket) {
       // called for non-decoded packets
        Log::instance()->ffmpeg().trace("delete " + getName() + " dtor, ffmpeg::av_free_packet() ...");
        av_free_packet(_pAvPacket);
        Log::instance()->ffmpeg().trace("delete " + getName() + " dtor, ffmpeg::av_free() ...");
        av_free(_pAvPacket);
        _pAvPacket = 0;
        _data = 0;
        _size = 0;
    }
    else if (_pAvFrame) {
        // called for Stream::_pDecodedVideoFrame
        Log::instance()->ffmpeg().trace("video frame dtor, delete _pAvFrame");
        av_free(_pAvFrame);
        _pAvFrame = 0;
    }
    else if (_data) {
        // called for Stream::_pDecodedAudioFrame
        Omm::AvStream::Log::instance()->avstream().trace("audio frame dtor, delete _data, size " + Poco::NumberFormatter::format(size()));
        delete _data;
        _data = 0;
        _size = 0;
    }
    Omm::AvStream::Log::instance()->avstream().trace("frame dtor finished.");
}


const char*
FFmpegFrame::data()
{
    return _data;
}


const int
FFmpegFrame::size()
{
//     Omm::AvStream::Log::instance()->avstream().debug("Frame::size()");
    Poco::ScopedLock<Poco::Mutex> lock(_sizeLock);
    return _size;
}


char*
FFmpegFrame::planeData(int plane)
{
    return (char*)_pAvFrame->data[plane];
}


int
FFmpegFrame::planeSize(int plane)
{
    return _pAvFrame->linesize[plane];
}


Omm::AvStream::Frame*
FFmpegFrame::convert(Omm::AvStream::Meta::ColorCoding targetFormat, int targetWidth, int targetHeight)
{
    getStreamInfo()->printInfo();
    int width = getStreamInfo()->width();
    int height = getStreamInfo()->height();
    if (targetWidth == -1) {
        targetWidth = width;
    }
    if (targetHeight == -1) {
        targetHeight = height;
    }
    Omm::AvStream::Meta::ColorCoding inPixFormat = getStreamInfo()->pixelFormat();
    
    Omm::AvStream::Log::instance()->avstream().debug("source pixelFormat: " + Poco::NumberFormatter::format(inPixFormat) + ", target pixelFormat: " + Poco::NumberFormatter::format(targetFormat));
    
    int scaleAlgo = SWS_BICUBIC;
//     struct SwsContext *pImgConvertContext = 0;
    
    struct SwsContext* pConvertContext = static_cast<FFmpegStreamInfo*>(getStreamInfo())->_pImgConvertContext;
    Log::instance()->ffmpeg().trace("ffmpeg::sws_getCachedContext() ...");
    static_cast<FFmpegStreamInfo*>(getStreamInfo())->_pImgConvertContext = sws_getCachedContext(pConvertContext,
                                              width, height, FFmpegMeta::toFFmpegPixFmt(inPixFormat),
                                              width, height, FFmpegMeta::toFFmpegPixFmt(targetFormat),
                                              scaleAlgo, NULL, NULL, NULL);
    pConvertContext = static_cast<FFmpegStreamInfo*>(getStreamInfo())->_pImgConvertContext;
    
    if (pConvertContext == 0) {
        Omm::AvStream::Log::instance()->avstream().warning("cannot initialize image conversion context");
        return 0;
    }
    else {
        Omm::AvStream::Log::instance()->avstream().debug("success: image conversion context set up.");
    }
    
    // FIXME: _pStream->pCodecContext is wrong with pOutFrame, because e.g. pix_fmt changed
    FFmpegFrame* pRes = static_cast<FFmpegFrame*>(getStreamInfo()->allocateVideoFrame(targetFormat));
    
    printInfo();
    Log::instance()->ffmpeg().trace("ffmpeg::sws_scale() ...");
    sws_scale(pConvertContext,
              _pAvFrame->data, _pAvFrame->linesize,
              0, height,
              pRes->_pAvFrame->data, pRes->_pAvFrame->linesize);
    
    pRes->printInfo();
    
    return pRes;
}


void
FFmpegFrame::write(Omm::AvStream::Overlay* pOverlay)
{
    Omm::AvStream::Log::instance()->avstream().debug("convert video frame to overlay pixel format ... ");
    
    getStreamInfo()->printInfo();
    int streamWidth = getStreamInfo()->width();
    int streamHeight = getStreamInfo()->height();
    Omm::AvStream::Meta::ColorCoding streamPixelFormat = getStreamInfo()->pixelFormat();
    
//     int targetWidth = pOverlay->getWidth();
//     int targetHeight = pOverlay->getHeight();
    int targetWidth = getStreamInfo()->width();
    int targetHeight = getStreamInfo()->height();

    Omm::AvStream::Meta::ColorCoding targetPixelFormat = pOverlay->getFormat();
    
    Omm::AvStream::Log::instance()->avstream().debug("stream pixelFormat: " + Poco::NumberFormatter::format(streamPixelFormat) + ", target pixelFormat: " +
        Poco::NumberFormatter::format(targetPixelFormat));
    
    int scaleAlgo = SWS_BICUBIC;
    
    struct SwsContext* pConvertContext = static_cast<FFmpegStreamInfo*>(getStreamInfo())->_pImgConvertContext;
    Log::instance()->ffmpeg().trace("ffmpeg::sws_getCachedContext() ... ");
    static_cast<FFmpegStreamInfo*>(getStreamInfo())->_pImgConvertContext = sws_getCachedContext(pConvertContext,
                                              streamWidth, streamHeight, FFmpegMeta::toFFmpegPixFmt(streamPixelFormat),
                                              targetWidth, targetHeight, FFmpegMeta::toFFmpegPixFmt(targetPixelFormat),
                                              scaleAlgo, NULL, NULL, NULL);
    pConvertContext = static_cast<FFmpegStreamInfo*>(getStreamInfo())->_pImgConvertContext;

    if (pConvertContext == 0) {
        Omm::AvStream::Log::instance()->avstream().warning("cannot initialize image conversion context");
        return;
    }
    else {
        Omm::AvStream::Log::instance()->avstream().debug("success: image conversion context set up.");
    }
    
    printInfo();
    Log::instance()->ffmpeg().trace("ffmpeg::sws_scale() ...");
    int outSlizeHeight = sws_scale(pConvertContext,
                                   _pAvFrame->data, _pAvFrame->linesize,
                                   0, streamHeight,
                                   pOverlay->_data, pOverlay->_pitch);
    Log::instance()->ffmpeg().debug("ffmpeg::sws_scale() frame written to overlay.");
}


void
FFmpegFrame::printInfo()
{
    if (!_pAvFrame) {
        return;
    }
    Omm::AvStream::Log::instance()->avstream().debug("frame linesize[0..2]: " + Poco::NumberFormatter::format(_pAvFrame->linesize[0]) + ", " + Poco::NumberFormatter::format(_pAvFrame->linesize[1]) + ", "+
        Poco::NumberFormatter::format(_pAvFrame->linesize[2]));
    
//    Omm::AvStream::Log::instance()->avstream().debug("frame data[0..2]: " + Poco::NumberFormatter::format((void*)_pAvFrame->data[0]) + ", " + Poco::NumberFormatter::format((unsigned int)_pAvFrame->data[1]) + ", " + Poco::NumberFormatter::format((unsigned int)_pAvFrame->data[2]));
}


int
FFmpegFrame::streamIndex()
{
    if (_pAvPacket) {
        return _pAvPacket->stream_index;
    }
    else {
        return Frame::streamIndex();
    }
}


int
FFmpegFrame::duration()
{
    if (_pAvPacket) {
        return _pAvPacket->duration;
    }
    else {
        return Frame::duration();
    }
}


POCO_BEGIN_MANIFEST(Omm::AvStream::Tagger)
POCO_EXPORT_CLASS(FFmpegTagger)
POCO_END_MANIFEST
