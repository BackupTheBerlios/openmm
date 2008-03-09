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
#ifndef NODE_H
#define NODE_H

#include "title.h"

#include <vector>
using namespace std;

/**
Nodes can be linked together. A source node can push or pop Titles to several sink nodes.

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
class Node {
public:
    Node();
    ~Node();

    virtual void addTitle(Title *title);
    virtual void delTitle(Title *title);

    void pushTitle(Title* title);
    void popTitle(Title* title);
    void addSink(Node* sink);
    void delSink(Node* sink);

private:
    vector<Node*> m_sinkList;
};

#endif
