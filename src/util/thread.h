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

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
// TODO: this does basically nothing! 
//       it's not yet finished, because jam crashes at startup with threading enabled.

class Thread
{
public:
    Thread();
    ~Thread();

    void start();
    void wait();
    void exit();

protected:
    virtual void run() = 0;
    virtual void beforeExit();

private:
    typedef void*(*ThreadStarter)(void*);
    static void     *startThread(void **);
    pthread_t        m_thread;
    pthread_attr_t   m_attr;
};


class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

private:
    pthread_mutex_t m_mutex;
};


class MutexLocker
{
public:
    MutexLocker(Mutex *mutex);
    ~MutexLocker();

private:
    Mutex *m_mutex;
};

#endif
