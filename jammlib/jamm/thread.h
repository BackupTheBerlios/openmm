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
#ifndef JAMMTHREAD_H
#define JAMMTHREAD_H

#include "signode.h"

#include <pthread.h>

namespace Jamm {

/**
  Simple thread encapsulation.
  start() starts a thread, which executes the method run().
  Only one thread is created at one time for each object of class Thread.
  The attempt to start another thread while run() is still running results in no further action.

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/

class JThread
{
public:
    JThread();
    ~JThread();

    void start();
    void wait();
    void kill();
    bool isRunning();
    
//     void sleep(int msec);

protected:
    // with start() a thread is created and run() is executed in this thread.
    virtual void run() = 0;
    // return true if we kill ourselves in suicide() otherwise return false and
    // the thread that created the run() thread will try to kill run().
    virtual bool suicide();
    bool    m_setupSuccess;

private:
    typedef void*(*ThreadStarter)(void*);
    static  void*  startThread   (void*);
    void setRunning(bool running);
    pthread_t        m_thread;
    pthread_attr_t   m_attr;
    bool             m_running;
    pthread_mutex_t  m_runningMutex;
};


class JMutex
{
public:
    JMutex();
    ~JMutex();

    void lock();
    void unlock();

private:
    pthread_mutex_t m_mutex;
};


class JMutexLocker
{
public:
    JMutexLocker(JMutex *mutex);
    ~JMutexLocker();

private:
    JMutex *m_mutex;
};


class JTimer : public JThread
{
public:
    JTimer();
    ~JTimer();

    void startTimer(int milliSec) { m_milliSec = milliSec; start(); }
    
    JSignal fire;

private:
    virtual void run();
    virtual bool suicide();
    
    int     m_milliSec;
    bool    m_stopTimer;
};

} // namespace Jamm

#endif
