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
#include <string>

#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/Random.h>

#include <Omm/AvStream.h>
#include <Omm/UpnpAvController.h>


class Writer : public Poco::Runnable
{
public:
    Writer(const std::string name, Omm::AvStream::ByteQueue* pByteQueue, int numberWrites, int maxWriteSize, int maxDelay) :
    _name(name),
    _pByteQueue(pByteQueue),
    _numberWrites(numberWrites),
    _maxWriteSize(maxWriteSize),
    _maxDelay(maxDelay)
    {
        _writeSizeGenerator.seed();
    }
    
private:
    virtual void run()
    {
        for(int i = 0; i < _numberWrites; ++i) {
            int writeSize = _writeSizeGenerator.next(_maxWriteSize);
            char buffer[writeSize];
            LOGNS(Omm::AvStream, avstream, debug, _name + " writing: " + Poco::NumberFormatter::format(writeSize) + " bytes, level: " + Poco::NumberFormatter::format(_pByteQueue->level()) + ", number: " + Poco::NumberFormatter::format(i));
            _pByteQueue->write(buffer, writeSize);
            LOGNS(Omm::AvStream, avstream, debug, _name + " wrote: " + Poco::NumberFormatter::format(writeSize) + " bytes, level: " + Poco::NumberFormatter::format(_pByteQueue->level()));
            if (_pByteQueue->full()) {
                LOGNS(Omm::AvStream, avstream, debug, "queue full");
            }
            Poco::Thread::sleep(_delayGenerator.next(_maxDelay));
        }
    }
    
    std::string                 _name;
    Omm::AvStream::ByteQueue*   _pByteQueue;
    int                         _numberWrites;
    int                         _maxWriteSize;
    Poco::Random                _writeSizeGenerator;
    int                         _maxDelay;
    Poco::Random                _delayGenerator;
};


class Reader : public Poco::Runnable
{
public:
    Reader(const std::string name, Omm::AvStream::ByteQueue* pByteQueue, int numberReads, int maxReadSize, int maxDelay) :
    _name(name),
    _pByteQueue(pByteQueue),
    _numberReads(numberReads),
    _maxReadSize(maxReadSize),
    _maxDelay(maxDelay)
    {
        _readSizeGenerator.seed();
    }
    
private:
    virtual void run()
    {
        for(int i = 0; i < _numberReads; ++i) {
            int readSize = _readSizeGenerator.next(_maxReadSize);
            char buffer[readSize];
            LOGNS(Omm::AvStream, avstream, debug, _name + " reading: " + Poco::NumberFormatter::format(readSize) + " bytes, level: " + Poco::NumberFormatter::format(_pByteQueue->level()) + ", number: " + Poco::NumberFormatter::format(i));
            _pByteQueue->read(buffer, readSize);
            LOGNS(Omm::AvStream, avstream, debug, _name + " read: " + Poco::NumberFormatter::format(readSize) + " bytes, level: " + Poco::NumberFormatter::format(_pByteQueue->level()));
            if (_pByteQueue->empty()) {
                LOGNS(Omm::AvStream, avstream, debug, "queue empty");
            }
            Poco::Thread::sleep(_delayGenerator.next(_maxDelay));
        }
    }
    
    std::string                 _name;
    Omm::AvStream::ByteQueue*   _pByteQueue;
    int                         _numberReads;
    int                         _maxReadSize;
    Poco::Random                _readSizeGenerator;
    int                         _maxDelay;
    Poco::Random                _delayGenerator;
};


int main(int argc, char** argv)
{
    Omm::AvStream::ByteQueue queue(2500);
    Poco::Thread t1;
    Writer w1("writer 1", &queue, 1000, 500, 10);
    Poco::Thread t2;
    Writer w2("writer 2", &queue, 1000, 500, 10);
    Poco::Thread t3;
    Reader r1("reader 1", &queue, 1000, 500, 10);
    Poco::Thread t4;
    Reader r2("reader 2", &queue, 1000, 500, 10);

    
    std::clog << "starting writer 1 ..." << std::endl;
    t1.start(w1);
    std::clog << "starting writer 2 ..." << std::endl;
    t2.start(w2);
    std::clog << "starting reader 1 ..." << std::endl;
    t3.start(r1);
    std::clog << "starting reader 2 ..." << std::endl;
    t4.start(r2);
    
    std::clog << "joining writer 1 ..." << std::endl;
    t1.join();
    std::clog << "writer 1 joined." << std::endl;
    std::clog << "joining writer 2 ..." << std::endl;
    t2.join();
    std::clog << "writer 2 joined." << std::endl;
    std::clog << "joining reader 1 ..." << std::endl;
    t3.join();
    std::clog << "reader 1 joined." << std::endl;
    std::clog << "joining reader 2 ..." << std::endl;
    t4.join();
    std::clog << "reader 2 joined." << std::endl;
    
    return 0;
}