/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011, 2012                                     |
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

#ifndef DvbUtil_INCLUDED
#define DvbUtil_INCLUDED

#include <cstring>

#include <Poco/BufferedStreamBuf.h>
//#include <Poco/UnbufferedStreamBuf.h>
#include <Poco/ByteOrder.h>
#include <Poco/AtomicCounter.h>
#include <Poco/String.h>

#include "../AvStream.h"


namespace Omm {
namespace Dvb {

class BitField
{
public:
    BitField() : _data(0) {}
    BitField(void* data) : _data(data) {}

    template<typename T>
    T getValue(unsigned int offset, unsigned int length)
    /// getValue(offset, length) cuts out bits at offset and length in bits and interprets value as of type T
    /// no boundary checks, so you have to know what you do
    {
        unsigned int byteOffsetLeft = offset >> 3;

        if (length <= 8) {
            Poco::UInt8 buf = *((Poco::UInt8*)(_data) + byteOffsetLeft);
            unsigned int bitOffsetLeft = offset % 8;
            if (bitOffsetLeft) {
                Poco::UInt8 leftMask = 0xff >> bitOffsetLeft;
                buf &= leftMask;
            }
            unsigned int bitOffsetRight = (offset + length) % 8;
            if (bitOffsetRight) {
                bitOffsetRight = 8 - bitOffsetRight;
                Poco::UInt8 rightMask = 0xff << bitOffsetRight;
                buf &= rightMask;
                buf >>= bitOffsetRight;
            }
            return (T)buf;
        }
        else {
            Poco::UInt8 buf[sizeof(T)];
            memcpy(buf, (Poco::UInt8*)(_data) + byteOffsetLeft, sizeof(T));

            unsigned int bitOffsetLeft = offset % 8;
            if (bitOffsetLeft) {
                Poco::UInt8 leftMask = 0xff >> bitOffsetLeft;
                buf[0] &= leftMask;
            }

            unsigned int bitOffsetRight = (offset + length) % 8;
            if (bitOffsetRight) {
                bitOffsetRight = 8 - bitOffsetRight;
                Poco::UInt8 rightMask = 0xff << bitOffsetRight;
                buf[sizeof(T) - 1] &= rightMask;
            }

            T val = *((T*)buf);
#ifdef POCO_ARCH_LITTLE_ENDIAN
            val = Poco::ByteOrder::flipBytes(val);
#endif
            val >>= bitOffsetRight;
            return val;
        }
    }

    template<typename T>
    void setValue(unsigned int offset, unsigned int length, T val)
    {
        unsigned int byteOffsetLeft = offset >> 3;

        if (length <= 8) {
            Poco::UInt8 mask = 0xff;
            unsigned int bitOffsetLeft = offset % 8;
            if (bitOffsetLeft) {
                Poco::UInt8 leftMask = 0xff >> bitOffsetLeft;
                mask &= leftMask;
            }
            unsigned int bitOffsetRight = (offset + length) % 8;
            if (bitOffsetRight) {
                bitOffsetRight = 8 - bitOffsetRight;
                Poco::UInt8 rightMask = 0xff << bitOffsetRight;
                mask &= rightMask;
                val <<= bitOffsetRight;
            }
            // cut of bits of val that are outside mask
            val &= mask;
            // set masked bits to 0
            *((Poco::UInt8*)(_data) + byteOffsetLeft) &= ~mask;
            // finally set bits
            *((Poco::UInt8*)(_data) + byteOffsetLeft) |= val;
        }
        else {
#ifdef POCO_ARCH_LITTLE_ENDIAN
            val = Poco::ByteOrder::flipBytes(val);
#endif
            unsigned int bitOffsetRight = (offset + length) % 8;
            if (bitOffsetRight) {
                bitOffsetRight = 8 - bitOffsetRight;
            }

            val <<= bitOffsetRight;
            Poco::UInt8 buf[sizeof(T)];
            *((T*)buf) = val;

            // write the bytes in the middle
            memcpy((Poco::UInt8*)(_data) + byteOffsetLeft + 1, buf + 1, sizeof(T) - 2);
            // write left and right byte
            unsigned int bitOffsetLeft = offset % 8;
            Poco::UInt8 leftMask = 0xff >> bitOffsetLeft;
            *((Poco::UInt8*)(_data) + byteOffsetLeft) &= ~leftMask;
            *((Poco::UInt8*)(_data) + byteOffsetLeft) |= buf[0] & leftMask;
            Poco::UInt8 rightMask = 0xff << bitOffsetRight;
            *((Poco::UInt8*)(_data) + byteOffsetLeft + sizeof(T) - 1) &= ~rightMask;
            *((Poco::UInt8*)(_data) + byteOffsetLeft + sizeof(T) - 1) |= buf[sizeof(T) - 1] & rightMask;
        }
    }

    template<typename T>
    T getBytes(unsigned int byteOffset)
    {
        T val = *((T*)(void*)((Poco::UInt8*)(_data) + byteOffset));
#ifdef POCO_ARCH_LITTLE_ENDIAN
        // FIXME: also need to check sizeof(T), because inline methods flipBytes() are only provided for sizeof(T) > 1 ?
        if (sizeof(T) > 1) {
            val = Poco::ByteOrder::flipBytes(val);
        }
#endif
        return val;
    }

    template<typename T>
    void setBytes(unsigned int byteOffset, T val)
    {
#ifdef POCO_ARCH_LITTLE_ENDIAN
        if (sizeof(T) > 1) {
            val = Poco::ByteOrder::flipBytes(val);
        }
#endif
       *((T*)(void*)((Poco::UInt8*)(_data) + byteOffset)) = val;
    }

    template<typename T>
    T getBcd(unsigned int byteOffset, int countDigits)
    {
    //    LOG(dvb, trace, "bcd number: 0x" + Poco::NumberFormatter::formatHex(getValue<Poco::UInt32>(0, 32)));
        T res = 0;
        for (int i = 0; i < countDigits; i++) {
            res *= 10;
            res += getValue<Poco::UInt8>(byteOffset * 8 + i * 4, 4);
        }
        return res;
    }

    std::string
    getBcdString(unsigned int byteOffset, int countDigits, int decimals = 0)
    {
        std::string res;
        for (int i = 0; i < countDigits; i++) {
            if (decimals > 0 && decimals <= countDigits && i == countDigits - decimals) {
                res += '.';
            }
            char digit = getValue<char>(byteOffset * 8 + i * 4, 4) + '0';
            res += digit;
        }
        return res;
    }

    void*
    getData()
    {
        return _data;
    }

    void
    setData(void* pData)
    {
        _data = pData;
    }

    void*
    getData(unsigned int byteOffset)
    {
        return (Poco::UInt8*)(_data) + byteOffset;
    }

    std::string
    getData(unsigned int byteOffset, unsigned int byteLength)
    {
        return std::string((char*)(_data) + byteOffset, byteLength);
    }

    void
    setData(unsigned int byteOffset, unsigned int byteLength, void* data)
    {
        ::memcpy((Poco::UInt8*)(_data) + byteOffset, data, byteLength);
    }

    std::string
    filter(const std::string& str)
    {
        std::string res(str);
        for (int i = 0; i < res.length(); i++) {
            if (!std::isprint(res[i])) {
                res[i] = ' ';
            }
        }
        return Poco::trim(res);
    }

protected:
    void*               _data;
};


class UnixFileStreamBuf : public Poco::BufferedStreamBuf
//class UnixFileStreamBuf : public Poco::UnbufferedStreamBuf
{
public:
    UnixFileStreamBuf(int fileDesc, int bufSize) : BasicBufferedStreamBuf(bufSize, std::ios_base::in), _fileDesc(fileDesc) {}
//    UnixFileStreamBuf(int fileDesc) : BasicUnbufferedStreamBuf(), _fileDesc(fileDesc) {}

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        return read(_fileDesc, buffer, length);
    }

private:
    int         _fileDesc;
};


class UnixFileIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    UnixFileIStream(int fileDesc, int bufSize = 4096) : _streamBuf(fileDesc, bufSize), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    UnixFileStreamBuf   _streamBuf;
};


class ByteQueueStreamBuf : public Poco::BufferedStreamBuf
//class ByteQueueStreamBuf : public Poco::UnbufferedStreamBuf
{
public:
    ByteQueueStreamBuf(AvStream::ByteQueue& byteQueue) : BasicBufferedStreamBuf(byteQueue.size(), std::ios_base::in), _byteQueue(byteQueue), _stop(0) {}
//    ByteQueueStreamBuf(int fileDesc) : BasicUnbufferedStreamBuf(), _fileDesc(fileDesc) {}

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        if (!_stop) {
            _byteQueue.read(buffer, length);
            return length;
        }
        else {
            return 0;
        }
    }

    void stop()
    {
        _stop = 1;
    }

private:
    AvStream::ByteQueue&         _byteQueue;
    Poco::AtomicCounter          _stop;
};


class ByteQueueIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    ByteQueueIStream(AvStream::ByteQueue& byteQueue) : _streamBuf(byteQueue), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

    void stop()
    {
        _streamBuf.stop();
    }

private:
    ByteQueueStreamBuf   _streamBuf;
};


}  // namespace Omm
}  // namespace Dvb

#endif
