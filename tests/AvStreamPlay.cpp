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

#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include <AvStream.h>
#include <Util.h>

#include "../renderer/src/AvStreamEngine.h"

class AvPlayer : public Poco::Util::ServerApplication
{
public:
    AvPlayer()
    {
        _engine.createPlayer();
    }
    
    
    ~AvPlayer()
    {
        _engine.destructPlayer();
    }
    
    
    void play(const std::string& uri)
    {
        _engine.setUri(uri);
        _engine.load();
        waitForTerminationRequest();
        _engine.stop();
    }

    AvStreamEngine _engine;
};


int main(int argc, char** argv)
{
    AvPlayer player;
    
    player.play(std::string(argv[1]));
}

