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
Sink("pcm audio sink")
{
    // audio sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams.back()->setInfo(0);
    _inStreams.back()->setQueue(new Omm::AvStream::StreamQueue(this));
    
    // and no output stream
}


PcmAudioSink::~PcmAudioSink()
{
}



bool
PcmAudioSink::init()
{
    if (!_inStreams[0]->getInfo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream info not allocated", getName()));
        return false;
    }
    _pcmStream.open(std::string(_inStreams[0]->getName() + ".pcm").c_str());
    return true;
}


void
PcmAudioSink::run()
{
    int frameCount = 0;
    Omm::AvStream::Frame* pFrame;
    if (!_inStreams[0]) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to audio sink, stopping.");
        return;
    }
    while (!_quit && (pFrame = _inStreams[0]->getFrame()))
    {
        std::clog << "writing frame to file " << _inStreams[0]->getName() << ".pcm";
        _pcmStream.write(pFrame->data(), pFrame->size());
    }
    
    Omm::AvStream::Log::instance()->avstream().debug("audio sink finished.");
}


int
PcmAudioSink::eventLoop()
{
    Omm::AvStream::Log::instance()->avstream().debug("event loop ...");
//     Poco::Thread::sleep(10000);
    
}



POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(PcmAudioSink)
POCO_END_MANIFEST
