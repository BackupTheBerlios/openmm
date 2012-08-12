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

#ifndef Log_INCLUDED
#define Log_INCLUDED

#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Timestamp.h>


namespace Omm {
namespace Util {

class Log
{
public:
    static Log* instance();
    Poco::Channel* channel();

    Poco::Logger& util();
    Poco::Logger& plugin();

private:
    Log();

    static Log*                         _pInstance;
    Poco::FormattingChannel*            _pChannel;
    Poco::Logger*                       _pUtilLogger;
    Poco::Logger*                       _pPluginLogger;
};


class TCPChannel : public Poco::Channel
{
public:
    TCPChannel();

    virtual void open();
    virtual void close();
    virtual void log(const Poco::Message& message);

protected:
    virtual ~TCPChannel();

private:
    void connectionThread();
    void sendMessage(const Poco::Message* pMessage);

    int                                         _port;
    Poco::Net::ServerSocket*                    _pSocket;
    Poco::Net::StreamSocket                     _connection;
    Poco::Thread                                _connectionThread;
    Poco::RunnableAdapter<TCPChannel>           _connectionThreadRunnable;
    Poco::FastMutex                             _lock;
    std::vector<Poco::Message*>                 _buffer;
    Poco::Timestamp::TimeDiff                   _bufferTime;
};

}  // namespace Omm
}  // namespace Util

#endif
