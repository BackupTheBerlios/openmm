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
#include <Poco/ClassLibrary.h>
#include <Poco/Thread.h>
#include <Poco/Format.h>
#include <Poco/NumberFormatter.h>

#include "PcmAudioSink.h"


PcmAudioSink::PcmAudioSink() :
AudioSink("pcm audio sink"),
_bufferSize(4 * 1024),
_buffer(new char[_bufferSize])
{
}


PcmAudioSink::~PcmAudioSink()
{
    delete _buffer;
}


bool
PcmAudioSink::initDevice()
{
    std::string fileName(_inStreams[0]->getName() + ".pcm");
    _pcmStream.open(fileName.c_str());
    Omm::AvStream::Log::instance()->avstream().information(Poco::format("%s writing PCM sample data to file %s",
        getName(),
        fileName));
    return true;
}


void
PcmAudioSink::startPresentation()
{
    int frameCount = 0;
    Omm::AvStream::Frame* pFrame;
    
    while(true) {
        audioReadBlocking(_buffer, _bufferSize);
        _pcmStream.write(_buffer, _bufferSize);
    }
    
    Omm::AvStream::Log::instance()->avstream().debug("audio sink finished.");
}



POCO_BEGIN_MANIFEST(Omm::AvStream::AudioSink)
POCO_EXPORT_CLASS(PcmAudioSink)
POCO_END_MANIFEST
