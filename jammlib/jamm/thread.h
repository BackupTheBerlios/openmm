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
#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

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


/**
  Very simple and unprecise timer for timeouts in the range of seconds.
  Virtual method exec() is executed after given time in seconds.
  Start the timer with start().

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
class JTimer : public JThread
{
public:
    JTimer(int sec = 0);
    ~JTimer();

    void setTimeout(int sec) { m_sec = sec; }
    virtual void exec() = 0;

private:
    virtual void run();
    int m_sec;
};

#endif
