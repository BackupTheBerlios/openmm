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
#include "FbVideoXine.h"


void
XineEngineFb::initVisual()
{
//    char* videoDriverName = "fb";
//    char* videoDriverName = "vidixfb";
    videoDriverName = "directfb";
//    int visualType = XINE_VISUAL_TYPE_FB;
    visualType = XINE_VISUAL_TYPE_DFB;
    visual.frame_output_cb = FrameOutputCallback;
    _videoDriver = xine_open_video_driver(_xineEngine,
	videoDriverName,  visualType,
	(void *) &(visual));
}


void
XineEngineFb::closeVisual()
{

}


POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(XineEngineFb)
POCO_END_MANIFEST