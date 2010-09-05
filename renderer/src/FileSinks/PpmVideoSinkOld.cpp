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

#include "PpmVideoSinkOld.h"


PpmoldVideoSink::PpmoldVideoSink() :
VideoSink("ppm video sink")
{
}


PpmoldVideoSink::~PpmoldVideoSink()
{
}


bool
PpmoldVideoSink::init()
{
    // init() is called on attach() so _inStreams[0]->_pStreamInfo should be available
    if (!_inStreams[0]->getInfo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream info not allocated", getName()));
        return false;
    }
    if (!_inStreams[0]->getInfo()->findCodec()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, could not find codec", getName()));
        return false;
    }
    if (!_inStreams[0]->getInfo()->isVideo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream is not a video stream", getName()));
        return false;
    }
    return true;
}


void
PpmoldVideoSink::run()
{
    if (!_inStreams[0]) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to video sink, stopping.");
        return;
    }
    
    int frameCount = 0;
    Omm::AvStream::Frame* pFrame;
    while (!_quit && (pFrame = _inStreams[0]->getFrame()))
    {
        Omm::AvStream::Frame* pDecodedFrame = _inStreams[0]->decodeFrame(pFrame);
        if (!pDecodedFrame) {
            Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s decoding failed, discarding packet", getName()));
        }
        else {
            std::string fileName(_inStreams[0]->getName() + "_" + Poco::NumberFormatter::format0(++frameCount, 3) + ".ppm");
            pDecodedFrame->writePpm(fileName);
        }
    }
    
    Omm::AvStream::Log::instance()->avstream().debug("video sink finished.");
}


int
PpmoldVideoSink::eventLoop()
{
    Omm::AvStream::Log::instance()->avstream().debug("event loop ...");
//     Poco::Thread::sleep(10000);
    
}



POCO_BEGIN_MANIFEST(Omm::AvStream::VideoSink)
POCO_EXPORT_CLASS(PpmoldVideoSink)
POCO_END_MANIFEST
