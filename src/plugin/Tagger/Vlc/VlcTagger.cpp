/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include <Poco/ClassLibrary.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/Timestamp.h>
#include <Poco/Path.h>

#include <vlc/vlc.h>
#ifdef LIBVLC_VERSION_HEADER_FOUND
#include <vlc/libvlc_version.h>
#endif

#include <Omm/AvStream.h>
#include <Poco/String.h>

#include "VlcTagger.h"
#include "Omm/UpnpAvObject.h"


VlcMeta::VlcMeta() :
_isStillImage(false)
{
}


VlcMeta::~VlcMeta()
{
}


bool
VlcMeta::isStillImage()
{
    return _isStillImage;
}


std::string
VlcMeta::getMime()
{
    return _mime;
}


VlcTagger::VlcTagger()
{
#if LIBVLC_VERSION_INT < 0x110
    _pException = new libvlc_exception_t;
    libvlc_exception_init(_pException);
#endif

    int argc = 1;
    const char* argv[1] = {"ommtagger"};
#if LIBVLC_VERSION_INT < 0x110
    _pVlcInstance = libvlc_new(argc, argv, _pException);
#else
    _pVlcInstance = libvlc_new(argc, argv);
#endif
}


VlcTagger::~VlcTagger()
{
}


Omm::AvStream::Meta*
VlcTagger::tag(const std::string& uri)
{
    Omm::Av::Log::instance()->upnpav().debug("vlc tagger tagging: " + uri);

    VlcMeta* pMeta = new VlcMeta;

#if LIBVLC_VERSION_INT < 0x110
    _pVlcMedia = libvlc_media_new(_pVlcInstance, uri.c_str(), _pException);
#else
    _pVlcMedia = libvlc_media_new_location(_pVlcInstance, uri.c_str());
#endif
    handleException();

#if LIBVLC_VERSION_INT < 0x110
#else
    libvlc_media_parse(_pVlcMedia);
#endif

#if LIBVLC_VERSION_INT < 0x110
    char* pTitle = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_Title, _pException);
#else
    char* pTitle = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_Title);
#endif
    handleException();
    if (pTitle) {
        pMeta->setTag("title", std::string(pTitle));
    }

#if LIBVLC_VERSION_INT < 0x110
    char* pArtist = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_Artist, _pException);
#else
    char* pArtist = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_Artist);
#endif
    handleException();
    if (pArtist) {
        pMeta->setTag("artist", std::string(pArtist));
    }

#if LIBVLC_VERSION_INT < 0x110
    char* pAlbum = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_Album, _pException);
#else
    char* pAlbum = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_Album);
#endif
    handleException();
    if (pAlbum) {
        pMeta->setTag("album", std::string(pAlbum));
    }

#if LIBVLC_VERSION_INT < 0x110
    char* pTrack = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_TrackNumber, _pException);
#else
    char* pTrack = libvlc_media_get_meta(_pVlcMedia, libvlc_meta_TrackNumber);
#endif
    handleException();
    if (pTrack) {
        pMeta->setTag("track", std::string(pTrack));
    }

    libvlc_media_release(_pVlcMedia);

    // vlc libs need to play the data in order to analyse the streams
    // thus we make a simple filename based media type detection
    Poco::Path path(uri);
    // try to get a filename extension for type of media
    std::string extension = Poco::toLower(path.getExtension());
//    Omm::Av::Log::instance()->upnpav().debug("vlc tagger extension: " + extension);

    Omm::AvStream::Meta::ContainerFormat containerFormat = Omm::AvStream::Meta::CF_UNKNOWN;
    if (extension == "mp3") {
        pMeta->_mime = Omm::Av::Mime::AUDIO_MPEG;
        containerFormat = Omm::AvStream::Meta::CF_AUDIO;
    }
    else if (extension == "wma" || extension == "ogg" || extension == "wav") {
        pMeta->_mime = Omm::Av::Mime::TYPE_AUDIO;
        containerFormat = Omm::AvStream::Meta::CF_AUDIO;
    }
    else if (extension == "mp4" || extension == "mpeg" || extension == "mpg") {
        pMeta->_mime = Omm::Av::Mime::VIDEO_MPEG;
        containerFormat = Omm::AvStream::Meta::CF_VIDEO;
    }
    else if (extension == "avi" || extension == "wmv" || extension == "flv") {
        pMeta->_mime = Omm::Av::Mime::TYPE_VIDEO;
        containerFormat = Omm::AvStream::Meta::CF_VIDEO;
    }
    else if (extension == "jpg" || extension == "png" || extension == "gif") {
        pMeta->_mime = Omm::Av::Mime::TYPE_IMAGE;
        containerFormat = Omm::AvStream::Meta::CF_IMAGE;
    }
    else if (extension == "m3u") {
        pMeta->_mime = Omm::Av::Mime::PLAYLIST;
        containerFormat = Omm::AvStream::Meta::CF_PLAYLIST;
    }

    if (containerFormat == Omm::AvStream::Meta::CF_AUDIO) {
        pMeta->_isStillImage = false;
        VlcStreamInfo* pStreamInfo = new VlcStreamInfo;
        pStreamInfo->_isAudio = true;
        pStreamInfo->_isVideo = false;
        pMeta->addStream(pStreamInfo);
    }
    else if (containerFormat == Omm::AvStream::Meta::CF_VIDEO) {
        pMeta->_isStillImage = false;
        VlcStreamInfo* pStreamInfo = new VlcStreamInfo;
        pStreamInfo->_isAudio = false;
        pStreamInfo->_isVideo = true;
        pMeta->addStream(pStreamInfo);
    }
    else if (containerFormat == Omm::AvStream::Meta::CF_IMAGE) {
        pMeta->_isStillImage = true;
        VlcStreamInfo* pStreamInfo = new VlcStreamInfo;
        pStreamInfo->_isAudio = false;
        pStreamInfo->_isVideo = false;
        pMeta->addStream(pStreamInfo);
    }
    else if (containerFormat == Omm::AvStream::Meta::CF_PLAYLIST) {
        pMeta->setIsPlaylist(true);
    }

    return pMeta;
}


Omm::AvStream::Meta*
VlcTagger::tag(std::istream& istr)
{
    return 0;
}


void
VlcTagger::handleException()
{
#if LIBVLC_VERSION_INT < 0x110
    if (libvlc_exception_raised(_pException)) {
        Omm::Av::Log::instance()->upnpav().error("vlc tagger: " + std::string(libvlc_exception_get_message(_pException)));
    }
    libvlc_exception_init(_pException);
#else
    const char* errMsg = libvlc_errmsg();
    if (errMsg) {
        Omm::Av::Log::instance()->upnpav().error("vlc tagger: " + std::string(errMsg));
    }
#endif
}


VlcStreamInfo::VlcStreamInfo() :
_isAudio(false),
_isVideo(false)
{
}


VlcStreamInfo::~VlcStreamInfo()
{
}


bool
VlcStreamInfo::isAudio()
{
    return _isAudio;
}


bool
VlcStreamInfo::isVideo()
{
    return _isVideo;
}


bool
VlcStreamInfo::findCodec()
{
    return true;
}


bool
VlcStreamInfo::findEncoder()
{
    return true;
}


int
VlcStreamInfo::sampleSize()
{
    return 0;
}


unsigned int
VlcStreamInfo::sampleRate()
{
    return 0;
}


int
VlcStreamInfo::channels()
{
    return 0;
}


int
VlcStreamInfo::width()
{
    return 0;
}


int
VlcStreamInfo::height()
{
    return 0;
}


Omm::AvStream::Meta::ColorCoding
VlcStreamInfo::pixelFormat()
{
//    return FFmpegMeta::fromFFmpegPixFmt(_pAvCodecContext->pix_fmt);
}


int
VlcStreamInfo::pictureSize()
{
    return 0;
}


float
VlcStreamInfo::aspectRatio()
{
    return 1.0;
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::AvStream::Tagger)
POCO_EXPORT_CLASS(VlcTagger)
POCO_END_MANIFEST
#endif
