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
#include <iostream>
#include <fstream>

#include <AvStream.h>
// #include <AvStreamFFmpeg.h>
#include <UpnpAvController.h>

int main(int argc, char** argv)
{
//     Omm::AvStream::FFmpegNodeFactory nodeFactory;
//     Omm::AvStream::Tagger* pTagger = nodeFactory.tagger();
    Omm::AvStream::Demuxer demuxer;
//     demuxer.set(pTagger->tag(std::string(argv[1])));

//     Omm::AvStream::Muxer muxer;
//     muxer.set(std::string("mux.mpeg"), std::string("mpeg"));

//     demuxer.attach(&muxer, demuxer.firstAudioStream(), 0);
//     demuxer.attach(&muxer, demuxer.firstVideoStream(), 1);
    
    demuxer.start();
    sleep(2);
    demuxer.stop();
}

