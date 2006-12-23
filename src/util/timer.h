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
#ifndef TIMER_H
#define TIMER_H

#include <thread.h>

/**
  Very simple and unprecise timer for timeouts in the range of seconds.
  Virtual method exec() is executed after given time in seconds.
  Start the timer with start().

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
class Timer : public Thread
{
public:
    Timer(int sec = 0);
    ~Timer();

    void setTimeout(int sec) { m_sec = sec; }
    virtual void exec() = 0;

private:
    virtual void run();
    int m_sec;
};

#endif
