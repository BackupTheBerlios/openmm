#include "/home/jb/devel/cc/omm/lib/Omm/AvStream.h"

// #include <Poco/Semaphore.h>
// #include <Poco/Mutex.h>
// #include <sstream>
// 
// class ByteQueue
// {
// public:
//     ByteQueue(int size);
//     
//     /**
//     read() and write() block until num bytes have been read or written
//     **/
// //     void read(char* buffer, int num);
// //     void write(char* buffer, int num);
//     
//     /**
//     readSome() and writeSome() read upto num bytes, return the number of bytes read
//     and block if queue is empty / full
//     **/
// //     int readSome(char* buffer, int num);
// //     int writeSome(char* buffer, int num);
//     
// //     int level();
//     
// private:
// //     std::fstream            _bytestream;
//     std::stringstream       _bytestream;
//     int                     _size;
//     int                     _level;
//     Poco::Semaphore         _writeSemaphore;
//     bool                    _writeSemaphoreSet; // workaround
//     Poco::Semaphore         _readSemaphore;
//     bool                    _readSemaphoreSet; // workaround
//     Poco::FastMutex         _lock;
// };
// 
// ByteQueue::ByteQueue(int size) :
// _size(size),
// _level(0),
// _writeSemaphore(1, 1),
// _writeSemaphoreSet(true),
// _readSemaphore(0, 1),
// _readSemaphoreSet(false)
// {
// //     _bytestream.open("bytestream");
// }


int
main(int argc, char** argv)
{
    Omm::AvStream::ByteQueue byteQueue(1024);
//     ByteQueue byteQueue(1024);
    return 0;
}