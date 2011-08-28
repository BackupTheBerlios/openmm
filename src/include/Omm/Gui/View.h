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

#include "Model.h"

namespace Omm {
namespace Gui {

class ViewImpl;
class Model;
class Controller;
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

    Model* getModel() const;
    virtual void setModel(Model* pModel);
    Controller* getController() const;
    void setController(Controller* pController);
    template<class C, class M> void setControllerModel(ControllerModel<C,M>* pControllerModel)
    {
        setModel(pControllerModel);
        setController(pControllerModel);
    }
    Layout* getLayout() const;
    void setLayout(Layout* pLayout);

    const std::string& getName() const;
    void setName(const std::string& name);

    typedef std::vector<View*>::iterator ChildIterator;
    ChildIterator beginChild();
    ChildIterator endChild();
    int childCount();

protected:
    View(ViewImpl* pViewImpl, View* pParent = 0);

    virtual void syncView(Model* pModel) {}
    virtual void select() {}

    View*                       _pParent;
    std::vector<View*>          _children;
    ViewImpl*                   _pImpl;
    Model*                      _pModel;
    Controller*                 _pController;
    Layout*                     _pLayout;
    std::string                 _name;

private:
    void syncViewWithModel(Model* pModel = 0);
};


template <class V, class C, class M>
class Widget : public V, public C, public M
{
public:
    Widget(View* pParent = 0) : V(pParent)
    {
        V::setModel(this);
        V::setController(this);
        C::attachModel(this);
    }
    
    virtual ~Widget() {}
};


}  // namespace Omm
}  // namespace Gui

#endif
