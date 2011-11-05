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

#include "EngineMPMoviePlayer.h"

#import <MediaPlayer/MPMoviePlayerController.h>
// iOS 3.2 and later, only
#import <MediaPlayer/MPMoviePlayerViewController.h>
#import <AVFoundation/AVAudioSession.h>


@interface MediaPlayerController : MPMoviePlayerController
{
    MPMoviePlayerEngine*        _pEngine;
}
@end


@implementation MediaPlayerController

//- (void)playbackFinished:(NSNotification*)notification
//{
//}


- (void)playbackStateChanged:(NSNotification*)notification
{
    Omm::Av::Log::instance()->upnpav().debug("ENGINE sending notification playback state did change");
    _pEngine->transportStateChangedNotification();
}


- (void)volumeChanged:(NSNotification *)notification
{
    float volume = [[[notification userInfo]
                        objectForKey:@"AVSystemController_AudioVolumeNotificationParameter"]
                        floatValue];
    _pEngine->volumeChangedNotification(volume);
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    // stop playback?
}


- (void)setEngine:(MPMoviePlayerEngine*)pEngine
{
    _pEngine = pEngine;
}


- (id)initWithContentURL:(NSURL*)contentURL
{
    if (self = [super initWithContentURL:contentURL]) {
        self.controlStyle = MPMovieControlStyleNone;
        [[NSNotificationCenter defaultCenter]
            addObserver:self
            selector:@selector(playbackStateChanged:)
            name:MPMoviePlayerPlaybackStateDidChangeNotification
            object:self];
        [[NSNotificationCenter defaultCenter]
            addObserver:self
            selector:@selector(volumeChanged:)
            name:@"AVSystemController_SystemVolumeDidChangeNotification"
            object:nil];
    }
    return self;
}

@end


MPMoviePlayerEngine::MPMoviePlayerEngine() :
_player(0),
_imageLength(0)
{
    _engineId = "iphone MediaPlayer engine " + Omm::OMM_VERSION;
//    _lastImageView = nil;
}


MPMoviePlayerEngine::~MPMoviePlayerEngine()
{
}


void
MPMoviePlayerEngine::createPlayer()
{
//    _imageBackgroundView = [[UIView alloc] init];
//    _imageBackgroundView.backgroundColor = [UIColor blackColor];
}


void
MPMoviePlayerEngine::setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

    Omm::Av::Log::instance()->upnpav().debug("media player engine, set uri");

    _urlString = uri;
    _protInfo = protInfo;
    _mime = Omm::Av::Mime(protInfo.getMimeString());
}


void
MPMoviePlayerEngine::setFullscreen(bool on)
{
}


void
MPMoviePlayerEngine::play()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

    Omm::Av::Log::instance()->upnpav().debug("media player engine, play");

    // NOTE: this is called from another thread, so we need a new memory pool.
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSURL* url = [NSURL URLWithString:[[NSString alloc] initWithUTF8String:_urlString.c_str()]];
//    CFURLRef urlRef = CFURLCreateWithString(NULL, CFStringCreateWithCharacters(NULL, (UniChar*)_urlString.data(), _urlString.size()), NULL);

    if (_mime.isImage()) {
//        NSLog(@"showing image URL: %@", url);
//        downloadImage();
//        showImage();
    }
//    else if (_mime.isAudio()) {
//
//    }
    else {
       _startTime = 0;
       _length = 0.0;

       Omm::Av::Log::instance()->upnpav().debug("ENGINE alloc media player ...");
       MediaPlayerController* pPlayer = [MediaPlayerController alloc];
       _player = pPlayer;
       [pPlayer setEngine:this];

       Omm::Av::Log::instance()->upnpav().debug("ENGINE init media player ...");
       [pPlayer performSelectorOnMainThread:@selector(initWithContentURL:) withObject:url waitUntilDone:YES];

       if (pPlayer) {
           Omm::Av::Log::instance()->upnpav().debug("ENGINE playing URL: " + _urlString);
           [pPlayer performSelectorOnMainThread:@selector(play) withObject:nil waitUntilDone:YES];
       }
       if (_mime.isVideo()) {
           Omm::Av::Log::instance()->upnpav().debug("ENGINE adding media player view ...");
//           [_parentView performSelectorOnMainThread:@selector(addSubview:) withObject:_player.view waitUntilDone:YES];
           [static_cast<UIView*>(_pVisual->getWindow()) performSelectorOnMainThread:@selector(addSubview:) withObject:pPlayer.view waitUntilDone:YES];
           pPlayer.view.frame = static_cast<UIView*>(_pVisual->getWindow()).frame;
           Omm::Av::Log::instance()->upnpav().debug("ENGINE adding media player view finished.");
       }
    }

   [pool release];
}


void
MPMoviePlayerEngine::setSpeed(int nom, int denom)
{
}


void
MPMoviePlayerEngine::pause()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

    if (!_player) {
        return;
    }

    // NOTE: this is called from another thread, so we need a new memory pool.
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    MediaPlayerController* pPlayer = static_cast<MediaPlayerController*>(_player);

    if (_mime.isImage()) {

    }
    else {
        [pPlayer performSelectorOnMainThread:@selector(pause) withObject:nil waitUntilDone:YES];
//        [pPlayer.moviePlayer performSelectorOnMainThread:@selector(pause) withObject:nil waitUntilDone:YES];
    }

    [pool release];
}


void
MPMoviePlayerEngine::stop()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);

    if (!_player) {
        return;
    }

    // NOTE: this is called from another thread, so we need a new memory pool.
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    if (_mime.isImage()) {
//        if (_lastImageView != nil) {
//            Omm::Av::Log::instance()->upnpav().debug("remove image view from super view");
//            [_lastImageView performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
//            [_imageBackgroundView performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
//        }
    }
    else {
        // FIXME: after stream finished, player view is removed and clicking stop crashes
        Omm::Av::Log::instance()->upnpav().debug("ENGINE stopping media player engine ...");
        MediaPlayerController* pPlayer = static_cast<MediaPlayerController*>(_player);

        [pPlayer performSelectorOnMainThread:@selector(stop) withObject:nil waitUntilDone:YES];
//        if (_mime.isVideo()) {
            //[_player.view removeFromSuperview];
//            [pPlayer.view performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
//        }
        [pPlayer release];
    }

    [pool release];
}


void
MPMoviePlayerEngine::next()
{
}


void
MPMoviePlayerEngine::previous()
{
}


void
MPMoviePlayerEngine::seekByte(Poco::UInt64 byte)
{

}


void
MPMoviePlayerEngine::seekPercentage(float percentage)
{

}


void
MPMoviePlayerEngine::seekSecond(float second)
{

}


Poco::UInt64
MPMoviePlayerEngine::getPositionByte()
{

}


float
MPMoviePlayerEngine::getPositionPercentage()
{

}


float
MPMoviePlayerEngine::getPositionSecond()
{

}


float
MPMoviePlayerEngine::getLengthSeconds()
{

}


void
MPMoviePlayerEngine::setVolume(const std::string& channel, float vol)
{

}


float
MPMoviePlayerEngine::getVolume(const std::string& channel)
{
//    AVAudioSession *session = [AVAudioSession sharedInstance];
}


MPMoviePlayerEngine::TransportState
MPMoviePlayerEngine::getTransportState()
{
    MediaPlayerController* pPlayer = static_cast<MediaPlayerController*>(_player);
    MPMoviePlaybackState playbackState = [pPlayer playbackState];
    switch (playbackState) {
        case MPMoviePlaybackStateInterrupted:
        case MPMoviePlaybackStateStopped:
        case MPMoviePlaybackStatePaused:
            return Stopped;
        case MPMoviePlaybackStatePlaying:
            return Playing;
        case MPMoviePlaybackStateSeekingForward:
        case MPMoviePlaybackStateSeekingBackward:
            return Transitioning;
//        case MPMoviePlaybackStatePaused:
//            return PausedPlayback;
        default:
            return Stopped;
    }
}


void
MPMoviePlayerEngine::transportStateChangedNotification()
{
    MediaPlayerController* pPlayer = static_cast<MediaPlayerController*>(_player);
    if ([pPlayer playbackState] == MPMoviePlaybackStateStopped && _mime.isVideo()) {
        [pPlayer.view performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
    }
    transportStateChanged();
}


void
MPMoviePlayerEngine::volumeChangedNotification(float volume)
{
    Omm::Av::Log::instance()->upnpav().debug("ENGINE volume changed on media player engine");
}


//void
//MPMoviePlayerEngine::downloadImage()
//{
//    Omm::Av::Log::instance()->upnpav().debug("download image: " + _urlString);
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
//        Omm::Av::Log::instance()->upnpav().error("download failed: " + e.displayText());
//    }
//    if (_imageLength == 0) {
//        Omm::Av::Log::instance()->upnpav().error("download failed, no bytes received.");
//        return;
//    }
//    else {
//        Omm::Av::Log::instance()->upnpav().debug("download success, bytes: " + Poco::NumberFormatter::format(_imageLength));
//    }
//}


//void
//MPMoviePlayerEngine::showImage()
//{
//    NSData* imageData = [NSData dataWithBytes:_imageBuffer.data() length:_imageLength];
//    if (imageData == nil) {
//        Omm::Av::Log::instance()->upnpav().error("no image data");
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
//        Omm::Av::Log::instance()->upnpav().debug("remove image view from super view");
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
