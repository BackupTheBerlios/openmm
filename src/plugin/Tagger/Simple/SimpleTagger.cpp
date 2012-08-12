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

#include <Omm/AvStream.h>
#include <Omm/Log.h>

#include "SimpleTagger.h"
#include "Omm/UpnpAvObject.h"


SimpleMeta::SimpleMeta() :
_isStillImage(false)
{
}


SimpleMeta::~SimpleMeta()
{
}


bool
SimpleMeta::isStillImage()
{
    return _isStillImage;
}


std::string
SimpleMeta::getMime()
{
    return _mime;
}


SimpleTagger::SimpleTagger()
{
}


SimpleTagger::~SimpleTagger()
{
}


Omm::AvStream::Meta*
SimpleTagger::tag(const std::string& uri)
{
    LOGNS(Omm::Av, upnpav, debug, "simple tagger tagging: " + uri);
    Poco::Path path(uri);
    // try to get filename for title
    std::string title = path.getFileName();
    LOGNS(Omm::Av, upnpav, debug, "simple tagger title: " + title);
    // try to get a filename extension for type of media
    std::string extension = Poco::toLower(path.getExtension());
    LOGNS(Omm::Av, upnpav, debug, "simple tagger extension: " + extension);
    SimpleMeta* pMeta = new SimpleMeta;

    // TODO: define possible tag strings as string constants
    pMeta->setTag("title", title);

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
        SimpleStreamInfo* pStreamInfo = new SimpleStreamInfo;
        pStreamInfo->_isAudio = true;
        pStreamInfo->_isVideo = false;
        pMeta->addStream(pStreamInfo);
    }
    else if (containerFormat == Omm::AvStream::Meta::CF_VIDEO) {
        pMeta->_isStillImage = false;
        SimpleStreamInfo* pStreamInfo = new SimpleStreamInfo;
        pStreamInfo->_isAudio = false;
        pStreamInfo->_isVideo = true;
        pMeta->addStream(pStreamInfo);
    }
    else if (containerFormat == Omm::AvStream::Meta::CF_IMAGE) {
        pMeta->_isStillImage = true;
        SimpleStreamInfo* pStreamInfo = new SimpleStreamInfo;
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
SimpleTagger::tag(std::istream& istr)
{
    return 0;
}


SimpleStreamInfo::SimpleStreamInfo() :
_isAudio(false),
_isVideo(false)
{
}


SimpleStreamInfo::~SimpleStreamInfo()
{
}


bool
SimpleStreamInfo::isAudio()
{
    return _isAudio;
}


bool
SimpleStreamInfo::isVideo()
{
    return _isVideo;
}


bool
SimpleStreamInfo::findCodec()
{
    return true;
}


bool
SimpleStreamInfo::findEncoder()
{
    return true;
}


int
SimpleStreamInfo::sampleSize()
{
    return 0;
}


unsigned int
SimpleStreamInfo::sampleRate()
{
    return 0;
}


int
SimpleStreamInfo::channels()
{
    return 0;
}


int
SimpleStreamInfo::width()
{
    return 0;
}


int
SimpleStreamInfo::height()
{
    return 0;
}


Omm::AvStream::Meta::ColorCoding
SimpleStreamInfo::pixelFormat()
{
//    return FFmpegMeta::fromFFmpegPixFmt(_pAvCodecContext->pix_fmt);
}


int
SimpleStreamInfo::pictureSize()
{
    return 0;
}


float
SimpleStreamInfo::aspectRatio()
{
    return 1.0;
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::AvStream::Tagger)
POCO_EXPORT_CLASS(SimpleTagger)
POCO_END_MANIFEST
#endif
