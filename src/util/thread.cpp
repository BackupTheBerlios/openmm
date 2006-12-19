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


Thread::Thread()
{
    TRACE("Thread::Thread()");
    pthread_attr_init(&m_attr);
    pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_JOINABLE);
}


Thread::~Thread()
{
    pthread_attr_destroy(&m_attr);
}


void
Thread::start()
{
    TRACE("Thread::start()");
    // TODO: uncomment this (and comment out run()), and you get the threads kick ...
     void **arg;
     *arg = this;
     pthread_create(&m_thread, NULL, (ThreadStarter)Thread::startThread, arg);  // TODO: threads won't start ...?

    // TODO: for now we don't create a thread and execute synchronous!!
//     run();
}


void
Thread::wait()
{
    TRACE("Thread::wait()");
    pthread_join(m_thread, 0);
    TRACE("Thread::wait(), Thread finished.");
}


void
Thread::exit()
{
    TRACE("Thread::exit()");
    beforeExit();
//     TRACE("Thread::exit() pthread_exit()");
// //     sleep(1);
//     pthread_exit(0);  // this hangs in Qt event loop (maybe QApp.exec() does a pthread_exit(), too?)
}


void
Thread::beforeExit()
{
}


void*
Thread::startThread(void **arg)
{
    TRACE("Thread::startThread()");
    (*((Thread**) arg))->run();
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
