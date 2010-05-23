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

#include "PpmVideoSink.h"


PpmVideoSink::PpmVideoSink() :
Sink("ppm video sink")
{
    // video sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams.back()->setInfo(0);
    // NOTE: stream queue for decoded video frames has size one. Reasons:
    // 1. decoded video frames take up quit some memory
    // 2. there is a memory management issue with ffmpeg, haven't been able to store
    //    decoded video frames, yet.
    _inStreams.back()->setQueue(new Omm::AvStream::StreamQueue(this));
    
    // and no output stream
}


PpmVideoSink::~PpmVideoSink()
{
}



bool
PpmVideoSink::init()
{
    if (!_inStreams[0]->getInfo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream info not allocated", getName()));
        return false;
    }
    return true;
}


void
PpmVideoSink::run()
{
    int frameCount = 0;
    // TODO: Node::run(), this loop is the same for all nodes
    Omm::AvStream::Frame* pFrame;
    if (!_inStreams[0]) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to video sink, stopping.");
        return;
    }
    while (!_quit && (pFrame = _inStreams[0]->getFrame()))
    {
        std::string fileName(_inStreams[0]->getName() + "_" + Poco::NumberFormatter::format0(++frameCount, 3) + ".ppm");
        Omm::AvStream::Frame* pDecodedFrame = pFrame->decode();
        if (!pDecodedFrame) {
            Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s decoding failed, discarding packet", getName()));
        }
        else {
            pDecodedFrame->writePpm(fileName);
        }
    }
    
    Omm::AvStream::Log::instance()->avstream().debug("video sink finished.");
}


int
PpmVideoSink::eventLoop()
{
    Omm::AvStream::Log::instance()->avstream().debug("event loop ...");
//     Poco::Thread::sleep(10000);
    
}



POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(PpmVideoSink)
POCO_END_MANIFEST
