/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/StreamCopier.h>
#include <Poco/NumberFormatter.h>

#include "EngineAVFoundation.h"

#import <AVFoundation/AVPlayer.h>


@interface OmmAvPlayer : NSObject
{
    AVFoundationEngine*        _pEngine;
    AVPlayer*                  _pAvPlayer;
}
@end


@implementation OmmAvPlayer

//- (void)playbackFinished:(NSNotification*)notification
//{
//}


- (id)initWithEngine:(AVFoundationEngine*)pEngine
{
    self = [super init];
    if (self) {
        _pEngine = pEngine;
        _pAvPlayer = 0;
    }
    return self;
}


- (AVFoundationEngine::TransportState)getTransportState
{
    if (_pAvPlayer) {
        return AVFoundationEngine::Playing;
    }
    else {
        return AVFoundationEngine::Stopped;
    }
}

//- (void)playbackStateChanged:(NSNotification*)notification
//{
//    LOGNS(Omm::Av, upnpav, debug, "ENGINE sending notification playback state did change");
//    _pEngine->transportStateChangedNotification();
//}
//
//
//- (void)volumeChanged:(NSNotification *)notification
//{
//    float volume = [[[notification userInfo]
//                        objectForKey:@"AVSystemController_AudioVolumeNotificationParameter"]
//                        floatValue];
//    _pEngine->volumeChangedNotification(volume);
//}
//
//
- (id)playUrl:(NSURL*)contentURL
{
    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine play url ...");

    if (!_pAvPlayer) {
        _pAvPlayer = [AVPlayer playerWithURL:contentURL];
        [_pAvPlayer play];
    }

    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine play url finished.");

//    if (self = [super initWithContentURL:contentURL]) {
//        self.controlStyle = MPMovieControlStyleNone;
//        [[NSNotificationCenter defaultCenter]
//            addObserver:self
//            selector:@selector(playbackStateChanged:)
//            name:MPMoviePlayerPlaybackStateDidChangeNotification
//            object:self];
//        [[NSNotificationCenter defaultCenter]
//            addObserver:self
//            selector:@selector(volumeChanged:)
//            name:@"AVSystemController_SystemVolumeDidChangeNotification"
//            object:nil];
//    }
//    return self;
}


- (id)stop
{
    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine stop ...");

    if (_pAvPlayer) {
        [_pAvPlayer stop];
        [_pAvPlayer release];
        _pAvPlayer = 0;
    }

    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine stop finished.");
}

@end


AVFoundationEngine::AVFoundationEngine() :
_pPlayer(0),
_imageLength(0)
{
    _engineId = "AV Foundation engine " + Omm::OMM_VERSION;
//    _lastImageView = nil;
}


AVFoundationEngine::~AVFoundationEngine()
{
}


void
AVFoundationEngine::createPlayer()
{
    _pPlayer = [[OmmAvPlayer alloc] initWithEngine:this];

//    _imageBackgroundView = [[UIView alloc] init];
//    _imageBackgroundView.backgroundColor = [UIColor blackColor];
}


void
AVFoundationEngine::setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo)
{
//    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine set uri: " + uri);

    _urlString = uri;
    _protInfo = protInfo;
    _mime = Omm::Av::Mime(protInfo.getMimeString());
}


void
AVFoundationEngine::setFullscreen(bool on)
{
}


void
AVFoundationEngine::play()
{
//    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine play ...");

    // NOTE: this is called from another thread, so we need a new memory pool.
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSURL* url = [NSURL URLWithString:[[NSString alloc] initWithUTF8String:_urlString.c_str()]];

//    OmmAvPlayer* pPlayer = static_cast<OmmAvPlayer*>(_pPlayer);
//    [pPlayer performSelectorOnMainThread:@selector(playUrl:) withObject:url waitUntilDone:YES];
//    [_pPlayer performSelectorOnMainThread:@selector(playUrl:) withObject:url waitUntilDone:YES];
//    [_pPlayer performSelectorOnMainThread:@selector(playUrl:) withObject:url waitUntilDone:NO];
    [_pPlayer playUrl:url];

//    if (_mime.isImage()) {
////        NSLog(@"showing image URL: %@", url);
////        downloadImage();
////        showImage();
//    }
////    else if (_mime.isAudio()) {
////
////    }
//    else {
//       _startTime = 0;
//       _length = 0.0;
//
//       LOGNS(Omm::Av, upnpav, debug, "ENGINE alloc media player ...");
////       AVPlayer* pPlayer = [AVPlayer alloc];
////       _player = pPlayer;
////       [pPlayer initWithURL:url];
////       [pPlayer play];
//
////       [pPlayer setEngine:this];
////
////       LOGNS(Omm::Av, upnpav, debug, "ENGINE init media player ...");
////       [pPlayer performSelectorOnMainThread:@selector(initWithContentURL:) withObject:url waitUntilDone:YES];
////
////       if (pPlayer) {
////           LOGNS(Omm::Av, upnpav, debug, "ENGINE playing URL: " + _urlString);
////           [pPlayer performSelectorOnMainThread:@selector(play) withObject:nil waitUntilDone:YES];
////           LOGNS(Omm::Av, upnpav, debug, "ENGINE playing started.");
////       }
////       if (_mime.isVideo()) {
////           LOGNS(Omm::Av, upnpav, debug, "ENGINE adding media player view ...");
////////           [_parentView performSelectorOnMainThread:@selector(addSubview:) withObject:_player.view waitUntilDone:YES];
//////           [static_cast<UIView*>(_pVisual->getWindow()) performSelectorOnMainThread:@selector(addSubview:) withObject:pPlayer.view waitUntilDone:YES];
//////           pPlayer.view.frame = static_cast<UIView*>(_pVisual->getWindow()).frame;
//////           LOGNS(Omm::Av, upnpav, debug, "ENGINE adding media player view finished.");
////       }
//    }

   [pool release];

   LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine play finished.");
}


void
AVFoundationEngine::setSpeed(int nom, int denom)
{
}


void
AVFoundationEngine::pause()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

    if (!_pPlayer) {
        return;
    }

    // NOTE: this is called from another thread, so we need a new memory pool.
//    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
//    MediaPlayerController* pPlayer = static_cast<MediaPlayerController*>(_player);
//
//    if (_mime.isImage()) {
//
//    }
//    else {
//        [pPlayer performSelectorOnMainThread:@selector(pause) withObject:nil waitUntilDone:YES];
////        [pPlayer.moviePlayer performSelectorOnMainThread:@selector(pause) withObject:nil waitUntilDone:YES];
//    }
//
//    [pool release];
}


void
AVFoundationEngine::stop()
{
//    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

//    if (!_pPlayer) {
//        return;
//    }
    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine stop ...");

//     NOTE: this is called from another thread, so we need a new memory pool.
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

//    [_pPlayer performSelectorOnMainThread:@selector(stop) withObject:nil waitUntilDone:YES];
//    [_pPlayer performSelectorOnMainThread:@selector(stop) withObject:nil waitUntilDone:NO];
    [_pPlayer stop];

//    if (_mime.isImage()) {
////        if (_lastImageView != nil) {
////            LOGNS(Omm::Av, upnpav, debug, "remove image view from super view");
////            [_lastImageView performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
////            [_imageBackgroundView performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
////        }
//    }
//    else {
//        // FIXME: after stream finished, player view is removed and clicking stop crashes
//        LOGNS(Omm::Av, upnpav, debug, "ENGINE stopping media player engine ...");
//        MediaPlayerController* pPlayer = static_cast<MediaPlayerController*>(_player);
//
//        [pPlayer performSelectorOnMainThread:@selector(stop) withObject:nil waitUntilDone:YES];
////        if (_mime.isVideo()) {
//            //[_player.view removeFromSuperview];
////            [pPlayer.view performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
////        }
//        [pPlayer release];
//        LOGNS(Omm::Av, upnpav, debug, "ENGINE media player engine stopped.");
//    }
//
    [pool release];

    LOGNS(Omm::Av, upnpav, debug, "AV Foundation engine stopped.");
}


void
AVFoundationEngine::next()
{
}


void
AVFoundationEngine::previous()
{
}


void
AVFoundationEngine::seekByte(Poco::UInt64 byte)
{

}


void
AVFoundationEngine::seekPercentage(float percentage)
{

}


void
AVFoundationEngine::seekSecond(Omm::r8 second)
{

}


Poco::UInt64
AVFoundationEngine::getPositionByte()
{
    return 0;
}


float
AVFoundationEngine::getPositionPercentage()
{

}


Omm::r8
AVFoundationEngine::getPositionSecond()
{
    return 0.0;
}


Omm::r8
AVFoundationEngine::getLengthSeconds()
{
    return 0.0;
}


void
AVFoundationEngine::setVolume(const std::string& channel, float vol)
{

}


float
AVFoundationEngine::getVolume(const std::string& channel)
{
    return 0.0;
//    AVAudioSession *session = [AVAudioSession sharedInstance];
}


AVFoundationEngine::TransportState
AVFoundationEngine::getTransportState()
{
    return [_pPlayer getTransportState];
}


void
AVFoundationEngine::transportStateChangedNotification()
{
//    MediaPlayerController* pPlayer = static_cast<MediaPlayerController*>(_player);
//    if ([pPlayer playbackState] == MPMoviePlaybackStateStopped && _mime.isVideo()) {
//// //        [pPlayer.view performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
//    }
//    transportStateChanged();
}


void
AVFoundationEngine::volumeChangedNotification(float volume)
{
    LOGNS(Omm::Av, upnpav, debug, "ENGINE volume changed on media player engine");
}


void
AVFoundationEngine::setMute(const std::string& channel, bool mute)
{

}


bool
AVFoundationEngine::getMute(const std::string& channel)
{
    return false;
}


//void
//AVFoundationEngine::downloadImage()
//{
//    LOGNS(Omm::Av, upnpav, debug, "download image: " + _urlString);
//    Poco::Net::HTTPStreamFactory streamOpener;
//
//    _imageBuffer.clear();
//    try {
//        std::istream* pInStream = streamOpener.open(Poco::URI(_urlString));
//        if (pInStream) {
//            _imageLength = Poco::StreamCopier::copyToString(*pInStream, _imageBuffer);
//        }
//    }
//    catch (Poco::Exception& e) {
//        LOGNS(Omm::Av, upnpav, error, "download failed: " + e.displayText());
//    }
//    if (_imageLength == 0) {
//        LOGNS(Omm::Av, upnpav, error, "download failed, no bytes received.");
//        return;
//    }
//    else {
//        LOGNS(Omm::Av, upnpav, debug, "download success, bytes: " + Poco::NumberFormatter::format(_imageLength));
//    }
//}


//void
//AVFoundationEngine::showImage()
//{
//    NSData* imageData = [NSData dataWithBytes:_imageBuffer.data() length:_imageLength];
//    if (imageData == nil) {
//        LOGNS(Omm::Av, upnpav, error, "no image data");
//        return;
//    }
//
//    UIImage* image = [UIImage imageWithData:imageData];
//    UIImageView* imageView = [[UIImageView alloc] initWithImage:image];
//    CGSize imageSize = [image size];
//    float aspect = imageSize.height / imageSize.width;
//    CGRect frame = _parentView.frame;
//    frame.size.height = aspect * frame.size.width;
//    frame.origin.x = 0;
//    frame.origin.y = (_parentView.frame.size.height - frame.size.height) / 2;
//    imageView.frame = frame;
//    NSLog(@"parent view: %@", _parentView);
//    NSLog(@"last image view: %@", _lastImageView);
//    _imageBackgroundView.frame = _parentView.frame;
//    if (_lastImageView != nil) {
//        LOGNS(Omm::Av, upnpav, debug, "remove image view from super view");
//        [_lastImageView performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
//        [_parentView performSelectorOnMainThread:@selector(setNeedsLayout) withObject:nil waitUntilDone:YES];
//        [UIView performSelectorOnMainThread:@selector(removeFromSuperview) withObject:_lastImageView waitUntilDone:YES];
//        [_lastImageView setHidden:YES];
//    }
//    [_parentView performSelectorOnMainThread:@selector(addSubview:) withObject:imageView waitUntilDone:YES];
//
//    [_parentView performSelectorOnMainThread:@selector(addSubview:) withObject:_imageBackgroundView waitUntilDone:YES];
//    [_imageBackgroundView performSelectorOnMainThread:@selector(addSubview:) withObject:imageView waitUntilDone:YES];
//
//    //[_parentView performSelectorOnMainThread:@selector(bringSubviewToFront:) withObject:imageView waitUntilDone:YES];
////    [_parentView performSelectorOnMainThread:@selector(insertSubview:) withObject:imageView waitUntilDone:YES];
////    [_parentView performSelectorOnMainThread:@selector(layoutIfNeeded) withObject:nil waitUntilDone:YES];
//
//    [_lastImageView release];
//    _lastImageView = imageView;
//    [_lastImageView retain];
//}
