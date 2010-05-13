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

#include "QtSink.h"


SinkPlugin::SinkPlugin() 
{
}


SinkPlugin::~SinkPlugin()
{
}


void
SinkPlugin::open()
{
}


void
SinkPlugin::close()
{
    std::clog << "SinkPlugin::close()" << std::endl;
    
}


void
SinkPlugin::writeFrame(Omm::Av::Frame* pFrame)
{

}


// void SinkPlugin::pause()
// {
// }
// 

// void SinkPlugin::resume()
// {
// }


// static int resume(snd_pcm_t *pcm)
// {
// }


// int SinkPlugin::latency()
// {
// }

POCO_BEGIN_MANIFEST(Omm::Av::Sink)
POCO_EXPORT_CLASS(SinkPlugin)
POCO_END_MANIFEST
