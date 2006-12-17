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
#include "node.h"
#include "debug.h"


Node::Node()
{
}


Node::~Node()
{
}


void
Node::addTitle(Title *title)
{
    TRACE("Node::addTitle()");
}


void
Node::delTitle(Title *title)
{
    TRACE("Node::delTitle()");
}


void
Node::pushTitle(Title* title)
{
//     TRACE("Node::pushTitle()");
    for (vector<Node*>::iterator i = m_sinkList.begin(); i != m_sinkList.end(); ++i) {
        (*i)->addTitle(title);
    }
}


void
Node::popTitle(Title* title)
{
    TRACE("Node::popTitle()");
    for (vector<Node*>::iterator i = m_sinkList.begin(); i != m_sinkList.end(); ++i) {
        (*i)->delTitle(title);
    }
}


void
Node::addSink(Node* sink)
{
    TRACE("Node::link()");
    m_sinkList.push_back(sink);
}


void
Node::delSink(Node* sink)
{
    TRACE("Node::link()");
    m_sinkList.erase(find(m_sinkList.begin(), m_sinkList.end(), sink));
}
