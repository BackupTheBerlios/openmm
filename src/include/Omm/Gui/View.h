/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef View_INCLUDED
#define View_INCLUDED

#include <vector>
#include <string>

namespace Omm {
namespace Gui {

class ViewImpl;
class Model;
class Layout;


class View
{
    friend class ViewImpl;
    friend class Model;
    friend class Layout;
    
public:
    View(View* pParent = 0);
    virtual ~View();

    void* getNativeView();
    View* getParent();

    void show();
    void hide();
    int width();
    int height();
    void resize(int width, int height);
    void move(int x, int y);

    Model* getModel();
    virtual void setModel(Model* pModel);

    Layout* getLayout();
    void setLayout(Layout* pLayout);

    const std::string& getName() const;
    void setName(const std::string& name);

    typedef std::vector<View*>::iterator ChildIterator;
    ChildIterator beginChild();
    ChildIterator endChild();
    int childCount();

protected:
    View(ViewImpl* pViewImpl, View* pParent = 0);

    virtual void syncView() {}
    virtual void select() {}

    View*                       _pParent;
    std::vector<View*>          _children;
    ViewImpl*                   _pImpl;
    Model*                      _pModel;
    Layout*                     _pLayout;
    std::string                 _name;
};


}  // namespace Omm
}  // namespace Gui

#endif
