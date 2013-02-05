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
#include <stdint.h>

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

#define PROXY_NOTIFY_CONTROLLER(CLASS, METHOD, ...) for (View::ControllerIterator it = _pViewImpl->getView()->beginController(); it != _pViewImpl->getView()->endController(); ++it) \
{ CLASS* pCLASS = dynamic_cast<CLASS*>(*it); if (pCLASS) { pCLASS->METHOD(__VA_ARGS__); } }


class View
{
    friend class ViewImpl;
    friend class Model;
    friend class Layout;
//    friend class HorizontalLayout;

public:
    typedef enum {None, Current, Min, Pref, Max} SizeConstraint;
    typedef enum {AlignLeft, AlignCenter, AlignRight} Alignment;
    typedef enum {Horizontal, Vertical} Orientation;

    View(View* pParent = 0);
    virtual ~View();

    void* getNativeView();
#ifdef __WINDOWS__
    void* getNativeWindowId();
#else
    uint32_t getNativeWindowId();
#endif

    View* getParent();
    void setParent(View* pView);

    virtual void show(bool async = true);
    virtual void hide(bool async = true);
    virtual void raise(bool async = true);

    int posX();
    int posY();
    int width(SizeConstraint size = Current);
    int height(SizeConstraint size = Current);
    void setSizeConstraint(int width, int height, SizeConstraint size = Current);
    float stretchFactor();
    void setStretchFactor(float stretch = 1.0);
    virtual void setWidth(int width);
    virtual void setHeight(int height);
    virtual void resize(SizeConstraint size = Pref);
    virtual void resize(int width, int height);
    virtual void scale(float factor);
    virtual void move(int x, int y);

    Model* getModel() const;
    void* getNativeModel();
    virtual void setModel(Model* pModel = 0);
    void detachModel();
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

    typedef std::vector<View*>::iterator SubviewIterator;
    SubviewIterator beginSubview();
    SubviewIterator endSubview();
    int subviewCount();

    typedef std::vector<Controller*>::iterator ControllerIterator;
    ControllerIterator beginController();
    ControllerIterator endController();

    void setBackgroundColor(const Color& color);
    virtual void setHighlighted(bool highlighted = true);

    ViewImpl* getViewImpl();
    void resizeNoLayout(int width, int height);
    void updateLayout();

    void syncView();
    virtual void syncViewImpl() {}

    bool getEnableRedraw();
    void setEnableRedraw(bool enable = true);

    void triggerKeyEvent(Controller::KeyCode key);
    void setAcceptDrops(bool accept = true);
    void setEnableHover(bool enable = true);

protected:
    View(View* pParent, bool createPlainView);

    void initView(View* pParent);
    void removedSubview(View* pView);
    void addedSubview(View* pView);

    View*                       _pParent;
    std::vector<View*>          _subviews;
    ViewImpl*                   _pImpl;
    Model*                      _pModel;
    std::vector<Controller*>    _controllers;
    Layout*                     _pLayout;
    std::string                 _name;
    int                         _minWidth;
    int                         _minHeight;
    int                         _prefWidth;
    int                         _prefHeight;
    int                         _maxWidth;
    int                         _maxHeight;
    float                       _stretchFactor;
    float                       _scaleFactor;
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
