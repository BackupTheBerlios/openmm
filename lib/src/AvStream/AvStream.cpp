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

#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include <Poco/ClassLoader.h>

#include <fstream>

#include "AvStream.h"

using namespace Omm;
using namespace Omm::Av;


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
    fprintf(stderr, "libommavstream Copyright (c) 2010 Joerg Bakker.\n");
    print_all_lib_versions(stderr, 1);
    fprintf(stderr, "  built on " __DATE__ " " __TIME__);
#ifdef __GNUC__
    fprintf(stderr, ", gcc: " __VERSION__ "\n");
#else
    fprintf(stderr, ", using a non-gcc compiler\n");
#endif
}


AvStream::AvStream() :
_pMeta(0),
_pAudioStream(0),
_pVideoStream(0)
{
    // Initialize libavformat and register all the muxers, demuxers and protocols
    av_register_all();  // can be called several times
//     show_banner();
}


AvStream::~AvStream()
{

}


void
AvStream::set(std::istream& istr)
{
    Tagger tagger;
    _pMeta = tagger.tag(istr);
    _pMeta->print();
    
    init();
}


void
AvStream::init()
{
    AVFormatContext* pFormatContext = _pMeta->_pFormatContext;
    
    for(int streamNr = 0; streamNr < pFormatContext->nb_streams; streamNr++) {
        //////////// allocate packet queues ////////////
        _streams.push_back(new Stream);
        _streams.back()->_pAvStream = pFormatContext->streams[streamNr];
        
        //////////// find first audio and video stream ////////////
        if (!_pAudioStream && _streams.back()->isAudio()) {
            std::clog << "found audio stream #" << streamNr << std::endl;
            _pAudioStream = _streams.back();
        }
        if (!_pVideoStream && _streams.back()->isVideo()) {
            std::clog << "found video stream #" << streamNr << std::endl;
            _pVideoStream = _streams.back();
        }
    }
}


void
AvStream::reset()
{
    if (_pMeta) {
        delete _pMeta;
    }
    _pMeta = 0;
    
    for(std::vector<Stream*>::iterator it = _streams.begin(); it != _streams.end(); ++it) {
        delete *it;
    }
}


void
AvStream::demux()
{
    // TODO: save copying of AVPacket packet into Frame and read it directly into frame with av_read_frame()
    int i = 0;
    AVPacket packet;
    while (true) {
        av_init_packet(&packet);
        int ret = av_read_frame(_pMeta->_pFormatContext, &packet);
        if (ret < 0) {
            std::clog << "av_read_frame() returns: " << ret << std::endl;
            if (ret == AVERROR_EOF) {
                std::clog << "eof reached." << std::endl;
            }
            break;
        }
        std::clog << "reading packet #" << Poco::NumberFormatter::format0(++i, 3);
        std::clog << " type: " /*<< (_streams[packet.stream_index]->isAudio() ? "audio" : "video")*/;
        if (_streams[packet.stream_index]->isAudio()) {
            std::clog << "audio";
        }
        else if (_streams[packet.stream_index]->isVideo()) {
            std::clog << "video";
        }
        else {
            std::clog << "other";
        }
        std::clog << " pts: " << packet.pts << " dts: " << packet.dts;
        std::clog << " size: " << packet.size /*<< " pos: " << packet.pos*/;
        std::clog << std::endl;
        
        _streams[packet.stream_index]->put(new Frame(_streams[packet.stream_index], &packet));
        
        av_free_packet(&packet);  // seems like the counterpart of av_init_packet()
    }
}


Stream*
AvStream::audioStream()
{
    return _pAudioStream;
}


Stream*
AvStream::videoStream()
{
    return _pVideoStream;
}


// void
// AvStream::stop()
// {
// }


Demuxer::Demuxer(AvStream* pAvStream) :
_pAvStream(pAvStream)
{
}


void
Demuxer::run()
{
    std::clog << "demuxing ..." << std::endl;
    _pAvStream->demux();
    std::clog << "demuxing finished." << std::endl;
}


Frame::Frame(const Frame& frame) :
_size(frame._size),
_pStream(frame._pStream)
{
    if (frame._pAvFrame) {
        uint8_t* buffer = (uint8_t*)av_malloc(_size);
        _pAvFrame = avcodec_alloc_frame();
        avpicture_fill((AVPicture*)_pAvFrame, buffer, _pStream->pixelFormat(), _pStream->width(), _pStream->height());
        
        for (int plane = 0; plane < 4; plane++) {
            memcpy(_pAvFrame->data[plane], frame._pAvFrame->data[plane], _pStream->height() * frame._pAvFrame->linesize[plane]);
        }
        _data = (char*)_pAvFrame->data[0];
    }
    else if (frame._pAvPacket) {
        // TODO: copy ctor for AVPacket
    }
    else {
        // TODO: only _data needs to be copied
    }
}


Frame::Frame(Stream* pStream) :
_data(0),
_size(0),
_pAvPacket(0),
_pAvFrame(0),
_pStream(pStream)
{
}


Frame::Frame(Stream* pStream, int dataSize) :
_data(0),
_size(0),
_pAvPacket(0),
_pAvFrame(0),
_pStream(pStream)
{
    _data = new char[dataSize];
    _size = dataSize;
}


Frame::Frame(Stream* pStream, char* data, int dataSize) :
_data(0),
_size(0),
_pAvPacket(0),
_pAvFrame(0),
_pStream(pStream)
{
    _data = new char[dataSize];
    _size = dataSize;
    memcpy(_data, data, dataSize);
}


Frame::Frame(Stream* pStream, AVPacket* pAvPacket) :
_data(0),
_size(0),
_pAvPacket(0),
_pAvFrame(0),
_pStream(pStream)
{
    _pAvPacket = new AVPacket;
    *_pAvPacket = *pAvPacket;
    _data = new char[pAvPacket->size];
    _size = pAvPacket->size;
    memcpy(_data, pAvPacket->data, pAvPacket->size);
    av_free(_pAvPacket);
    
//     std::clog << "new frame of size: " << _pAvPacket->size << std::endl;
}


Frame::Frame(Stream* pStream, AVFrame* pAvFrame) :
_data(0),
_size(0),
_pAvPacket(0),
_pAvFrame(pAvFrame),
_pStream(pStream)
{
    // NOTE: if we don't copy the frame data after decoding a frame,
    // it is deleted / overriden by libavcodec (for example: only the last three frames of 
    // the stream are stored)
    
    // NOTE: why these two lines are needed, dunno exactly ...
    _pAvFrame = new AVFrame;
    *_pAvFrame = *pAvFrame;
    
    _size = _pStream->pictureSize();
    _data = (char*)_pAvFrame->data[0];
}


Frame::~Frame()
{
    if (_data) {
        delete _data;
        _data = 0;
    }
    if (_pAvPacket) {
        delete _pAvPacket;
        _pAvPacket = 0;
    }
    if (_pAvFrame) {
        delete _pAvFrame;
        _pAvFrame = 0;
    }
}


char*
Frame::data()
{
    return _data;
}


int
Frame::size()
{
    return _size;
}


char*
Frame::planeData(int plane)
{
    return (char*)_pAvFrame->data[plane];
}


int
Frame::planeSize(int plane)
{
    return _pAvFrame->linesize[plane];
}


void
Frame::printInfo()
{
    std::clog << "frame info:" << std::endl;
    std::clog << " _pAvFrame->linesize[0..2]: " 
        << _pAvFrame->linesize[0] 
        << ", " << _pAvFrame->linesize[1] 
        << ", " << _pAvFrame->linesize[2] 
        << std::endl;
    
    std::clog << " _pAvFrame->data: " 
        << (int)_pAvFrame->data 
        << " _pAvFrame->base: " 
        << (int)_pAvFrame->base << std::endl;
    
    std::clog << " _pAvFrame->data[0..2]: " 
        << (unsigned int)_pAvFrame->data[0] 
        << ", " << (unsigned int)_pAvFrame->data[1] 
        << ", " << (unsigned int)_pAvFrame->data[2] 
        << std::endl;
}


Stream*
Frame::getStream()
{
    return _pStream;
}


Frame*
Frame::decode()
{
    if (_pStream->isAudio()) {
        return _pStream->decodeAudioFrame(this);
    }
    else if (_pStream->isVideo()) {
        return _pStream->decodeVideoFrame(this);
    }
}


Frame*
Frame::convert(PixelFormat targetFormat)
{
    _pStream->printInfo();
    int width = _pStream->width();
    int height = _pStream->height();
    PixelFormat inPixFormat = _pStream->_pAvCodecContext->pix_fmt;
    std::clog << "source pixelFormat: " << inPixFormat << " target pixelFormat: " << targetFormat << std::endl;
    
    int scaleAlgo = SWS_BICUBIC;
    struct SwsContext *pImgConvertContext = 0;
    
    pImgConvertContext = sws_getCachedContext(pImgConvertContext,
                                              width, height, inPixFormat,
                                              width, height, targetFormat,
                                              scaleAlgo, NULL, NULL, NULL);
    
    if (pImgConvertContext == 0) {
        std::cerr << "error: cannot initialize image conversion context" << std::endl;
        return 0;
    }
    else {
        std::clog << "image conversion context set up." << std::endl;
    }
    
    // FIXME: _pStream->pCodecContext is wrong with pOutFrame, because e.g. pix_fmt changed
    Frame* pOutFrame = _pStream->allocateVideoFrame(targetFormat);
    
    printInfo();
    std::clog << "sws_scale ..." << std::endl;
    int outSlizeHeight = sws_scale(pImgConvertContext,
                                   _pAvFrame->data, _pAvFrame->linesize,
                                   0, height,
                                   pOutFrame->_pAvFrame->data, pOutFrame->_pAvFrame->linesize);
    
    std::clog << "height of output slize: " << outSlizeHeight << std::endl;
    pOutFrame->printInfo();
    
    return pOutFrame;
}


void
Frame::writePpm(const std::string& fileName)
{
    std::clog << "write video frame to PPM file name: " << fileName << std::endl;
    
    Frame* pRes = convert(PIX_FMT_RGB24);
    
    std::ofstream ppmFile(fileName.c_str());
    // write PPM header
    ppmFile << "P6\n" << _pStream->width() << " " << _pStream->height() << "\n" << 255 << "\n";
    // write RGB pixel data
    ppmFile.write((const char*)pRes->_pAvFrame->data[0], _pStream->width() * _pStream->height() * 3);
}


void
Frame::write(Overlay* overlay)
{
    std::clog << "convert video frame to YUV ... " << std::endl;
    
    int width = _pStream->width();
    int height = _pStream->height();
    std::clog << "width: " <<  width << " height: " << height << std::endl;
    PixelFormat pixelFormat = _pStream->_pAvCodecContext->pix_fmt;
    std::clog << "source pixelFormat: " << pixelFormat << " dest pixelFormat: " << PIX_FMT_YUV420P << std::endl;
    
    int scaleAlgo = SWS_BICUBIC;
    struct SwsContext *pImgConvertContext = 0;
    
    pImgConvertContext = sws_getCachedContext(pImgConvertContext,
                                              width, height,
                                              pixelFormat,
                                              width, height,
                                              PIX_FMT_YUV420P, scaleAlgo, NULL, NULL, NULL);
    
    if (pImgConvertContext == 0) {
        std::cerr << "error: cannot initialize image conversion context" << std::endl;
        return;
    }
    else {
        std::clog << "image conversion context set up." << std::endl;
    }
    
    std::clog << "sws_scale ..." << std::endl;
    int outSlizeHeight = sws_scale(pImgConvertContext, _pAvFrame->data, _pAvFrame->linesize,
                                   0, height, overlay->_data, overlay->_pitch);
}


void
Stream::open()
{
    std::clog << "opening stream ..." << std::endl;
    _pAvCodecContext = _pAvStream->codec;
    
    //////////// find decoders for audio and video stream ////////////
    
    std::clog << "searching codec with codec_id: " << _pAvStream->codec->codec_id << std::endl;
    _pAvCodec = avcodec_find_decoder(_pAvStream->codec->codec_id);
    
    if(_pAvCodec == 0) {
        std::cerr << "error: could not find decoder" << std::endl;
        return;
    }
    
    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
//         if(_pVideoCodec->capabilities & CODEC_CAP_TRUNCATED) {
//             _pMeta->_pVideoCodecContext->flags |= CODEC_FLAG_TRUNCATED;
//         }
    
    if(avcodec_open(_pAvStream->codec, _pAvCodec) < 0) {
        std::cerr << "error: could not open decoder" << std::endl;
        return;
    }
    std::clog << "found codec: " << _pAvCodec->name << " (" << _pAvCodec->long_name << ")" << std::endl;
    std::clog << "start_time: " << _pAvStream->start_time << std::endl;
    std::clog << "duration: " << _pAvStream->duration << std::endl;
    std::clog << "number of frames (nb_frames): " << _pAvStream->nb_frames << std::endl;
    
//     if(_pVideoCodec->frame_rate > 1000 && _pVideoCodec->frame_rate_base == 1) {
//         _pVideoCodec->frame_rate_base = 1000;
//     }
}


void
Stream::close()
{
    std::clog << "closing stream." << std::endl;
}


void
Stream::run()
{
    std::clog << "streaming ";
    if (isAudio()) {
        std::clog << "audio ...";
    }
    else if (isVideo()) {
        std::clog << "video ...";
    }
    else {
        std::clog << "nothing.";
        return;
    }
    std::clog << std::endl;
    
    int frameCount = 0;
    int maxWaitDemux = 5;
    int waitDemux = maxWaitDemux;
    Frame* pFrame;
    while (true) {
        if (pFrame = get()) {
            waitDemux = maxWaitDemux;
            std::clog << "decode frame #" << Poco::NumberFormatter::format0(++frameCount, 3) << " ";
            Frame* pFrameDecoded = pFrame->decode();
            if (pFrameDecoded) {
                _pSink->writeFrame(pFrameDecoded);
            }
        }
        else {
            Poco::Thread::sleep(10);
            if (--waitDemux <= 0) {
                std::clog << "stream stopped waiting for packages from demuxer." << std::endl;
                break;
            }
        }
    }
    std::clog << "stream finished." << std::endl;
}


void
Stream::put(Frame* pFrame)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_packetQueueLock);
    
    pFrame->_pStream = this;
    _packetQueue.push(pFrame);
}


Frame*
Stream::get()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_packetQueueLock);
    
    Frame* ret;
    
    if (_packetQueue.empty()) {
        return 0;
    }
    
    ret = _packetQueue.front();
    _packetQueue.pop();
    return ret;
}


Frame*
Stream::allocateVideoFrame(PixelFormat targetFormat)
{
    // Determine required buffer size and allocate buffer
    int numBytes = avpicture_get_size(targetFormat, width(), height());
    uint8_t* buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    AVFrame* pOutFrame = avcodec_alloc_frame();
    avpicture_fill((AVPicture *)pOutFrame, buffer, targetFormat, width(), height());
    
    return new Frame(this, pOutFrame);
}


bool
Stream::isAudio()
{
    return _pAvStream->codec->codec_type == CODEC_TYPE_AUDIO;
}


bool
Stream::isVideo()
{
    return _pAvStream->codec->codec_type == CODEC_TYPE_VIDEO;
}


void
Stream::printInfo()
{
    std::clog << "stream info:" << std::endl;
    std::clog << "width: " <<  width() << " height: " << height() << std::endl;
}


int
Stream::width()
{
    if (_pAvCodecContext) {
        return _pAvCodecContext->width;
    }
    else {
        return 0;
    }
}


int
Stream::height()
{
    if (_pAvCodecContext) {
        return _pAvCodecContext->height;
    }
    else {
        return 0;
    }
}


PixelFormat
Stream::pixelFormat()
{
    return _pAvCodecContext->pix_fmt;
}


int
Stream::pictureSize()
{
    return avpicture_get_size(pixelFormat(), width(), height());
}


Frame*
Stream::decodeAudioFrame(Frame* pFrame)
{
    if (!pFrame) {
        return 0;
    }
    const int outBufferSize = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2;
    Frame* pOutFrame = new Frame(this, outBufferSize);
    pOutFrame->_data[outBufferSize-1] = 0;
//     pOutFrame->_pStream = this;
    
    uint8_t* inBuffer = (uint8_t*) pFrame->data();
    int inBufferSize = pFrame->size();
    while(inBufferSize > 0) {
        int bytesConsumed = avcodec_decode_audio2(_pAvStream->codec,
                                                  (int16_t*)pOutFrame->_data, &pOutFrame->_size, 
                                                  inBuffer, inBufferSize);
        
        std::clog << "inBufferSize: " << inBufferSize << " bytesConsumed: " << bytesConsumed << " decoded size: " << pOutFrame->_size << std::endl;
        if (pOutFrame->_size == 0) {
            std::cerr << "error: no frame could be decompressed" << std::endl;
            pOutFrame->_size = 0;
        }
        if (bytesConsumed < 0) {
            std::cerr << "error: skipped frame" << std::endl;
            pOutFrame->_size = 0;
        }
        inBuffer += bytesConsumed;
        inBufferSize -= bytesConsumed;
    }
    return pOutFrame;
}


Frame*
Stream::decodeVideoFrame(Frame* pFrame)
{
    if (!pFrame) {
        return 0;
    }
    
    Frame* pOutFrame;
    AVFrame outPic;
    // TODO: FF_INPUT_BUFFER_PADDING_SIZE
    int decodeSuccess;
//     std::clog << "size: " << pFrame->size() << std::endl;
    
    int bytesConsumed = avcodec_decode_video(_pAvStream->codec,
                                             &outPic, &decodeSuccess,
                                            (const uint8_t*)pFrame->data(), pFrame->size());
    
    std::clog << "bytesConsumed: " << bytesConsumed << " decode success: " << decodeSuccess /*<< std::endl*/;
    std::clog << "outPic.linesize[0..2]: " << outPic.linesize[0] << ", " << outPic.linesize[1] << ", " << outPic.linesize[2] << std::endl;
    
    if (bytesConsumed <= 0) {
        std::cerr << ">>>>>>> skipping frame while decoding" << std::endl;
        return 0;
    }
    if (decodeSuccess <= 0) {
        std::cerr << ">>>>>>> skipping frame while decoding" << std::endl;
        return 0;
    }
    
    pOutFrame = new Frame(this, &outPic);
//     pOutFrame->_pStream = this;
    
    return pOutFrame;
}


void
Stream::attachSink(Sink* pSink)
{
    _pSink = pSink;
    pSink->_pStream = this;
}


Meta::Meta() :
_pFormatContext(0),
_pIoContext(0),
_pInputFormat(0)
{
}


int
Stream::sampleWidth()
{
    std::clog << "stream sample_fmt: " << (int)_pAvStream->codec->sample_fmt << std::endl;
    // avcodec.h says sample_fmt is not used. I guess it means it is always S16
    switch(_pAvStream->codec->sample_fmt) {
    case SAMPLE_FMT_U8:
        return 8; // beware unsigned!
        break;
    case SAMPLE_FMT_S16:
        return 16;
        break;
    case SAMPLE_FMT_S32:
        return 32;
        break;
    case SAMPLE_FMT_FLT:
        return -32;
        break;
    default:
        return 0;
    }
}


unsigned int
Stream::sampleRate()
{
    std::clog << "stream sample_rate: " << (int)_pAvStream->codec->sample_rate << std::endl;
    
    return _pAvStream->codec->sample_rate;
}


int
Stream::channels()
{
    std::clog << "stream channels: " << (int)_pAvStream->codec->channels << std::endl;
    
    return _pAvStream->codec->channels;
}


Meta::~Meta()
{
//     if (_pAudioCodecContext) {
//         avcodec_close(_pAudioCodecContext);
//     }
//     if (_pVideoCodecContext) {
//         avcodec_close(_pVideoCodecContext);
//     }
    if (_pFormatContext) {
//         std::clog << "Meta::_pFormatContext: " << _pFormatContext << std::endl;
        av_close_input_stream(_pFormatContext);
    }
}


// AVCodecContext*
// Meta::getAudioCodecContext()
// {
//     return _pAudioCodecContext;
// }


void
Meta::print()
{
    dump_format(_pFormatContext, 0, _pFormatContext->filename, 0);
    
    AVMetadataTag* tag = 0;
    while ((tag = av_metadata_get(_pFormatContext->metadata, "", tag, AV_METADATA_IGNORE_SUFFIX))) {
        std::clog << tag->key << ", " << tag->value << std::endl;
    }
}

// FIXME: for correct buffer sizes see libavformat/utils.c: av_open_input_file()
Tagger::Tagger() :
_tagBufferSize(2048),
// _IoBufferSize(32768),
// _IoBufferSize(2048),
// _IoBufferSize(1024),
_IoBufferSize(8192),
_pIoBuffer(new unsigned char[_IoBufferSize])
{

}


Tagger::~Tagger()
{
    if (_pIoBuffer) {
        delete _pIoBuffer;
    }
}


AVInputFormat*
Tagger::probeInputFormat(std::istream& istr)
{
    AVInputFormat* pInputFormat = 0;
    
    AVProbeData probeData;
    probeData.filename = "";
    std::clog << "probing stream ..." << std::endl;
    probeData.buf_size = _tagBufferSize;
    unsigned char buffer[_tagBufferSize];
    probeData.buf = (unsigned char*)&buffer;
    istr.read((char*)probeData.buf, _tagBufferSize);
    if(istr.bad()) {
        std::cerr << "error reading probe data" << std::endl;
        return 0;
    }
//     istr.seekg(0);
    std::clog << "done." << std::endl;
    
    std::clog << "detecting format ..." << std::endl;
    // initialize _pInputFormat
    pInputFormat = av_probe_input_format(&probeData, 1 /*int is_opened*/);
    if (pInputFormat) {
        std::clog << "detected format: " << pInputFormat->name << " (" << pInputFormat->long_name << ")" << std::endl;
    }
    else {
        std::cerr << "container format unknown" << std::endl;
        return 0;
    }
    
    // TODO: reset the stream after probing
//     // Seek back to 0
//     // copied from url_fseek
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
    
//     pInputFormat->flags |= AVFMT_NOFILE;
    
    return pInputFormat;
}


static long totalRead = 0;
static long totalReadCount = 0;

static int IORead( void *opaque, uint8_t *buf, int buf_size )
{
    std::clog << "IORead()" << std::endl;
    
    URLContext* pUrlContext = (URLContext*)opaque;
    std::clog << "IORead() pUrlContext pointer: " << pUrlContext << std::endl;
    std::clog << "IORead() URLProtocol name: " << pUrlContext->prot->name << std::endl;
    
    std::istream* pInputStream = (std::istream*)pUrlContext->priv_data;
    std::clog << "IORead() pInputStream pointer: " << pInputStream << std::endl;
    if (!pInputStream) {
        std::cerr << "IORead failed, std::istream not set" << std::endl;
        return -1;
    }
    
    int bytes = pInputStream->readsome((char*)buf, buf_size);
    if (!pInputStream->good()) {
        std::cerr << "IORead failed to read from std::istream" << std::endl;
        return -1;
    }
    
    totalRead += bytes;
    totalReadCount++;
    std::clog << "IORead() bytes read: " << bytes << " total: " << totalRead << " read ops: " << totalReadCount << std::endl;
    return bytes;
    
    std::clog << "IORead() finished" << std::endl;
}


static int64_t IOSeek( void *opaque, int64_t offset, int whence )
{
    std::clog << "IOSeek() offset: " << offset << std::endl;
    
    URLContext* pUrlContext = (URLContext*)opaque;
    std::clog << "IOSeek() pUrlContext pointer: " << pUrlContext << std::endl;
    std::clog << "IOSeek() URLProtocol name: " << pUrlContext->prot->name << std::endl;
    
    std::istream* pInputStream = (std::istream*)pUrlContext->priv_data;
    std::clog << "IOSeek() pInputStream pointer: " << pInputStream << std::endl;
    if (!pInputStream) {
        std::cerr << "IOSeek failed, std::istream not set" << std::endl;
        return 0;
    }
    
    pInputStream->seekg(offset);
    if (!pInputStream->good()) {
        std::cerr << "IOSeek failed to seek std::istream" << std::endl;
        return 0;
    }
    totalRead = offset;
    
    std::clog << "IOSeek() finished" << std::endl;
}


ByteIOContext*
Tagger::initIo(std::istream& istr)
{
    URLContext* pUrlContext = new URLContext;
    pUrlContext->is_streamed = 1;
    pUrlContext->priv_data = 0;
    pUrlContext->filename = "std::istream";
    pUrlContext->prot = new URLProtocol;
    pUrlContext->prot->name = "OMM avio wrapper for std::iostream";
    pUrlContext->prot->next = 0;
    pUrlContext->prot->url_open = 0;
    pUrlContext->prot->url_read = (int (*) (URLContext *, unsigned char *, int))IORead;
    pUrlContext->prot->url_write = 0;
    pUrlContext->prot->url_seek = (int64_t (*) (URLContext *, int64_t, int))IOSeek;
    pUrlContext->prot->url_close = 0;
    pUrlContext->priv_data = &istr;
    
    // TODO: maybe IOSeek = 0 stops libavformat from doing seek operations
    ByteIOContext* pIoContext = av_alloc_put_byte(_pIoBuffer, _IoBufferSize, 0, pUrlContext, IORead, 0, IOSeek);
    pIoContext->is_streamed = 1;
    pIoContext->max_packet_size = _IoBufferSize;
    
    return pIoContext;
}


Meta*
Tagger::tag(std::istream& istr)
{
//     std::clog << "Tagger::tag()" << std::endl;
//     std::clog << "sizeof(AVFormatContext): " << sizeof(AVFormatContext) << std::endl;  // should be 3960, see -malign-double
//     std::clog << "sizeof(AVCodecContext): " << sizeof(AVCodecContext) << std::endl;
    
    
    Meta* pMeta = new Meta;
    int error;
    
//     pMeta->_pIoContext = initIo(istr);
//     pMeta->_pInputFormat = probeInputFormat(istr);
    
    error = av_open_input_file(&pMeta->_pFormatContext, "/home/jb/tmp/omm/o1$r1", 0, 0, 0);
//     error = av_open_input_file(&pMeta->_pFormatContext, "/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3", 0, 0, 0);
//     error = av_open_input_stream(&pMeta->_pFormatContext, pMeta->_pIoContext, "std::istream", pMeta->_pInputFormat, 0);
    if (error) {
        std::cerr << "av_open_input_stream() failed" << std::endl;
        return 0;
    }
    
    error = av_find_stream_info(pMeta->_pFormatContext);
    if (error) {
        std::cerr << "av_find_stream_info() failed, could not find codec parameters" << std::endl;
        return 0;
    }
    
//     pMeta->_pFormatContext->flags |= AVFMT_FLAG_NONBLOCK;
    
//     if (pMeta->_pFormatContext->flags & AVFMT_FLAG_NONBLOCK) {
//         std::clog << "frame reading is set to NONBLOCK" << std::endl;
//     }
//     else {
//         std::clog << "frame reading is set to BLOCK" << std::endl;
//     }
    
    std::clog << "Tagger::tag() finished" << std::endl;
    
    return pMeta;
}


Sink*
Sink::loadPlugin(const std::string& libraryPath, const std::string& className)
{
    Poco::ClassLoader<Sink> sinkPluginLoader;
    if (sinkPluginLoader.isLibraryLoaded(libraryPath)) {
        std::cerr << "error: library " << libraryPath << " already loaded" << std::endl;
        return 0;
    }
    try {
        sinkPluginLoader.loadLibrary(libraryPath);
    }
    catch (Poco::NotFoundException) {
        std::cerr << "error: could not find " << libraryPath << " sink plugin." << std::endl;
        return 0;
    }
    catch (Poco::LibraryLoadException) {
        std::cerr << "error: could not load " << libraryPath << " sink plugin." << std::endl;
        return 0;
    }
    std::clog << "sink plugin successfully loaded." << std::endl;
    
    Omm::Av::Sink* res;
    try {
        res = sinkPluginLoader.create(className);
    }
    catch (Poco::NotFoundException) {
        std::cerr << "error: could not create instance of sink plugin." << std::endl;
        return 0;
    }
    return res;
}
