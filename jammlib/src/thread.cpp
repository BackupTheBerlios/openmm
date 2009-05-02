/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#include "thread.h"
#include "signode.h"
#include "debug.h"

#include <string>
#include <cstring>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

using namespace Jamm;

JThread::JThread()
{
    TRACE("JThread::JThread()");
    m_running = false;
    m_setupSuccess = true;
    pthread_attr_init(&m_attr);
    pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&m_runningMutex, 0);
}


JThread::~JThread()
{
    pthread_attr_destroy(&m_attr);
    pthread_mutex_destroy(&m_runningMutex);
}


bool
JThread::isRunning()
{
    bool res;
    pthread_mutex_lock(&m_runningMutex);
    res = m_running;
    pthread_mutex_unlock(&m_runningMutex);
    return res;
}


void
JThread::setRunning(bool running)
{
    pthread_mutex_lock(&m_runningMutex);
    m_running = running;
    pthread_mutex_unlock(&m_runningMutex);
}


void
JThread::start()
{
    if (!m_setupSuccess) {
        TRACE("JThread::start() setup of thread was not successfull, not starting");
        return;
    }

    TRACE("JThread::start()");
    // don't start thread again, if already running.
    // if no thread is running, start can only be called once at a time, that means
    // that m_running is set when a thread is succesfully created, before any other
    // attempt is made to start another thread.
    if (!isRunning()) {
        void *arg;
        arg = this;
        if (pthread_create(&m_thread, NULL, (ThreadStarter)JThread::startThread, arg) != 0) {
            TRACE("JThread::start() pthread_create() returned with error: %s", strerror(errno));
        }
        else {
            setRunning(true);
        }
    }
}


// use dirty tricks with the argument parameter of pthread_create(), to call the non-static class method run()
// as a C-function for use as an argument of pthread_create() we need the static method startThread()
void*
JThread::startThread(void *arg)
{
    TRACE("JThread::startThread()");
    // now call the real virtual method run() which is the "threaded method" of all Thread subclasses.
    ((JThread*) arg)->run();
    ((JThread*) arg)->setRunning(false);
}


void
JThread::wait()
{
    TRACE("JThread::wait()");
    pthread_join(m_thread, 0);
    TRACE("JThread::wait(), Thread finished.");
}


void
JThread::kill()
{
    TRACE("JThread::kill()");
    if (!suicide()) {
        if (isRunning() && pthread_kill(m_thread, SIGHUP) != 0) {
            TRACE("JThread::kill() pthread_kill() returned with error: %s", strerror(errno));
        }
        else {
            TRACE("JThread::kill() pthread_kill() success");
            setRunning(false);
        }
    }
    else {
        setRunning(false);  // we believe suicide() was successful.
    }
}


bool
JThread::suicide()
{
    return false;  // default: we don't commit suicide and execute pthread_kill().
}


JMutex::JMutex()
{
//     TRACE("JMutex::JMutex()");
    pthread_mutex_init(&m_mutex, 0);
}


JMutex::~JMutex()
{
    pthread_mutex_destroy(&m_mutex);
}


void
JMutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}


void
JMutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

/*-----------------------------------------------------------------------*/

JMutexLocker::JMutexLocker(JMutex *mutex)
{
//     TRACE("JMutexLocker::JMutexLocker()");
    m_mutex = mutex;
    m_mutex->lock();
}


JMutexLocker::~JMutexLocker()
{
    m_mutex->unlock();
//     TRACE("JMutexLocker::~JMutexLocker(), Mutex released.");
}

/*-----------------------------------------------------------------------*/

JTimer::JTimer()
 : JThread(),
m_milliSec(0),
m_stopTimer(false)
{
}


JTimer::~JTimer()
{
}


void
JTimer::run()
{
    // TODO: lock m_stopTimer
    TRACE("JTimer::run()");
    while (!m_stopTimer) {
        usleep(m_milliSec*1000);
        fire.emitSignal();
    }
}


bool
JTimer::suicide()
{
    // TODO: lock m_stopTimer
    m_stopTimer = true;
    return true;
}
