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

#ifndef ViewImpl_INCLUDED
#define ViewImpl_INCLUDED

#include "Gui/View.h"
#include "Gui/Model.h"
#include "Gui/Geometry.h"

namespace Omm {
namespace Gui {


class ViewImpl
{
    friend class View;

public:
    virtual ~ViewImpl();

    virtual void init() {}
    View* getView();
    void* getNativeView();
    uint32_t getNativeWindowId();
    void* getNativeViewController();
    void setNativeView(void* pView);
    void setParent(View* pView);
    virtual void showView(bool async);
    virtual void hideView(bool async);
    void raise(bool async);
    bool isVisible();
    virtual int posXView();
    virtual int posYView();
    virtual int widthView();
    virtual int heightView();
    virtual void resizeView(int width, int height);
    virtual void setSizeConstraint(int width, int height, View::SizeConstraint size);
    virtual float getFontSize();
    virtual void setFontSize(float fontSize);
    virtual void moveView(int x, int y);
    virtual void setHighlighted(bool highlighted);
    void setBackgroundColor(const Color& color);
    bool getEnableRedraw() { return true; }
    void setEnableRedraw(bool enable) {}
    void setAcceptDrops(bool accept);
    int getDragMode();
    void setEnableHover(bool enable) {}

    void shown();
    void hidden();
    void resized(int width, int height);
    void selected();
    void released();
    void activated();
    void mouseMoved(const Position& pos);
    void dragStarted();
    void dragEntered(const Position& pos, Drag* pDrag, bool& accept);
    void dragMoved(const Position& pos, Drag* pDrag, bool& accept);
    void dragLeft();
    void dropped(const Position& pos, Drag* pDrag, bool& accept);

    View*                       _pView;
protected:
    void initViewImpl(View* pView, void* pNative);
    void triggerViewSync();

    void*                       _pNativeView;
    void*                       _pNativeViewController;
    void*                       _pNativeViewSelectorDispatcher;
    void*                       _pNativeViewActionTarget;
};


class PlainViewImpl : public ViewImpl
{
public:
    PlainViewImpl(View* pView);
};


}  // namespace Omm
}  // namespace Gui

#endif

