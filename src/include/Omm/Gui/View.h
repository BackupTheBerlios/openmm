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
#include "Color.h"

namespace Omm {
namespace Gui {

class ViewImpl;
class Model;
class Controller;
class Layout;


// need a dynamic_cast, because different subclasses of Controller can be attached to View.
#define NOTIFY_CONTROLLER(CLASS, METHOD, ...) for (ControllerIterator it = beginController(); it != endController(); ++it) \
{ CLASS* pCLASS = dynamic_cast<CLASS*>(*it); if (pCLASS) { pCLASS->METHOD(__VA_ARGS__); } }

#define IMPL_NOTIFY_CONTROLLER(CLASS, METHOD, ...) for (View::ControllerIterator it = _pView->beginController(); it != _pView->endController(); ++it) \
{ CLASS* pCLASS = dynamic_cast<CLASS*>(*it); if (pCLASS) { pCLASS->METHOD(__VA_ARGS__); } }


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
    virtual void setModel(Model* pModel = 0);
    void attachController(Controller* pController);
    void detachController(Controller* pController);
    template<class C, class M> void setControllerModel(ControllerModel<C,M>* pControllerModel)
    {
        setModel(pControllerModel);
        attachController(pControllerModel);
    }
    Layout* getLayout() const;
    void setLayout(Layout* pLayout);

    const std::string& getName() const;
    void setName(const std::string& name);

    typedef std::vector<View*>::iterator ChildIterator;
    ChildIterator beginChild();
    ChildIterator endChild();
    int childCount();

    typedef std::vector<Controller*>::iterator ControllerIterator;
    ControllerIterator beginController();
    ControllerIterator endController();

    void setBackgroundColor(const Color& color);
    virtual void setHighlighted(bool highlighted = true);

    ViewImpl* getViewImpl();
    void selected();

protected:
    View(View* pParent, bool createPlainView);

    void initView(View* pParent);
    virtual void syncView(Model* pModel) {}

    View*                       _pParent;
    std::vector<View*>          _children;
    ViewImpl*                   _pImpl;
    Model*                      _pModel;
    std::vector<Controller*>    _controllers;
    Layout*                     _pLayout;
    std::string                 _name;

private:
    void syncViewWithModel(Model* pModel = 0);
};


template <class C, class V>
class ControllerView : public C, public V
{
public:
    ControllerView(View* pParent = 0) : V(pParent)
    {
        V::attachController(this);
    }

    virtual ~ControllerView() {}
};


template <class V, class C, class M>
class Widget : public V, public C, public M
{
public:
    Widget(View* pParent = 0) : V(pParent)
    {
        V::attachController(this);
        C::attachModel(this);
        V::setModel(this);
    }
    
    virtual ~Widget() {}
};


}  // namespace Omm
}  // namespace Gui

#endif
