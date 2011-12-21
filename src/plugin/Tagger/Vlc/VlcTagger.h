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
#ifndef VlcTagger_INCLUDED
#define VlcTagger_INCLUDED

#include <Omm/AvStream.h>

#if LIBVLC_VERSION_INT < 0x110
struct libvlc_exception_t;
#endif
struct libvlc_instance_t;
struct libvlc_media_t;

class VlcMeta : public Omm::AvStream::Meta
{
    friend class VlcTagger;

public:
    VlcMeta();
    virtual ~VlcMeta();

    virtual bool isStillImage();
    virtual std::string getMime();

private:
    bool            _isStillImage;
    std::string     _mime;
};


class VlcTagger : public Omm::AvStream::Tagger
{
public:
    VlcTagger();
    ~VlcTagger();

    virtual Omm::AvStream::Meta* tag(const std::string& uri);
    virtual Omm::AvStream::Meta* tag(std::istream& istr);

private:
    void handleException();

#if LIBVLC_VERSION_INT < 0x110
    libvlc_exception_t*     _pException;
#endif
    libvlc_instance_t*      _pVlcInstance;
    libvlc_media_t*         _pVlcMedia;
};


class VlcStreamInfo : public Omm::AvStream::StreamInfo
{
    friend class VlcTagger;

public:
    VlcStreamInfo();
    virtual ~VlcStreamInfo();

    virtual bool isAudio();
    virtual bool isVideo();
    virtual bool findCodec();
    virtual bool findEncoder();

    // audio parameters
    virtual int sampleSize();
    virtual unsigned int sampleRate();
    virtual int channels();

    // video parameters
    virtual int width();
    virtual int height();
    virtual Omm::AvStream::Meta::ColorCoding pixelFormat();
    virtual int pictureSize();
    virtual float aspectRatio();

private:
    bool    _isAudio;
    bool    _isVideo;
};


#endif