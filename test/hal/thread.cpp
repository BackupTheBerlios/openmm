/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "thread.h"
#include "debug.h"

#include <string.h>
#include <errno.h>
#include <signal.h>


Thread::Thread()
{
    TRACE("Thread::Thread()");
    m_running = false;
    pthread_attr_init(&m_attr);
    pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&m_runningMutex, 0);
}


Thread::~Thread()
{
    pthread_attr_destroy(&m_attr);
    pthread_mutex_destroy(&m_runningMutex);
}


bool
Thread::isRunning()
{
    bool res;
    pthread_mutex_lock(&m_runningMutex);
    res = m_running;
    pthread_mutex_unlock(&m_runningMutex);
    return res;
}


void
Thread::setRunning(bool running)
{
    pthread_mutex_lock(&m_runningMutex);
    m_running = running;
    pthread_mutex_unlock(&m_runningMutex);
}


void
Thread::start()
{
    TRACE("Thread::start()");
    // don't start thread again, if already running.
    // if no thread is running, start can only be called once at a time, that means
    // that m_running is set when a thread is succesfully created, before any other
    // attempt is made to start another thread.
    if (!isRunning()) {
        void *arg;
        arg = this;
        if (pthread_create(&m_thread, NULL, (ThreadStarter)Thread::startThread, arg) != 0) {
            TRACE("Thread::start() pthread_create() returned with error: %s", strerror(errno));
        }
        else {
            setRunning(true);
        }
    }
}


// use dirty tricks with the argument parameter of pthread_create(), to call the non-static class method run()
// as a C-function for use as an argument of pthread_create() we need the static method startThread()
void*
Thread::startThread(void *arg)
{
    TRACE("Thread::startThread()");
    // now call the real virtual method run() which is the "threaded method" of all Thread subclasses.
    ((Thread*) arg)->run();
    ((Thread*) arg)->setRunning(false);
}


void
Thread::wait()
{
    TRACE("Thread::wait()");
    pthread_join(m_thread, 0);
    TRACE("Thread::wait(), Thread finished.");
}


void
Thread::kill()
{
    TRACE("Thread::exit()");
    if (!suicide()) {
        TRACE("Thread::exit() pthread_exit()");
        if (pthread_kill(m_thread, SIGHUP) != 0) {
            TRACE("Thread::kill() pthread_kill() returned with error: %s", strerror(errno));
        }
        else {
            setRunning(false);
        }
    }
    else {
        setRunning(false);  // we believe suicide() was successful.
    }
}


bool
Thread::suicide()
{
    return false;  // default: we don't commit suicide and execute pthread_kill().
}


Mutex::Mutex()
{
    TRACE("Mutex::Mutex()");
    pthread_mutex_init(&m_mutex, 0);
}


Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);
}


void
Mutex::lock()
{
    pthread_mutex_lock(&m_mutex);
}


void
Mutex::unlock()
{
    pthread_mutex_unlock(&m_mutex);
}


MutexLocker::MutexLocker(Mutex *mutex)
{
    TRACE("MutexLocker::MutexLocker()");
    m_mutex = mutex;
    m_mutex->lock();
}


MutexLocker::~MutexLocker()
{
    m_mutex->unlock();
    TRACE("MutexLocker::MutexLocker(), Mutex unlocked.");
}
