/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2013                                                       |
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

#include <queue>

#include <Poco/ClassLibrary.h>
#include <Poco/File.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/BufferedStreamBuf.h>
#include <Poco/Mutex.h>
#include <Poco/Condition.h>

#include <Omm/UpnpAvObject.h>
#include <Omm/AvStream.h>
#include <Omm/Util.h>

#include "iPhoneCamera.h"

#import <AVFoundation/AVCaptureSession.h>
#import <AVFoundation/AVCaptureDevice.h>
#import <AVFoundation/AVCaptureInput.h>
#import <AVFoundation/AVCaptureOutput.h>
#import <AVFoundation/AVCaptureSession.h>
#import <AVFoundation/AVMediaFormat.h>
#import <AVFoundation/AVAsset.h>
#import <AVFoundation/AVAssetReader.h>
#import <AVFoundation/AVAssetReaderOutput.h>


@interface OmmAvCaptureVideoOutput : AVCaptureVideoDataOutput<AVCaptureVideoDataOutputSampleBufferDelegate>
{
//    AVFoundationEngine*        _pEngine;
//    AVPlayer*                  _pAvPlayer;
    dispatch_queue_t                _queue;
    std::queue<CMSampleBufferRef>   _sampleQueue;
    Poco::FastMutex                 _queueLock;
}
@end


@implementation OmmAvCaptureVideoOutput

- (id)init
{
    self = [super init];
    if (self) {
        _queue = dispatch_queue_create("OmmAvCaptureVideoOutputQueue", NULL);
        [self setSampleBufferDelegate:self queue:_queue];

        self.videoSettings = [NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA]
                                           forKey:(id)kCVPixelBufferPixelFormatTypeKey];
        self.minFrameDuration = CMTimeMake(1, 15);
    }
    return self;
}


- (void)captureOutput:(AVCaptureOutput*)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection*)connection
{
    CFRetain(sampleBuffer);

    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel captured sample of size: " + Poco::NumberFormatter::format(CMSampleBufferGetTotalSampleSize(sampleBuffer)));
    Poco::ScopedLock<Poco::FastMutex> lock(_queueLock);
    _sampleQueue.push(sampleBuffer);
}


- (CMSampleBufferRef)get
{
    Poco::ScopedLock<Poco::FastMutex> lock(_queueLock);
    CMSampleBufferRef res = _sampleQueue.front();
    _sampleQueue.pop();
    return res;
}


- (int)getSize
{
    Poco::ScopedLock<Poco::FastMutex> lock(_queueLock);
    return _sampleQueue.size();
}

@end


class AVCaptureSessionStreamBuf : public Poco::BufferedStreamBuf
{
public:
    AVCaptureSessionStreamBuf(OmmAvCaptureVideoOutput* pCaptureOutput, int bufSize) : Poco::BufferedStreamBuf(bufSize, std::ios_base::in), _pCaptureOutput(pCaptureOutput), _bufSize(bufSize), _offset(0)
    {
//        if ([_pCaptureOutput getSize]) {
//            _pSampleBuf = [_pCaptureOutput get];
//            _pBlockBuf = CMSampleBufferGetDataBuffer(_pSampleBuf);
//        }
    }

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel AVCaptureOutput stream read from device, num bytes: " + Poco::NumberFormatter::format(length));
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel AVCaptureOutput sample queue size: " + Poco::NumberFormatter::format([_pCaptureOutput getSize]));
        int res = 0;
        int bytesAvailable = 0;
        if (!_offset && [_pCaptureOutput getSize]) {
//            CFRelease(_pSampleBuf);
            _pSampleBuf = [_pCaptureOutput get];
            _pBlockBuf = CMSampleBufferGetDataBuffer(_pSampleBuf);
            bytesAvailable = CMBlockBufferGetDataLength(_pBlockBuf);
        }
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel AVCaptureOutput bytes available: " + Poco::NumberFormatter::format(bytesAvailable));
        if (!bytesAvailable) {
            LOGNS(Omm::Av, upnpav, error, "IPhoneCameraModel AVCaptureOutput no bytes available");
            return 0;
        }
        int bytesLeft = bytesAvailable - _offset;
        if (length <= bytesLeft) {
            CMBlockBufferCopyDataBytes(_pBlockBuf, _offset, length, buffer);
            res = length;
            _offset += length;
        }
        else if (bytesAvailable && [_pCaptureOutput getSize]) {
            CMBlockBufferCopyDataBytes(_pBlockBuf, _offset, bytesLeft, buffer);
            res = bytesLeft;
            CFRelease(_pSampleBuf);
            _pSampleBuf = [_pCaptureOutput get];
            _pBlockBuf = CMSampleBufferGetDataBuffer(_pSampleBuf);
            _offset = 0;
        }
        return res;
    }

private:
    OmmAvCaptureVideoOutput*    _pCaptureOutput;
    CMBlockBufferRef            _pBlockBuf;
    CMSampleBufferRef           _pSampleBuf;
    const int                   _bufSize;
    std::size_t                 _offset;
};


class AVCaptureSessionIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    AVCaptureSessionIStream(OmmAvCaptureVideoOutput* pCaptureOutput, int bufSize = 4096) : _streamBuf(pCaptureOutput, bufSize), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    AVCaptureSessionStreamBuf   _streamBuf;
};


@interface OmmAvCaptureAudioOutput : AVCaptureAudioDataOutput<AVCaptureAudioDataOutputSampleBufferDelegate>
{
//    AVFoundationEngine*        _pEngine;
//    AVPlayer*                  _pAvPlayer;
    dispatch_queue_t                _queue;
    std::queue<CMSampleBufferRef>   _sampleQueue;
    Poco::FastMutex                 _queueLock;
    Poco::Condition                 _queueReadCondition;
}
@end


@implementation OmmAvCaptureAudioOutput

- (id)init
{
    self = [super init];
    if (self) {
        _queue = dispatch_queue_create("OmmAvCaptureAudioOutputQueue", NULL);
        [self setSampleBufferDelegate:self queue:_queue];
    }
    return self;
}


- (void)captureOutput:(AVCaptureOutput*)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection*)connection
{
    CFRetain(sampleBuffer);

    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel captured sample of size: " + Poco::NumberFormatter::format(CMSampleBufferGetTotalSampleSize(sampleBuffer)));
    Poco::ScopedLock<Poco::FastMutex> lock(_queueLock);
    _sampleQueue.push(sampleBuffer);
    _queueReadCondition.broadcast();

    // check what sample format we have
    // this should always be linear PCM, 16 bit samples
    // but may have 1 or 2 channels
    CMAudioFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
    const AudioStreamBasicDescription* desc = CMAudioFormatDescriptionGetStreamBasicDescription(format);
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel format id: " + std::string((char*)(&desc->mFormatID), 4));
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel sample rate: " + Poco::NumberFormatter::format(desc->mSampleRate));
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel channels: " + Poco::NumberFormatter::format(desc->mChannelsPerFrame));
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel bits: " + Poco::NumberFormatter::format(desc->mBitsPerChannel));
//    assert(desc->mFormatID == kAudioFormatLinearPCM);
//    if (desc->mChannelsPerFrame == 1 && desc->mBitsPerChannel == 16) {
//        float *convertedSamples = malloc(numSamples * sizeof(float));
//        vDSP_vflt16((short *)samples, 1, convertedSamples, 1, numSamples);
//    } else {
//        // handle other cases as required
//    }
}


- (CMSampleBufferRef)get
{
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel get sample from queue with size: " + Poco::NumberFormatter::format(_sampleQueue.size()));
    Poco::ScopedLock<Poco::FastMutex> lock(_queueLock);
    if (_sampleQueue.size() == 0) {
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel wait for sample ...");
        _queueReadCondition.wait<Poco::FastMutex>(_queueLock);
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel wait for sample finished.");
    }
    CMSampleBufferRef res = _sampleQueue.front();
    _sampleQueue.pop();
    return res;
}


- (int)getSize
{
    Poco::ScopedLock<Poco::FastMutex> lock(_queueLock);
    return _sampleQueue.size();
}

@end


class AVCaptureSessionAudioStreamBuf : public Poco::BufferedStreamBuf
{
public:
    AVCaptureSessionAudioStreamBuf(OmmAvCaptureAudioOutput* pCaptureOutput, int bufSize) : Poco::BufferedStreamBuf(bufSize, std::ios_base::in), _pCaptureOutput(pCaptureOutput), _pSampleBuf(0), _bufSize(bufSize), _offset(0)
    {
//        if ([_pCaptureOutput getSize]) {
//            _pSampleBuf = [_pCaptureOutput get];
//            _pBlockBuf = CMSampleBufferGetDataBuffer(_pSampleBuf);
//        }
    }

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel AVCaptureOutput read from device bytes: " + Poco::NumberFormatter::format(length));
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel AVCaptureOutput sample queue size: " + Poco::NumberFormatter::format([_pCaptureOutput getSize]));
        int res = 0;

        if (![_pCaptureOutput getSize] || !_pSampleBuf) {
            _pSampleBuf = [_pCaptureOutput get];
        }
//        if (!_pSampleBuf) {
//            _pSampleBuf = [_pCaptureOutput get];
//        }
        _pBlockBuf = CMSampleBufferGetDataBuffer(_pSampleBuf);
        int bytesAvailable = CMBlockBufferGetDataLength(_pBlockBuf);
        int bytesLeft = bytesAvailable - _offset;
        if (length <= bytesLeft) {
            CMBlockBufferCopyDataBytes(_pBlockBuf, _offset, length, buffer);
            res = length;
            _offset += length;
        }
        else {
            CMBlockBufferCopyDataBytes(_pBlockBuf, _offset, bytesLeft, buffer);
            res = bytesLeft;
            CFRelease(_pSampleBuf);
            _pSampleBuf = [_pCaptureOutput get];
            _pBlockBuf = CMSampleBufferGetDataBuffer(_pSampleBuf);
            _offset = 0;
        }
        return res;
    }

private:
    OmmAvCaptureAudioOutput*    _pCaptureOutput;
    CMBlockBufferRef            _pBlockBuf;
    CMSampleBufferRef           _pSampleBuf;
    const int                   _bufSize;
    std::size_t                 _offset;
};


class AVCaptureSessionAudioIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    AVCaptureSessionAudioIStream(OmmAvCaptureAudioOutput* pCaptureOutput, int bufSize = 2048) : _streamBuf(pCaptureOutput, bufSize), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    AVCaptureSessionAudioStreamBuf   _streamBuf;
};


IPhoneCameraModel::IPhoneCameraModel()
{
}


IPhoneCameraModel::~IPhoneCameraModel()
{
}


std::string
IPhoneCameraModel::getModelClass()
{
    return "IPhoneCameraModel";
}


void
IPhoneCameraModel::scan()
{
    addPath("0");
}


Omm::Av::ServerObject*
IPhoneCameraModel::getMediaObject(const std::string& path)
{
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel get media object with path: " + path);

    Omm::Av::ServerItem* pItem = getServerContainer()->createMediaItem();
    pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_ITEM));
    pItem->setTitle("iPhone Camera");

    Omm::Av::ServerObjectResource* pResource = pItem->createResource();
//    pResource->setSize(getSize(path));
    pResource->setProtInfo("http-get:*:" + Omm::Av::Mime::VIDEO_QUICKTIME + ":*");
    pItem->addResource(pResource);
//        // TODO: add icon property
////        pItem->_icon = pItem->_path;

    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel get media object finished.");

    return pItem;
}


std::streamsize
IPhoneCameraModel::getSize(const std::string& path)
{
    return 0;
}


bool
IPhoneCameraModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return false;
}


std::istream*
IPhoneCameraModel::getStream(const std::string& path, const std::string& resourcePath)
{
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel get stream of media object with path: " + path);

    std::istream* pRes = 0;
    id savedException = nil;
    @try {
        _pCaptureSession = [[AVCaptureSession alloc] init];
//        [_pCaptureSession setSessionPreset:AVCaptureSessionPresetLow];

        NSArray *devices = [AVCaptureDevice devices];
        AVCaptureDevice *frontCamera;
        AVCaptureDevice *backCamera;
        for (AVCaptureDevice *device in devices) {
            LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel device name: " + std::string([[device localizedName] UTF8String]));
            if ([device hasMediaType:AVMediaTypeVideo]) {
                if ([device position] == AVCaptureDevicePositionBack) {
                    LOGNS(Omm::Av, upnpav, debug, "Device position : back");
                    backCamera = device;
                }
                else {
                    LOGNS(Omm::Av, upnpav, debug, "Device position : front");
                    frontCamera = device;
                }
            }
        }

        // need to convert audio/video format from raw data to ...
        //http://stackoverflow.com/questions/10817036/can-i-use-avcapturesession-to-encode-an-aac-stream-to-memory

//        AVCaptureDevice* videoCaptureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
//        AVCaptureDevice* videoCaptureDevice = backCamera;
        AVCaptureDevice* videoCaptureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
        NSError* error = nil;
        AVCaptureDeviceInput* videoInput = [AVCaptureDeviceInput deviceInputWithDevice:videoCaptureDevice error:&error];
        if (videoInput) {
            [_pCaptureSession addInput:videoInput];
        }
        else {
            LOGNS(Omm::Av, upnpav, error, "IPhoneCameraModel failed to get camera video input: " + std::string([[error localizedDescription] UTF8String]));
        }
//        OmmAvCaptureVideoOutput* videoOutput = [[OmmAvCaptureVideoOutput alloc] init];
        OmmAvCaptureAudioOutput* videoOutput = [[OmmAvCaptureAudioOutput alloc] init];

        [_pCaptureSession addOutput:videoOutput];
        [_pCaptureSession startRunning];
        LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel start recording ...");
        Poco::Thread::sleep(1000);
//        pRes = new AVCaptureSessionIStream(videoOutput);
        pRes = new AVCaptureSessionAudioIStream(videoOutput);
    }
    @catch (NSException* exception) {
        LOGNS(Omm::Av, upnpav, error, "IPhoneCameraModel failed to get stream: " + std::string([[exception reason] UTF8String]));
        savedException = [exception retain];
        @throw;
    }
    @finally {
        [savedException autorelease];
    }

    return pRes;
}


void
IPhoneCameraModel::freeStream(std::istream* pIstream)
{
    LOGNS(Omm::Av, upnpav, debug, "deleting file stream");

    [_pCaptureSession stopRunning];
    LOGNS(Omm::Av, upnpav, debug, "IPhoneCameraModel stopped recording ...");

    // TODO: also release memory pool of stream (where AVCaptureOutput is allocated)
    delete pIstream;
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(IPhoneCameraModel)
POCO_END_MANIFEST
#endif
