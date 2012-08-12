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
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS

#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/ClassLoader.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>

#include <fstream>
#include <cstring>
#include <cmath>

#include "AvStream.h"
#include "Log.h"

namespace Omm {
namespace AvStream {

Log* Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Log::Log()
{
    Poco::Channel* pChannel = Util::Log::instance()->channel();
#ifdef NDEBUG
    _pAvStreamLogger = &Poco::Logger::create("AVSTREAM", pChannel, 0);
#else
    _pAvStreamLogger = &Poco::Logger::create("AVSTREAM", pChannel, Poco::Message::PRIO_TRACE);
#endif
//     _pAvStreamLogger = &Poco::Logger::create("AVSTREAM", pChannel, Poco::Message::PRIO_DEBUG);
//     _pAvStreamLogger = &Poco::Logger::create("AVSTREAM", pChannel, Poco::Message::PRIO_INFORMATION);
//     _pAvStreamLogger = &Poco::Logger::create("AVSTREAM", pChannel, Poco::Message::PRIO_ERROR);
}


Log*
Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Log::avstream()
{
    return *_pAvStreamLogger;
}


RingBuffer::RingBuffer(int size) :
_ringBuffer(new char[size]),
_readPos(_ringBuffer),
_writePos(_ringBuffer),
_size(size)
{
}


RingBuffer::~RingBuffer()
{
    delete _ringBuffer;
}


void
RingBuffer::read(char* buffer, int num)
{
    int relReadPos = _readPos - _ringBuffer;
    if (relReadPos + num >= _size) {
        int firstHalf = _size - relReadPos;
        int secondHalf = num - firstHalf;
        memcpy(buffer, _readPos, firstHalf);
        memcpy(buffer + firstHalf, _ringBuffer, secondHalf);
        _readPos = _ringBuffer + secondHalf;
    }
    else {
        memcpy(buffer, _readPos, num);
        _readPos += num;
    }
}


void
RingBuffer::write(const char* buffer, int num)
{
    int relWritePos = _writePos - _ringBuffer;
    if (relWritePos + num >= _size) {
        int firstHalf = _size - relWritePos;
        int secondHalf = num - firstHalf;
        memcpy(_writePos, buffer, firstHalf);
        memcpy(_ringBuffer, buffer + firstHalf, secondHalf);
        _writePos = _ringBuffer + secondHalf;
    }
    else {
        memcpy(_writePos, buffer, num);
        _writePos += num;
    }
}


void
RingBuffer::clear()
{
    _readPos = _ringBuffer;
    _writePos = _ringBuffer;
}


ByteQueue::ByteQueue(int size) :
_ringBuffer(size),
_size(size),
_level(0)
{
}


void
ByteQueue::read(char* buffer, int num)
{
    int bytesRead = 0;
    while (bytesRead < num) {
        bytesRead += readSome(buffer + bytesRead, num - bytesRead);
    }
}


void
ByteQueue::write(const char* buffer, int num)
{
    Log::instance()->avstream().trace("byte queue write, num bytes: " + Poco::NumberFormatter::format(num));
    int bytesWritten = 0;
    while (bytesWritten < num) {
        Log::instance()->avstream().trace("byte queue write -> writeSome, trying to write: " + Poco::NumberFormatter::format(num - bytesWritten) + " bytes");
        bytesWritten += writeSome(buffer + bytesWritten, num - bytesWritten);
    }
    Log::instance()->avstream().trace("byte queue write finished.");
}


int
ByteQueue::readSome(char* buffer, int num)
{
    _lock.lock();
    if (_level == 0) {
        Log::instance()->avstream().trace("byte queue readSome() try to read " + Poco::NumberFormatter::format(num) + " bytes, level: " + Poco::NumberFormatter::format(_level));
        // block byte queue for further reading
        _readCondition.wait<Poco::FastMutex>(_lock);
        Log::instance()->avstream().trace("byte queue readSome() wait over, now reading " + Poco::NumberFormatter::format(num) + " bytes, level: " + Poco::NumberFormatter::format(_level));
    }

    int bytesRead = (_level < num) ? _level : num;
    _ringBuffer.read(buffer, bytesRead);
    _level -= bytesRead;

    Log::instance()->avstream().trace("byte queue readSome() read " + Poco::NumberFormatter::format(bytesRead) + " bytes, level: " + Poco::NumberFormatter::format(_level));

    // we've read some bytes, so we can put something in, again
    if (_level < _size) {
        _writeCondition.broadcast();
    }
    _lock.unlock();
    return bytesRead;
}


int
ByteQueue::writeSome(const char* buffer, int num)
{
    _lock.lock();
    if (_level == _size) {
        // block byte queue for further writing
        Log::instance()->avstream().trace("byte queue writeSome() try to write " + Poco::NumberFormatter::format(num) + " bytes, level: " + Poco::NumberFormatter::format(_level));
        _writeCondition.wait<Poco::FastMutex>(_lock);
        Log::instance()->avstream().trace("byte queue writeSome() wait over, now writing " + Poco::NumberFormatter::format(num) + " bytes, level: " + Poco::NumberFormatter::format(_level));
    }

    int bytesWritten = (_size - _level < num) ? (_size - _level) : num;
    _ringBuffer.write(buffer, bytesWritten);
    _level += bytesWritten;

    Log::instance()->avstream().trace("byte queue writeSome() wrote " + Poco::NumberFormatter::format(bytesWritten) + " bytes, level: " + Poco::NumberFormatter::format(_level));

    // we've written some bytes, so we can get something out, again
    if (_level > 0) {
        _readCondition.broadcast();
    }
    _lock.unlock();
    return bytesWritten;
}


int
ByteQueue::size()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    return _size;
}


int
ByteQueue::level()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    return _level;
}


void
ByteQueue::clear()
{
    Log::instance()->avstream().trace("byte queue clear");
    char buf[_level];
    read(buf, _level);
}


bool
ByteQueue::full()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    Log::instance()->avstream().trace("byte queue check full() at level: " + Poco::NumberFormatter::format(_level));
    return (_level == _size);
}


bool
ByteQueue::empty()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    Log::instance()->avstream().trace("byte queue check empty() at level: " + Poco::NumberFormatter::format(_level));
    return (_level == 0);
}


// TODO: queue name should contain the stream name, too.
StreamQueue::StreamQueue(Node* pNode, int size) :
Queue<Frame*>(pNode->getName() + " stream", size),
_pNode(pNode)
{
}


Node*
StreamQueue::getNode()
{
    return _pNode;
}


StreamInfo::StreamInfo(const std::string name) :
_streamName(name),
_newFrameNumber(0)
{
}


StreamInfo::~StreamInfo()
{
}


std::string
StreamInfo::getName()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    return _streamName;
}


void
StreamInfo::setName(const std::string& name)
{
    _streamName = name;
}


void
StreamInfo::printInfo()
{
    Log::instance()->avstream().debug("stream width: " + Poco::NumberFormatter::format(width()) + ", height: " +
        Poco::NumberFormatter::format(height()));
}


Poco::Int64
StreamInfo::newFrameNumber()
{
    return ++_newFrameNumber;
}


Stream::Stream(Node* pNode) :
_pNode(pNode),
_pStreamInfo(0),
_pStreamQueue(0)
{
}


Stream::~Stream()
{
    // StreamQueues belong to the nodes that are connected downstream
    // so they are not deleted here
}


Frame*
Stream::firstFrame()
{
    if (!_pStreamQueue) {
        Log::instance()->avstream().warning(getNode()->getName() + " [" + getInfo()->getName() + "] get frame no queue attached, ignoring");
        return 0;
    }
    else {
        Frame* pRes = _pStreamQueue->front();
        if (!pRes) {
            Log::instance()->avstream().warning(getNode()->getName() + " [" + getInfo()->getName() + "] first frame is null frame.");
        }
        else {
            Log::instance()->avstream().trace(getNode()->getName() + " [" + getInfo()->getName() + "] first frame " + pRes->getName() + ".");
        }
        return pRes;
    }
}


Frame*
Stream::getFrame()
{
    if (!_pStreamQueue) {
        Log::instance()->avstream().trace("getFrame() no stream queue");
        Log::instance()->avstream().warning(getNode()->getName() + " [" + getInfo()->getName() + "] get frame no queue attached, ignoring");
        return 0;
    }
    else {
        Frame* pRes = _pStreamQueue->get();
        if (!pRes) {
            Log::instance()->avstream().trace("getFrame() null frame");
            Log::instance()->avstream().warning(getNode()->getName() + " [" + getInfo()->getName() + "] get frame returns null frame.");
        }
        else {
            Log::instance()->avstream().trace(getNode()->getName() + " [" + getInfo()->getName() + "] get frame " + pRes->getName() + ", queue size: " +
                Poco::NumberFormatter::format(_pStreamQueue->count()));
        }
        return pRes;
    }
}


void
Stream::putFrame(Frame* pFrame)
{
    if (!pFrame) {
        Log::instance()->avstream().warning(getNode()->getName() + " [" + getInfo()->getName() + "] put null frame, discarding frame");
    }
    else if (!_pStreamQueue) {
        Log::instance()->avstream().warning(getNode()->getName() + " [" + getInfo()->getName() + "] put frame " + pFrame->getName() + " no queue attached, discarding frame");
    }
    else {
        Log::instance()->avstream().trace(getNode()->getName() + " [" + getInfo()->getName() + "] put frame " + pFrame->getName() + ", queue size: " +
            Poco::NumberFormatter::format(_pStreamQueue->count()));
        _pStreamQueue->put(pFrame);
    }
}


StreamInfo*
Stream::getInfo()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    return _pStreamInfo;
}


StreamQueue*
Stream::getQueue()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    return _pStreamQueue;
}


void
Stream::setInfo(StreamInfo* pInfo)
{
    _pStreamInfo = pInfo;
}


void
Stream::setQueue(StreamQueue* pQueue)
{
    _pStreamQueue = pQueue;
}


void
Stream::setNode(Node* pNode)
{
    _pNode = pNode;
}


Node*
Stream::getNode()
{
    return _pNode;
}


Frame*
StreamInfo::decodeFrame(Frame* pFrame)
{
    if (!pFrame || !pFrame->data()) {
        Log::instance()->avstream().warning("input frame broken while decoding stream " + getName() + ", discarding frame.");
        return 0;
    }

    Frame* pDecodedFrame = 0;
    if (isAudio()) {
        pDecodedFrame = decodeAudioFrame(pFrame);
    }
    else if (isVideo()) {
        pDecodedFrame = decodeVideoFrame(pFrame);
    }
    if (pDecodedFrame) {
        pDecodedFrame->setNumber(pFrame->getNumber());
        pDecodedFrame->setPts(pFrame->getPts());
    }
    // packet is decoded and not needed anymore, we can free it.
    delete pFrame;
    // and return the decoded frame (which always points to the allocated audio/video buffer provided by the stream).
    return pDecodedFrame;
}


Node::Node(const std::string& name) :
_name(name),
_quit(false)
{
    _thread.setName(name);
}


std::string
Node::getName()
{
//     Log::instance()->avstream().debug("Node::getName()");
    Poco::ScopedLock<Poco::FastMutex> lock(_nameLock);
    return _name;
}


void
Node::setName(const std::string& name)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_nameLock);
    _name = name;
    _thread.setName(name);
}


void
Node::start()
{
    Log::instance()->avstream().debug(getName() + " starting run thread ...");

    // first start all nodes downstream, so they can begin sucking frames ...
    int outStreamNumber = 0;
    for (std::vector<Stream*>::iterator it = _outStreams.begin(); it != _outStreams.end(); ++it, ++outStreamNumber) {
        if (!(*it)) {
            Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not start downstream node, no stream attached");
            continue;
        }
        Node* downstreamNode = getDownstreamNode(outStreamNumber);
        if (!downstreamNode) {
            Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not start downstream node, no node attached");
            continue;
        }
        downstreamNode->start();
    }
    // then start this node. If this node would be started before the downstream node,
    // all frames processed in this node would be discarded before the downstream nodes could process them.
    if (!_thread.isRunning()) {
        _thread.start(*this);
    }
}


void
Node::stop()
{
    Log::instance()->avstream().debug(getName() + " stopping run thread ...");

    initiateStop();
    waitForStop();

    Log::instance()->avstream().debug(getName() + " run thread stopped.");
}


void
Node::initiateStop()
{
    Log::instance()->avstream().debug(getName() + " trying to stop run thread ...");
    // first stop this node by setting the _quit flag ...
    setStop(true);

    // then stop all nodes downstream
    int outStreamNumber = 0;
    for (std::vector<Stream*>::iterator it = _outStreams.begin(); it != _outStreams.end(); ++it, ++outStreamNumber) {
        if (!(*it)) {
            Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not stop downstream node, no stream attached");
            continue;
        }
        Node* downstreamNode = getDownstreamNode(outStreamNumber);
        if (!downstreamNode) {
            Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not stop downstream node, no node attached");
            continue;
        }
        downstreamNode->initiateStop();
        if ((*it)->getQueue()->full()) {
            Log::instance()->avstream().debug(getName() + " out stream queue full while stopping node, getting frame");
            delete (*it)->getFrame();
        }
        else if ((*it)->getQueue()->empty()){
            Log::instance()->avstream().debug(getName() + " out stream queue empty while stopping node, putting flush frame");
            (*it)->putFrame(new Frame(0, (*it)->getInfo(), true));
        }
    }
}


void
Node::waitForStop()
{
    Log::instance()->avstream().debug(getName() + " trying to join run thread ...");
    try {
        // wait for run thread loop to finish
        // FIXME: what, if the thread finishes after _thread.isRunning() and _thread.join()
        if (_thread.isRunning()) {
            _thread.join(500);
        }
    }
    catch(...) {
        Log::instance()->avstream().warning(getName() + " failed to cleanly shutdown run thread");
    }
    setStop(false);
    Log::instance()->avstream().debug(getName() + " run thread joined.");

    // then wait for all nodes downstream to stop
    int outStreamNumber = 0;
    for (std::vector<Stream*>::iterator it = _outStreams.begin(); it != _outStreams.end(); ++it, ++outStreamNumber) {
        if (!(*it)) {
            Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not wait for downstream node to stop, no stream attached");
            continue;
        }
        Node* downstreamNode = getDownstreamNode(outStreamNumber);
        if (!downstreamNode) {
            Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not wait for downstream node to stop, no node attached");
            continue;
        }
        downstreamNode->waitForStop();
    }
}


void
Node::attach(Node* node, int outStreamNumber, int inStreamNumber)
{
    if (!node) {
        Log::instance()->avstream().warning(getName() + ": could not attach null node");
        return;
    }

    // entwine StreamInfo and StreamQueue of inStream and outStream
    // TODO: improve warning messages
    if (!_outStreams[outStreamNumber]) {
        Log::instance()->avstream().warning(getName() + "%s: could not attach ..., output stream invalid");
        return;
    }
    if (!_outStreams[outStreamNumber]->getInfo()) {
        Log::instance()->avstream().warning(getName() + ": could not attach ..., output stream has no info");
        return;
    }
    if (!node->_inStreams[inStreamNumber]) {
        Log::instance()->avstream().warning(getName() + ": could not attach ..., downstream node input stream invalid");
        return;
    }
    if (!node->_inStreams[inStreamNumber]->getQueue()) {
        Log::instance()->avstream().warning(getName() + ": could not attach ..., downstream node input stream has no queue");
        return;
    }

    _outStreams[outStreamNumber]->setQueue(node->_inStreams[inStreamNumber]->getQueue());
    node->_inStreams[inStreamNumber]->setInfo(_outStreams[outStreamNumber]->getInfo());

    Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] attached node " + node->getName() + " [" + Poco::NumberFormatter::format(inStreamNumber) + "]");

    Log::instance()->avstream().debug("calling init() of " + node->getName());
    bool initSuccess = node->init();
    if (initSuccess) {
        Log::instance()->avstream().debug(node->getName() + " init success.");
    }
    else {
        Log::instance()->avstream().warning(node->getName() + " init failed, start of node canceled.");
        setStop(true);
    }
}


void
Node::detach(int outStreamNumber)
{
    if (outStreamNumber >= _outStreams.size()) {
        Log::instance()->avstream().error(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not detach node, stream number to high");
        return;
    }
    Log::instance()->avstream().debug(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] detached node " + _outStreams[outStreamNumber]->getQueue()->getNode()->getName());

    // lazy strategie: only set queue of outStream to 0, so no frames are put into the queue anymore
    // the attached node empties the queue with a valid stream context (StreamInfo) and blocks
    // without being connected to anything
    _outStreams[outStreamNumber]->setQueue(0);
}


Node*
Node::getDownstreamNode(int outStreamNumber)
{
//     Poco::ScopedLock<Poco::FastMutex> lock(_lock);
    if (outStreamNumber >= _outStreams.size()) {
        Log::instance()->avstream().error(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not get downstream node, stream number to high");
        return 0;
    }
    Stream* outStream = _outStreams[outStreamNumber];
    if (!outStream || !outStream->getQueue()) {
        Log::instance()->avstream().warning(getName() + " [" + Poco::NumberFormatter::format(outStreamNumber) + "] could not get downstream node, no node attached to output stream.");
        return 0;
    }
    return outStream->getQueue()->getNode();
}


void
Node::setStop(bool stop)
{
    _quitLock.lock();
    _quit = stop;
    _quitLock.unlock();
}


bool
Node::getStop()
{
//     Poco::ScopedLock<Poco::FastMutex> lock(_quitLock);
    bool stop = false;
    _quitLock.lock();
    stop = _quit;
    _quitLock.unlock();
    return stop;
}


Stream*
Node::getInStream(int inStreamNumber)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_inStreamsLock);
    return _inStreams[inStreamNumber];
}


Stream*
Node::getOutStream(int outStreamNumber)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_outStreamsLock);
    return _inStreams[outStreamNumber];
}


void
Node::reset()
{
    Omm::AvStream::Log::instance()->avstream().debug("node reset ...");
    setStop(false);
    for (std::vector<Stream*>::iterator it = _inStreams.begin(); it != _inStreams.end(); ++it) {
        Queue<Frame*>* pQueue = (*it)->getQueue();
        if (pQueue) {
            while (pQueue->count()) {
                Frame* pFrame = pQueue->get();
                // FIXME: random segfaults deleting last video frame of VideoSink queue (reading from std::istream, reason still unknown ...)
                // ... this leaves a memory hole.
                if (pFrame && pQueue->count()) {
                    delete pFrame;
                }
            }
        }
    }
    Omm::AvStream::Log::instance()->avstream().debug("node reset finished.");
}


Frame::Frame(Poco::Int64 number, StreamInfo* pStreamInfo, bool endOfStream) :
_number(number),
_pts(Meta::invalidPts),
_pStreamInfo(pStreamInfo),
_endOfStream(endOfStream)
{
}


Frame::~Frame()
{
}


bool
Frame::isEndOfStream()
{
    return _endOfStream;
}


std::string
Frame::getName()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_nameLock);
    return "["
        + getStreamInfo()->getName() + "] "
        + "#" + Poco::NumberFormatter::format(getNumber()) + ", pts: "
        + (_pts == Meta::invalidPts ? "invalid pts" : Poco::NumberFormatter::format(getPts()));
//         + ", duration: " + Poco::NumberFormatter::format(_duration);
}


const Poco::Int64
Frame::getNumber()
{
//     Poco::ScopedLock<Poco::Mutex> lock(_numberLock);
    return _number;
}


void
Frame::setNumber(Poco::Int64 frameNumber)
{
//     Poco::ScopedLock<Poco::Mutex> lock(_numberLock);
    _number = frameNumber;
}


const Poco::Int64
Frame::getPts()
{
//     Poco::ScopedLock<Poco::Mutex> lock(_ptsLock);
    return _pts;
}


void
Frame::setPts(Poco::Int64 pts)
{
//     Poco::ScopedLock<Poco::Mutex> lock(_ptsLock);
    _pts = pts;
}


StreamInfo*
Frame::getStreamInfo()
{
    return _pStreamInfo;
}


void
Frame::writePpm(const std::string& fileName)
{
    Log::instance()->avstream().debug("write video frame to PPM file name: " + fileName);

    Frame* pRes = convert(Meta::CC_RGB24);

    std::ofstream ppmFile(fileName.c_str());
    // write PPM header
    ppmFile << "P6\n" << getStreamInfo()->width() << " " << getStreamInfo()->height() << "\n" << 255 << "\n";
    // write RGB pixel data
    ppmFile.write((const char*)pRes->data(), getStreamInfo()->width() * getStreamInfo()->height() * 3);
}


Demuxer::Demuxer() :
Node("demuxer"),
_pMeta(0),
_firstAudioStream(-1),
_firstVideoStream(-1)
{
//     // Initialize libavformat and register all the muxers, demuxers and protocols
//     Log::instance()->ffmpeg().trace("ffmpeg::av_register_all() ...");
//     av_register_all();
}


void
Demuxer::set(Meta* pMeta)
{
    _pMeta = pMeta;
    if (!_pMeta) {
        Log::instance()->avstream().warning("demuxer could not tag input stream, giving up.");
        setStop(true);
    }
    else {
        _pMeta->print();
        if (!init()) {
            Log::instance()->avstream().warning("demuxer init failed, giving up.");
            setStop(true);
        }
    }
}


bool
Demuxer::init()
{
    // init the demuxer streams
    int audioStreamCount = 0;
    int videoStreamCount = 0;
    for(int streamNr = 0; streamNr < _pMeta->numberStreams(); streamNr++) {
        //////////// allocate streams ////////////
        Stream* pStream = new Stream(this);
        _outStreams.push_back(pStream);
        pStream->setInfo(_pMeta->streamInfo(streamNr));
//         pStream->setNode(this);
        pStream->setQueue(0);

        //////////// find first audio and video stream ////////////
        if (pStream->getInfo()->isAudio()) {
            Log::instance()->avstream().information("found audio stream #" + Poco::NumberFormatter::format(streamNr));
            pStream->getInfo()->setName("audio" + Poco::NumberFormatter::format(audioStreamCount++));
            if (_firstAudioStream < 0) {
                _firstAudioStream = streamNr;
            }
        }
        else if (pStream->getInfo()->isVideo()) {
            Log::instance()->avstream().information("found video stream #" + Poco::NumberFormatter::format(streamNr));
            pStream->getInfo()->setName("video" + Poco::NumberFormatter::format(videoStreamCount++));
            if (_firstVideoStream < 0) {
                _firstVideoStream = streamNr;
            }
        }
        else {
            Log::instance()->avstream().information("found unknown stream #" + Poco::NumberFormatter::format(streamNr));
        }
    }
    return true;
}


void
Demuxer::reset()
{
    Log::instance()->avstream().debug("demuxer reset ...");
    if (_pMeta) {
        delete _pMeta;
    }
    _pMeta = 0;

    for(std::vector<Stream*>::iterator it = _outStreams.begin(); it != _outStreams.end(); ++it) {
        delete *it;
    }
    _outStreams.clear();
    _firstAudioStream = -1;
    _firstVideoStream = -1;
    Node::reset();
    Log::instance()->avstream().debug("demuxer reset finished.");
}


void
Demuxer::run()
{
    Omm::AvStream::Log::instance()->avstream().debug(getName() + " run thread started.");

    std::vector<Poco::Int64> lastPtsVec(_outStreams.size(), Meta::invalidPts);
    std::vector<int> lastDurationVec(_outStreams.size(), 0);

    while (!getStop()) {
        Frame* pFrame = _pMeta->readFrame();
        int streamIndex = pFrame->streamIndex();

        if (pFrame->isEndOfStream()) {
            for(std::vector<Stream*>::iterator it = _outStreams.begin(); it != _outStreams.end(); ++it) {
                (*it)->putFrame(pFrame);
            }
            break;
        }

        if (!_outStreams[streamIndex]->getQueue()) {
            Log::instance()->avstream().warning(getName() + " stream " + Poco::NumberFormatter::format(streamIndex) + " not connected, discarding packet");
            delete pFrame;
            continue;
        }
        // pass a reference to the stream to where this frame belongs (for further use of the codec context when decoding)
        // create a new frame out of the AVPacket that we extracted from the stream
        // create means: make a copy of the data buffer and delete the packet
//         Frame* pFrame = new Frame(frameNumber, _outStreams[packet.stream_index], &packet);
//         pFrame->setPts(packet.pts);

        // try to correct wrong packet.pts
        // this relies on a correct packet.duration
        // (pts or duration, one of them must be right, otherwise we are lost, especially with variable frame rate)
        Poco::Int64 currentPts = pFrame->getPts();
        Poco::Int64 correctedPts = correctPts(currentPts, lastPtsVec[streamIndex], lastDurationVec[streamIndex]);
        if (correctedPts != currentPts) {
            Log::instance()->avstream().warning(getName() + " frame " + pFrame->getName() + ", correct pts " + Poco::NumberFormatter::format(currentPts) + " -> " + Poco::NumberFormatter::format(correctedPts) + ", delta: " +
                Poco::NumberFormatter::format((correctedPts - currentPts)));
        }
        pFrame->setPts(correctedPts);
        lastPtsVec[streamIndex] = correctedPts;
        lastDurationVec[streamIndex] = pFrame->duration();

        _outStreams[streamIndex]->putFrame(pFrame);
    }
    Omm::AvStream::Log::instance()->avstream().debug(getName() + " run thread finished.");
}


int
Demuxer::firstAudioStream()
{
    return _firstAudioStream;
}


int
Demuxer::firstVideoStream()
{
    return _firstVideoStream;
}


Poco::Int64
Demuxer::correctPts(Poco::Int64 pts, Poco::Int64 lastPts, int lastDuration)
{
    // OK, this is the place for black magic.
    // Everything would be fine, if pts and duration
    // would be in perfect harmony and pts would always be monotone ...
    // but this isn't the case, and we rely on two assumptions here:
    // 1. all frames are in correct order
    // 2. duration of frames is correct
    //
    //     In general, there seem to be "seven symptoms of sick streams"
    //
    //     pts related (all corrected in synchronizer):
    //     1a. pts number overflow
    //     ->  set flag Frame::numberOverflow = true
    //     1b. pts jump in one stream, and not the other
    //     ->  ignore and discard/inject frames as appropriate
    //     2. pts cicle (pts systematic not monotone)
    //     -> reorder pts
    //     3. pts sprite (pts local not monotone)
    //     -> "tear" sprite pts linear
    //     4. packet shift
    //     -> discard packets until max_of_streams(pts first packet of stream)
    //     5. packet loss (pts not linear) (-> 1b pts jump)
    //     -> inject blank frame (set Frame::_display = false)
    //
    //     not pts related:
    //     6. decoding error
    //     -> set flag Frame::_display = false
    //     7. clock drift
    //     -> correct drift or one clock source (watch for cumulating drift when using timers)
    //

    Poco::Int64 correctedPts;

    if (lastPts != Meta::invalidPts && pts != Meta::invalidPts) {
        if (lastDuration > 0 && pts != lastPts + lastDuration) {
            correctedPts = lastPts + lastDuration;
        }
        else {
            correctedPts = pts;
        }

    }
    else if (lastPts == Meta::invalidPts && pts != Meta::invalidPts) {
        correctedPts = pts;
    }
    else if (pts == Meta::invalidPts && lastPts != Meta::invalidPts) {
        if (lastDuration > 0) {
            correctedPts = lastPts + lastDuration;
        }
        else {
            correctedPts = pts;
        }
    }
    else { // lastPts && packet.pts == Meta::invalidPts
        correctedPts = pts;
    }

    return correctedPts;
}


// Muxer::Muxer() :
// Node("muxer"),
// _pMeta(new Meta),
// _uri("mux.ts")
// {
//     // muxer has two input streams
//     _inStreams.push_back(new Stream(this));
//     _inStreams[0]->setInfo(0);
//     _inStreams[0]->setQueue(new StreamQueue(this));
//
//     _inStreams.push_back(new Stream(this));
//     _inStreams[1]->setInfo(0);
//     _inStreams[1]->setQueue(new StreamQueue(this));
//
//     // and no output stream
//
//     Omm::AvStream::Log::instance()->ffmpeg().debug("ffmpeg::avformat_alloc_context()");
//     _pMeta->_pFormatContext = avformat_alloc_context();
// }


Muxer::~Muxer()
{
    delete _pMeta;
}


// void
// Muxer::set(const std::string& uri, const std::string& format)
// {
//     _uri = uri;
//     Omm::AvStream::Log::instance()->ffmpeg().debug("ffmpeg::guess_format()");
//     _pMeta->_pFormatContext->oformat = guess_format(format.c_str(), NULL, NULL);
//     // TODO: check for max uri length supported by ffmpeg (currently 1024 ...?)
//     _uri.copy(_pMeta->_pFormatContext->filename, _uri.size());
//     _pMeta->_pFormatContext->filename[_uri.size()] = '\0';
//
//     if (av_set_parameters(_pMeta->_pFormatContext, 0) < 0) {
//         Omm::AvStream::Log::instance()->avstream().error("invalid output format parameters");
//         return;
//     }
// }


void
Muxer::set(std::ostream& ostr, const std::string& format)
{
    // TODO: implement Muxer::set() for std::ostream
}


void
Muxer::reset()
{
    if (_pMeta) {
        delete _pMeta;
    }
    _pMeta = 0;
}


// bool
// Muxer::prepareOutStream(int streamNumber)
// {
//     if (!_inStreams[streamNumber]->getInfo()->findCodec()) {
//         Omm::AvStream::Log::instance()->avstream().warning(getName() + " init failed, could not find codec");
//         return false;
//     }
//
//     _inStreams[streamNumber]->setInfo(_inStreams[streamNumber]->getInfo()->cloneOutStreamInfo(_pMeta, streamNumber));
//
//     if (!_inStreams[streamNumber]->getInfo()->findEncoder()) {
//         Omm::AvStream::Log::instance()->avstream().warning(getName() + " init failed, could not find encoder");
//         return false;
//     }
//     return true;
// }


// bool
// Muxer::init()
// {
//     if (!_inStreams[0]->getInfo() && !_inStreams[1]->getInfo()) {
//         Log::instance()->avstream().warning(getName() + " init failed, two input streams must be connected");
//         return false;
//     }
//
//     if (_inStreams[0]->getInfo() && _inStreams[1]->getInfo()) {
//         if (!prepareOutStream(0)) {
//             return false;
//         }
//         if (!prepareOutStream(1)) {
//             return false;
//         }
//
//         _pMeta->print(true);
//
//         Omm::AvStream::Log::instance()->ffmpeg().debug("ffmpeg::url_fopen()");
//         url_fopen(&_pMeta->_pFormatContext->pb, _uri.c_str(), URL_WRONLY);
//
//         Omm::AvStream::Log::instance()->ffmpeg().debug("ffmpeg::av_write_header()");
//         av_write_header(_pMeta->_pFormatContext);
//     }
//
//     return true;
// }


// void
// Muxer::run()
// {
//     Omm::AvStream::Log::instance()->avstream().debug(getName() + " run thread started.");
//
//     while (!getStop()) {
//         // FIXME: handle packet number wrap in muxer
//         Frame* pFirstStreamFrame = _inStreams[0]->getFrame();
//         Frame* pSecondStreamFrame = _inStreams[1]->getFrame();
//
//         while (pSecondStreamFrame->getNumber() < pFirstStreamFrame->getNumber())
//         {
//             // FIXME: segfault (floating point exception) in av_write_frame()
//             Log::instance()->avstream().debug(getName() + " write frame " + pSecondStreamFrame->getName());
//             pSecondStreamFrame->_pAvPacket->stream_index = 1;
//             Log::instance()->ffmpeg().trace("ffmpeg::av_write_frame() ...");
//             av_write_frame(_pMeta->_pFormatContext, pSecondStreamFrame->_pAvPacket);
// //             delete pSecondStreamFrame;
//             pSecondStreamFrame = _inStreams[1]->getFrame();
//         }
//
//         while (pFirstStreamFrame->getNumber() < pSecondStreamFrame->getNumber())
//         {
//             Log::instance()->avstream().debug(getName() + " write frame " + pFirstStreamFrame->getName());
//             pFirstStreamFrame->_pAvPacket->stream_index = 0;
//             Log::instance()->ffmpeg().trace("ffmpeg::av_write_frame() ...");
//             av_write_frame(_pMeta->_pFormatContext, pFirstStreamFrame->_pAvPacket);
// //             delete pFirstStreamFrame;
//             pFirstStreamFrame = _inStreams[0]->getFrame();
//         }
//     }
//     Log::instance()->ffmpeg().trace("ffmpeg::av_write_trailer() ...");
//     av_write_trailer(_pMeta->_pFormatContext);
// }


const Poco::Int64 Meta::invalidPts = INT64_C(0x8000000000000000);

Meta::Meta() :
_isPlaylist(false)
{
}


Meta::~Meta()
{
    for (std::vector<StreamInfo*>::iterator it = _streamInfos.begin(); it != _streamInfos.end(); ++it) {
        if (*it) {
            delete *it;
        }
    }
//     _streamInfos.clear();
//     _tags.clear();
}


Meta::ContainerFormat
Meta::getContainerFormat()
{
    if (_isPlaylist) {
        return Omm::AvStream::Meta::CF_PLAYLIST;
    }
    bool hasAudio = false;
    bool hasVideo = false;
    for (int i = 0; i < numberStreams(); i++) {
        if (streamInfo(i)->isAudio()) {
            hasAudio = true;
        }
        if (streamInfo(i)->isVideo()) {
            hasVideo = true;
        }
    }
    if (!hasVideo && hasAudio) {
        return Omm::AvStream::Meta::CF_AUDIO;
    }
    else if (hasVideo) {
        if (isStillImage()) {
            return Omm::AvStream::Meta::CF_IMAGE;
        }
        else {
            return Omm::AvStream::Meta::CF_VIDEO;
        }
    }
    else {
        return Omm::AvStream::Meta::CF_UNKNOWN;
    }
}


std::string
Meta::getTag(TagKey key)
{
    // for a comparison of tags, see http://age.hobba.nl/audio/tag_frame_reference.html
    // id3 tags: http://www.id3.org/id3v2.4.0-frames
    std::string res;
    switch (key) {
        case TK_TITLE:
            res = getTag("TIT2", "TT2", "title");
            break;
        case TK_ALBUM:
            res = getTag("TALB", "TAL", "TOAL", "album");
            break;
        case TK_ARTIST:
            res = getTag("TPE1", "TP1", "artist");
            break;
        case TK_GENRE:
            res = getTag("TCON", "TCO", "genre");
            break;
        case TK_TRACK:
            res = getTag("TRCK", "TRK", "track", "TRACKNUMBER", "PART_NUMBER");
            break;
    }
    return res;
}


std::string
Meta::getTag(const std::string& key1)
{
    if (_tags[key1] != "") {
        return _tags[key1];
    }
    else {
        return "";
    }
}


std::string
Meta::getTag(const std::string& key1, const std::string& key2)
{
    if (_tags[key1] != "") {
        return _tags[key1];
    }
    else if (_tags[key2] != "") {
        return _tags[key2];
    }
    else {
        return "";
    }
}


std::string
Meta::getTag(const std::string& key1, const std::string& key2, const std::string& key3)
{
    if (_tags[key1] != "") {
        return _tags[key1];
    }
    else if (_tags[key2] != "") {
        return _tags[key2];
    }
    else if (_tags[key3] != "") {
        return _tags[key3];
    }
    else {
        return "";
    }
}


std::string
Meta::getTag(const std::string& key1, const std::string& key2, const std::string& key3, const std::string& key4)
{
    if (_tags[key1] != "") {
        return _tags[key1];
    }
    else if (_tags[key2] != "") {
        return _tags[key2];
    }
    else if (_tags[key3] != "") {
        return _tags[key3];
    }
    else if (_tags[key4] != "") {
        return _tags[key4];
    }
    else {
        return "";
    }
}


std::string
Meta::getTag(const std::string& key1, const std::string& key2, const std::string& key3, const std::string& key4, const std::string& key5)
{
    if (_tags[key1] != "") {
        return _tags[key1];
    }
    else if (_tags[key2] != "") {
        return _tags[key2];
    }
    else if (_tags[key3] != "") {
        return _tags[key3];
    }
    else if (_tags[key4] != "") {
        return _tags[key4];
    }
    else if (_tags[key5] != "") {
        return _tags[key5];
    }
    else {
        return "";
    }
}


void
Meta::setTag(const std::string& key, const std::string& value)
{
    _tags[key] = value;
}


int
Meta::numberStreams()
{
    return _streamInfos.size();
}


void
Meta::addStream(StreamInfo* pStreamInfo)
{
    _streamInfos.push_back(pStreamInfo);
}


StreamInfo*
Meta::streamInfo(int streamNumber)
{
    return _streamInfos[streamNumber];
}


void
Meta::setIsPlaylist(bool isPlaylist)
{
    _isPlaylist = isPlaylist;
}


Sink::Sink(const std::string& name) :
Node(name),
_timeQueue(name + " timer", 10),
_firstDecodeSuccess(false)
{
    // sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams[0]->setInfo(0);
    _inStreams[0]->setQueue(new Omm::AvStream::StreamQueue(this));

    // and no output stream
}


bool
Sink::init()
{
    if (!_inStreams[0]->getInfo()) {
        Omm::AvStream::Log::instance()->avstream().warning(getName() + " init failed, input stream info not allocated");
        return false;
    }
    if (!_inStreams[0]->getInfo()->findCodec()) {
        Omm::AvStream::Log::instance()->avstream().warning(getName() + " init failed, could not find codec");
        return false;
    }

    return checkInStream() && initDevice();
}


void
Sink::run()
{
    Log::instance()->avstream().debug(getName() + " run thread started.");

    if (!_inStreams[0]->getQueue()) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to video sink, stopping.");
        return;
    }
    Omm::AvStream::Frame* pFrame;
    Log::instance()->avstream().debug(getName() + " run thread looping ...");
    while ((pFrame = _inStreams[0]->getFrame()) && !getStop()) {
        if (pFrame->isEndOfStream()) {
            Log::instance()->avstream().debug(getName() + " run thread got end of stream frame, bailing out ...");
            _streamEventNotifier.postNotification(new EndOfStream);
            break;
        }
        Omm::AvStream::Frame* pDecodedFrame = _inStreams[0]->getInfo()->decodeFrame(pFrame);
        if (!pDecodedFrame) {
            Omm::AvStream::Log::instance()->avstream().warning(getName() + " decoding failed, discarding frame");
        }
        else {
            if (!_firstDecodeSuccess) {
                _firstDecodeSuccess = true;
                _startTime = pDecodedFrame->getPts();
                _timeQueue.put(_startTime);
                Omm::AvStream::Log::instance()->avstream().debug(getName() + " start time is " +
                    Poco::NumberFormatter::format(_startTime));
            }
            writeDecodedFrame(pDecodedFrame);
        }
    }
    Omm::AvStream::Log::instance()->avstream().debug(getName() + " run thread finished.");
}


void
Sink::reset()
{
    Omm::AvStream::Log::instance()->avstream().debug("sink reset ...");
    closeDevice();
    _firstDecodeSuccess = false;
    _startTime = Meta::invalidPts;
    _timeQueue.clear();
    Node::reset();
    Omm::AvStream::Log::instance()->avstream().debug("sink reset finished.");
}


void
Sink::currentTime(Poco::Int64 time)
{
    Log::instance()->avstream().trace(getName() + " received current time: " + Poco::NumberFormatter::format(time));

    _timeQueue.put(time);
}


void
Sink::registerStreamEventObserver(const Poco::AbstractObserver* pObserver)
{
    _streamEventNotifier.addObserver(*pObserver);
}


AudioSink::AudioSink(const std::string& name) :
Sink(name),
// allocate byte queue for 20k s16-2chan-samples
_byteQueue(20 * 1024 * 2 * 2),
_volume(0.5),
_pClock(0)
{
}


AudioSink::~AudioSink()
{
}


bool
AudioSink::checkInStream()
{
    if (!_inStreams[0]->getInfo()->isAudio()) {
        Omm::AvStream::Log::instance()->avstream().warning(getName() + " init failed, input stream is not an audio stream");
        return false;
    }

    return true;
}


void
AudioSink::writeDecodedFrame(Frame* pDecodedFrame)
{
    Omm::AvStream::Log::instance()->avstream().trace(getName() + " writing decoded audio frame of size " + Poco::NumberFormatter::format(pDecodedFrame->size()) + " to byte queue ...");
    _byteQueue.write(pDecodedFrame->data(), pDecodedFrame->size());
    Omm::AvStream::Log::instance()->avstream().trace(getName() + " writing decoded audio frame to byte queue finished.");
}


void
AudioSink::stopSinkPresentation()
{
    stopPresentation();
    halfByteQueue();
}


int
AudioSink::channels()
{
    return _inStreams[0]->getInfo()->channels();
}


unsigned int
AudioSink::sampleRate()
{
    return _inStreams[0]->getInfo()->sampleRate();
}


unsigned int
AudioSink::sampleSize()
{
    return _inStreams[0]->getInfo()->sampleSize();
}


int
AudioSink::frameCount(int bytes)
{
    return bytes / (channels() * sampleSize());
}


int
AudioSink::byteCount(int frames)
{
    return frames * channels() * sampleSize();
}


int
AudioSink::silence()
{
    return 0;
}


bool
AudioSink::audioAvailable()
{
    return _byteQueue.level();
}


int
AudioSink::audioRead(char* buffer, int size)
{
    _pClock->setTime(_audioTime);
    int bytesRead = _byteQueue.readSome(buffer, size);
    _audioTime += audioLength(bytesRead);
    setVolume(buffer, size);
    return bytesRead;
}


void
AudioSink::audioReadBlocking(char* buffer, int size)
{
    _pClock->setTime(_audioTime);
    _byteQueue.read(buffer, size);
    _audioTime += audioLength(size);
    setVolume(buffer, size);
}


void
AudioSink::initSilence(char* buffer, int size)
{
    memset(buffer, silence(), size);
}


void
AudioSink::setVolume(float vol)
{
    _volumeLock.lock();
    _volume = vol / 100.0;
    _volumeLock.unlock();
}


void
AudioSink::setVolume(char* buffer, int size)
{
    Log::instance()->avstream().debug("audio sink set volume ...");
    int16_t* buf = (int16_t*) buffer;
    int s = size >> 1;
    _volumeLock.lock();
    for (int i = 0; i < s; i++) {
        buf[i] = _volume * buf[i];
    }
    _volumeLock.unlock();
    Log::instance()->avstream().debug("audio sink set volume finished.");
}


void
AudioSink::halfByteQueue()
{
    Log::instance()->avstream().debug("trimming byte queue half full");
    int diff = _byteQueue.size() / 2 - _byteQueue.level();
    if (diff == 0) {
        return;
    }
    int buffSize = std::abs(diff);
    char buff[buffSize];
    if (diff > 0) {
        initSilence(buff, buffSize);
        _byteQueue.write(buff, buffSize);
    }
    else {
        _byteQueue.read(buff, buffSize);
    }
}


void
AudioSink::clearByteQueue()
{
    _byteQueue.clear();
}


void
AudioSink::reset()
{
    Log::instance()->avstream().debug("audio sink reset ...");
    _audioTime = Meta::invalidPts;
    _byteQueue.clear();
    Sink::reset();
    Log::instance()->avstream().debug("audio sink reset finished.");
}


void
AudioSink::setStartTime(Poco::Int64 startTime, bool toFirstFrame)
{
    Log::instance()->avstream().debug(getName() + " audio start time: " + Poco::NumberFormatter::format(_startTime) + ", clock start time: " + Poco::NumberFormatter::format(startTime) + ".");

    if (startTime < _startTime) {
        // TODO: insert silence until _startTime ...?
        Log::instance()->avstream().warning(getName() + " start time is later than CLOCK start time, should insert silence.");
    }
    else if ((startTime > _startTime) && !toFirstFrame) {
        // TODO: don't hardcode time base and sample size
        Poco::Int64 discardSize = (startTime - _startTime) * (sampleRate() / 90000.0) * channels() * 2; // s16 sample = 2 bytes
        // round discardSize to a multiple of sample size
        discardSize = (discardSize >> 2) << 2;
        char discardBuffer[discardSize];

        Log::instance()->avstream().debug(getName() + " start time is earlier than CLOCK start time, discarding " + Poco::NumberFormatter::format(discardSize) + " bytes.");
        audioReadBlocking(discardBuffer, discardSize);
    }
    _audioTime = startTime;
}


Poco::Int64
AudioSink::audioLength(Poco::Int64 bytes)
{
    return bytes * 90000.0 / (sampleRate() * channels() * 2);
}


VideoSink::VideoSink(const std::string& name, int width, int height, Meta::ColorCoding pixelFormat, int overlayCount, bool fullScreen) :
Sink(name),
_overlayCount(overlayCount),
_overlayVector(overlayCount),
_overlayQueue(name + " overlay", overlayCount - 2),
_timerThread(name + " timer thread"),
_timerThreadRunnable(*this, &VideoSink::timerThread),
_timerQuit(false),
_fullScreen(fullScreen),
_width(width),
_height(height),
_pixelFormat(pixelFormat),
_writeOverlayNumber(0),
_lastAspectRatio(1.0)
{
}


bool
VideoSink::checkInStream()
{
    if (!_inStreams[0]->getInfo()->isVideo()) {
        Omm::AvStream::Log::instance()->avstream().warning(getName() + " init failed, input stream is not a video stream");
        return false;
    }

    return true;
}


void
VideoSink::setStartTime(Poco::Int64 startTime)
{
}


void
VideoSink::startPresentation()
{
    Log::instance()->avstream().debug(getName() + " starting timer thread ...");

    _timerThread.start(_timerThreadRunnable);
}


void
VideoSink::stopPresentation()
{
    Log::instance()->avstream().debug(getName() + " stopping timer thread ...");

    setTimerStop(true);

    if (_timeQueue.full()) {
        Log::instance()->avstream().debug("video sink time queue full while stopping node, getting tick");
        _timeQueue.get();
    }
    else if (_timeQueue.empty()) {
        Log::instance()->avstream().debug("video sink time queue empty while stopping node, putting tick");
        _timeQueue.put(0);
    }
}


void
VideoSink::stopSinkPresentation()
{
    stopPresentation();
}


void
VideoSink::waitPresentationStop()
{
    Log::instance()->avstream().debug(getName() + " trying to join timer thread ...");
    try {
        // wait for run thread loop to finish
        // FIXME: what, if the thread finishes after _thread.isRunning() and before _thread.join()
        if (_timerThread.isRunning()) {
            _timerThread.join(500);
        }
    }
    catch(...) {
        Log::instance()->avstream().warning(getName() + " failed to cleanly shutdown timer thread");
    }
    setTimerStop(false);
    Log::instance()->avstream().debug(getName() + " timer thread joined.");
}


void
VideoSink::timerThread()
{
    Log::instance()->avstream().debug(getName() + " timer thread started.");

    while(!getTimerStop()) {
        Log::instance()->avstream().trace("video sink getting time ...");
        Poco::Int64 time = _timeQueue.get();
//         Log::instance()->avstream().trace(Poco::format("%s timer thread on tick time: %s, time queue size: %s",
//             getName(), Poco::NumberFormatter::format(time), Poco::NumberFormatter::format(_timeQueue.count())));
        Log::instance()->avstream().trace("video sink timer thread on tick time: " +
                                          Poco::NumberFormatter::format(time));
        onTick(time);
    }

    Log::instance()->avstream().debug(getName() + " timer thread finished.");
}


void
VideoSink::setTimerStop(bool stop)
{
    _timerLock.lock();
    _timerQuit = stop;
    _timerLock.unlock();
}


bool
VideoSink::getTimerStop()
{
    bool stop = false;
    _timerLock.lock();
    stop = _timerQuit;
    _timerLock.unlock();
    return stop;
}


int
VideoSink::getWidth()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_sinkLock);
    return _width;
}


int
VideoSink::getHeight()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_sinkLock);
    return _height;
}


Meta::ColorCoding
VideoSink::getFormat()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_sinkLock);
    return _pixelFormat;
}


int
VideoSink::displayWidth()
{
    return _width;
}


int
VideoSink::displayHeight()
{
    return _height;
}


void
VideoSink::displayRect(int& x, int& y, int& w, int& h)
{
    float ar = getInStream(0)->getInfo()->aspectRatio() * (float)getInStream(0)->getInfo()->width()/getInStream(0)->getInfo()->height();
    // FIXME: _overlayQueue.front() causes segfault here.
//     float ar = _overlayQueue.front()->_aspectRatio * (float)getInStream(0)->getInfo()->width()/getInStream(0)->getInfo()->height();
    h = displayHeight();
    w = ((int)rint(h * ar)) & ~1;
    if (w > displayWidth()) {
        w = displayWidth();
        h = ((int)rint(w / ar)) & ~1;
    }
    x = (displayWidth() - w) / 2;
    y = (displayHeight() - h) / 2;

    if (_lastAspectRatio != ar) {
        blankDisplay();
    }
    _lastAspectRatio = ar;
}


void
VideoSink::reset()
{
    Log::instance()->avstream().debug("video sink reset ...");
    _writeOverlayNumber = 0;
    setTimerStop(false);
    blankDisplay();
    Sink::reset();
    _overlayQueue.clear();
    Log::instance()->avstream().debug("video sink reset finished.");
}


void
VideoSink::writeDecodedFrame(Omm::AvStream::Frame* pDecodedFrame)
{
//     Omm::AvStream::Log::instance()->avstream().trace(getName() + " writing decoded video frame to overlay ...");
    Log::instance()->avstream().trace("video sink run thread, writing frame to overlay, frame pts: " + Poco::NumberFormatter::format(pDecodedFrame->getPts()) + " ...");
    Overlay* pWriteOverlay = _overlayVector[_writeOverlayNumber];
    pDecodedFrame->write(pWriteOverlay);
    pWriteOverlay->_pts = pDecodedFrame->getPts();
    pWriteOverlay->_aspectRatio = getInStream(0)->getInfo()->aspectRatio();
    Log::instance()->avstream().trace("video sink run thread, written frame to overlay, frame pts: " + Poco::NumberFormatter::format(pDecodedFrame->getPts()));
    _overlayQueue.put(pWriteOverlay);
    // increment modulo _overlayCount
    _writeOverlayNumber = (_writeOverlayNumber + 1) % _overlayCount;
}


void
VideoSink::onTick(Poco::Int64 time)
{
    Overlay* pOverlay = _overlayQueue.front();

    if (!pOverlay) {
        Omm::AvStream::Log::instance()->avstream().warning("null video frame in overlay queue, ignoring");
        return;
    }

    Poco::Int64 framePts = pOverlay->_pts;
    Log::instance()->avstream().trace("video sink timer thread time: " + Poco::NumberFormatter::format(time) + ", frame pts: " + Poco::NumberFormatter::format(framePts) + ", delta: " + Poco::NumberFormatter::format((framePts - time)));

    // wait until the frame in the overlay queue is not newer than current clock time
    // then fetch it from the overlay queue and display it.
    if (pOverlay->_pts <= time) {
        pOverlay = _overlayQueue.get();
        if (pOverlay) {
            displayFrame(pOverlay);
        }
    }
}


Overlay::Overlay(VideoSink* pVideoSink, int width, int height) :
_pVideoSink(pVideoSink),
_pts(Meta::invalidPts),
_width(width),
_height(height),
_aspectRatio(1.0)
{
}


int
Overlay::getWidth()
{
    return _pVideoSink->getWidth();
}


int
Overlay::getHeight()
{
    return _pVideoSink->getHeight();
}


Meta::ColorCoding
Overlay::getFormat()
{
    return _pVideoSink->getFormat();
}


Clock::Clock() :
_streamTime(Meta::invalidPts),
_clockTick(20),
_streamBase(90.0), // TODO: set _streamBase while loading the stream
_clockTickStreamBase(_clockTick * _streamBase)
{
}


void
Clock::attachAudioSink(AudioSink* pAudioSink)
{
    _audioSinkVec.push_back(pAudioSink);
    pAudioSink->_pClock = this,

    Log::instance()->avstream().debug("CLOCK attached " +
        pAudioSink->getName());
}


void
Clock::attachVideoSink(VideoSink* pVideoSink)
{
    _videoSinkVec.push_back(pVideoSink);

    Log::instance()->avstream().debug("CLOCK attached " +
        pVideoSink->getName());
}


void
Clock::setStartTime(bool toFirstFrame)
{
    Poco::Int64 startTime = Meta::invalidPts;

    for (std::vector<AudioSink*>::iterator it = _audioSinkVec.begin(); it != _audioSinkVec.end(); ++it) {
        Poco::Int64 time = (*it)->_timeQueue.get();
        Omm::AvStream::Log::instance()->avstream().debug("CLOCK received start time " + Poco::NumberFormatter::format(time) + " from " + (*it)->getName());
        // if startTime is not yet in initialized with a valid pts, take the first audio pts
        if (startTime == Meta::invalidPts) {
            startTime = time;
        }
        if (toFirstFrame) {
            startTime = std::min(time, startTime);
        }
        else {
            startTime = std::max(time, startTime);
        }
    }
    for (std::vector<VideoSink*>::iterator it = _videoSinkVec.begin(); it != _videoSinkVec.end(); ++it) {
        Poco::Int64 time = (*it)->_timeQueue.get();
        Omm::AvStream::Log::instance()->avstream().debug("CLOCK received start time " + Poco::NumberFormatter::format(time) + " from " + (*it)->getName());
        if (toFirstFrame) {
            startTime = std::min(time, startTime);
        }
        else {
            startTime = std::max(time, startTime);
        }
    }
    Log::instance()->avstream().debug("CLOCK setting start time to: " + Poco::NumberFormatter::format(startTime));

    for (std::vector<AudioSink*>::iterator it = _audioSinkVec.begin(); it != _audioSinkVec.end(); ++it) {
        (*it)->setStartTime(startTime, toFirstFrame);
    }
    for (std::vector<VideoSink*>::iterator it = _videoSinkVec.begin(); it != _videoSinkVec.end(); ++it) {
        (*it)->setStartTime(startTime);
    }
    _streamTime = startTime;
    _systemTime.update();

    Log::instance()->avstream().debug("CLOCK start time set.");
}



// setTime() is called by audio sink, where the samples written to pcm are counted, calculated to base time
// and frequently setTime is called (especially at a pts jump).
void
Clock::setTime(Poco::Int64 currentTime)
{
    Log::instance()->avstream().debug("CLOCK set stream time to: " + Poco::NumberFormatter::format(currentTime));
//     Poco::ScopedLock<Poco::FastMutex> lock(_clockLock);
    _clockLock.lock();
    _streamTime = currentTime;
    _systemTime.update();
    _clockLock.unlock();
}


void
Clock::clockTick(Poco::Timer& timer)
{
    // get system time and calculate elapsed time since last tick (_lastTickTime) in time base units. Add this to _streamTime.
    // _streamTime and _clockTickStreamBase is in time base units, clock tick is in ms
//     Poco::ScopedLock<Poco::FastMutex> lock(_clockLock);
    _clockLock.lock();
    if (_streamTime != Meta::invalidPts) {
        Poco::Int64 lastTick = _streamTime;
        _streamTime += _systemTime.elapsed() * 9.0 / 100.0;
        _systemTime.update();

        Log::instance()->avstream().debug("CLOCK TICK " + Poco::NumberFormatter::format(_streamTime) + ", since last tick: " +
             Poco::NumberFormatter::format((_streamTime - lastTick)));

        for (std::vector<VideoSink*>::iterator it = _videoSinkVec.begin(); it != _videoSinkVec.end(); ++it) {
            (*it)->currentTime(_streamTime);
        }
    }
    _clockLock.unlock();
}


void
Clock::start()
{
    Log::instance()->avstream().debug("CLOCK starting audio presentation ...");
    for (std::vector<AudioSink*>::iterator it = _audioSinkVec.begin(); it != _audioSinkVec.end(); ++it) {
        (*it)->startPresentation();
    }
    Log::instance()->avstream().debug("CLOCK audio presentation started.");
    Log::instance()->avstream().debug("CLOCK starting video presentation ...");
    for (std::vector<VideoSink*>::iterator it = _videoSinkVec.begin(); it != _videoSinkVec.end(); ++it) {
        (*it)->startPresentation();
    }
    Log::instance()->avstream().debug("CLOCK video presentation started.");
    Log::instance()->avstream().debug("CLOCK start ticking ...");
    _clockTimer.setPeriodicInterval(_clockTick);
    _clockTimer.start(Poco::TimerCallback<Clock>(*this, &Clock::clockTick));
    Log::instance()->avstream().debug("CLOCK started.");
}


void
Clock::stop()
{
    Log::instance()->avstream().debug("CLOCK stop ticking ...");
    _clockTimer.stop();
    Log::instance()->avstream().debug("CLOCK ticking stopped.");
    Log::instance()->avstream().debug("CLOCK stopping audio presentation ...");
    for (std::vector<AudioSink*>::iterator it = _audioSinkVec.begin(); it != _audioSinkVec.end(); ++it) {
        (*it)->stopSinkPresentation();
    }
    Log::instance()->avstream().debug("CLOCK stopping video presentation ...");
    for (std::vector<VideoSink*>::iterator it = _videoSinkVec.begin(); it != _videoSinkVec.end(); ++it) {
        (*it)->stopSinkPresentation();
    }
    for (std::vector<AudioSink*>::iterator it = _audioSinkVec.begin(); it != _audioSinkVec.end(); ++it) {
        (*it)->waitPresentationStop();
    }
    Log::instance()->avstream().debug("CLOCK audio presentation stopped.");
    for (std::vector<VideoSink*>::iterator it = _videoSinkVec.begin(); it != _videoSinkVec.end(); ++it) {
        (*it)->waitPresentationStop();
    }
    Log::instance()->avstream().debug("CLOCK video presentation stopped.");
    Log::instance()->avstream().debug("CLOCK stopped.");
}


void
Clock::reset()
{
    _audioSinkVec.clear();
    _videoSinkVec.clear();
    _streamTime = Meta::invalidPts;
}

} // namespace AvStream
} // namespace Omm

