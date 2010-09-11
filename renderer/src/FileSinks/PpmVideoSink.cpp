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


PpmOverlay::PpmOverlay(Omm::AvStream::VideoSink* pVideoSink, int width, int height) :
Overlay(pVideoSink, width, height)
{
}


PpmVideoSink::PpmVideoSink() :
// ppm file sink size doesn't matter, all frames are written out unscaled
VideoSink("ppm video sink", 0, 0, PIX_FMT_RGB24, 5),
_frameCount(0)
{
}


PpmVideoSink::~PpmVideoSink()
{
}


bool
PpmVideoSink::initDevice()
{
    Omm::AvStream::Log::instance()->avstream().debug("opening ppm video sink ...");
    
    for (int numOverlay = 0; numOverlay < _overlayCount; numOverlay++) {
        Omm::AvStream::Frame* pFrame = getInStream(0)->allocateVideoFrame(_pixelFormat);
        
        PpmOverlay* pOverlay = new PpmOverlay(this, getInStream(0)->getInfo()->width(), getInStream(0)->getInfo()->height());
        
        pOverlay->_pFrame = pFrame;
        
        pOverlay->_data[0] = (uint8_t*)pFrame->planeData(0);
        pOverlay->_data[1] = (uint8_t*)pFrame->planeData(1);
        pOverlay->_data[2] = (uint8_t*)pFrame->planeData(2);
        
        pOverlay->_pitch[0] = pFrame->planeSize(0);
        pOverlay->_pitch[1] = pFrame->planeSize(1);
        pOverlay->_pitch[2] = pFrame->planeSize(2);
        
        _overlayVector[numOverlay] = pOverlay;
    }

    Omm::AvStream::Log::instance()->avstream().debug(getName() + " opened.");
    return true;
}


void
PpmVideoSink::displayFrame(Omm::AvStream::Overlay* pOverlay)
{
    std::string fileName(getInStream(0)->getName() + "_" + Poco::NumberFormatter::format0(++_frameCount, 3) + ".ppm");
    
    // normally, VideoSink implementation is responsible for scaling the video (hw-accelerated)
    // however, the ppm file sink only writes the stream in original size into files.
//     int x, y, w, h;
//     displayRect(x, y, w, h);
    
    std::ofstream ppmFile(fileName.c_str());
    // write PPM header
    ppmFile << "P6\n" << getInStream(0)->getInfo()->width() << " " << getInStream(0)->getInfo()->height() << "\n" << 255 << "\n";
    // write RGB pixel data
    ppmFile.write(static_cast<PpmOverlay*>(pOverlay)->_pFrame->planeData(0), getInStream(0)->getInfo()->width() * getInStream(0)->getInfo()->height() * 3);
}



POCO_BEGIN_MANIFEST(Omm::AvStream::VideoSink)
POCO_EXPORT_CLASS(PpmVideoSink)
POCO_END_MANIFEST
