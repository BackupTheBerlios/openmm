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

#ifndef EngingePhonon_INCLUDED
#define EngingePhonon_INCLUDED

#include <Omm/UpnpAvRenderer.h>


class PhononEngine : public Omm::Av::Engine
{
public:
    PhononEngine();
    ~PhononEngine();

    virtual void createPlayer();

    /*
      AVTransport
    */
    virtual void setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo = Omm::Av::ProtocolInfo());
//     virtual void setUri(std::istream& istr, const Omm::Av::ProtocolInfo& protInfo = Omm::Av::ProtocolInfo());
    virtual void play();

    /**
        void setSpeed(int nom, int denom)
        set speed to nom/denom
    */
    virtual void setSpeed(int nom, int denom);
    /**
        void pause()
        toggle pause
    */
    virtual void pause();
    virtual void stop();

    virtual void seekByte(Poco::UInt64 byte);
    virtual void seekPercentage(float percentage);
    virtual void seekSecond(float second);

    virtual Poco::UInt64 getPositionByte();
    virtual float getPositionPercentage();
    virtual float getPositionSecond();

    virtual float getLengthSeconds();

    virtual TransportState getTransportState();

    /*
      Rendering Control
    */
    virtual void setVolume(const std::string& channel, float vol);
    virtual float getVolume(const std::string& channel);

private:
    std::string             _uri;
    Omm::Av::Mime           _mime;

};


#endif
