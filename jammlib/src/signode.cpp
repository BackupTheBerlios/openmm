/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
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
#include "signode.h"

using namespace Jamm;

JNode::JNode()
{
}


void
JNode::connectNodes(JNode* sender, JNode* receiver)
{
    sender->registerReceiver(receiver);
}


void
JNode::disconnectNodes(JNode* sender, JNode* receiver)
{
    sender->unregisterReceiver(receiver);
}


void
JNode::registerReceiver(JNode* receiver)
{
    m_receiverList.push_back(receiver);
}


void
JNode::unregisterReceiver(JNode* /*receiver*/)
{
    //TODO: implement deleting of receivers
//     m_receiverList.erase(m_receiverList.find(receiver));
}


void
JNode::emitSignal()
{
    for (vector<JNode*>::iterator i = m_receiverList.begin(); i != m_receiverList.end() ;++i) {
        (*i)->onSignalReceived();
    }
}


void 
JNode::onSignalReceived()
{
}
