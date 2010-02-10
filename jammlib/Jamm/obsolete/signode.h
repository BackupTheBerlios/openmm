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

#ifndef JAMMSIGNODE_H
#define JAMMSIGNODE_H

#include <vector>
using namespace std;


namespace Jamm {

// TODO: solve multiple inheritance, JNode not accessible when inherited as second class
//       -> NodeInterface with pure virtual method onSignalReceived()
//       -> Node that sends singals kann have a member of type Node
//          and a getNode() returning a pointer to it.
//       we like to be symmetric, so keep it in one class (no NodeInterface)
// TODO: more than one signal
// TODO: signal arguments
// TODO: signals between threads

class JSlot
{
public:
    virtual void onSignalReceived() = 0;
};


// template<class T> class JJSlot {
// public:
//     JJSlot(void* slotUser) { s = static_cast<T*>(slotUser); }
//     virtual void onSignalReceived();
// protected:
//     T* s;
// };


class JSignal
{
public:
    JSignal();
    
    static void connectNodes(JSignal* sender, JSlot* receiver);
    static void disconnectNodes(JSignal* sender, JSlot* receiver);
    void emitSignal();
    
private:
    vector<JSlot*> m_receiverList;
};

} // namespace Jamm

#endif
