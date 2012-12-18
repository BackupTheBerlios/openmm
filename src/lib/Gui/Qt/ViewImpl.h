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


class QWidget;

namespace Omm {
namespace Gui {

class SignalProxy;
class QtEventFilter;
class Drag;

class ViewImpl
{
    friend class View;
    friend class SignalProxy;
    friend class QtEventFilter;

    template <class W> friend class QtViewImpl;

public:
    virtual ~ViewImpl();

    View* getView();
    QWidget* getNativeView();
#ifdef __WINDOWS__
    void* getNativeWindowId();
#else
    uint32_t getNativeWindowId();
#endif
    void setNativeView(QWidget* pView);
    void raise();
    virtual void setParent(View* pView);
    virtual void showView(bool async);
    virtual void hideView(bool async);
    virtual int posXView();
    virtual int posYView();
    virtual int widthView();
    virtual int heightView();
    virtual void resizeView(int width, int height);
    virtual float getFontSize();
    virtual void setFontSize(float fontSize);
    virtual void moveView(int x, int y);
    virtual void setHighlighted(bool highlighted);
    virtual void setBackgroundColor(const Color& color);
    void setAcceptDrops(bool accept);

protected:
    void initViewImpl(View* pView, QWidget* pNative, SignalProxy* pSignalProxy = 0);
    void triggerViewSync();

    View*                       _pView;
    QWidget*                    _pNativeView;
    SignalProxy*                _pSignalProxy;

private:
    void presented();
    void resized(int width, int height);
    void selected();
    void keyPressed(int key);
    void dragStarted();
    void dragEntered(const Position& pos, Drag* pDrag);
    void dragMoved(const Position& pos, Drag* pDrag);
    void dragLeft();
    void dropped(const Position& pos, Drag* pDrag);

    QtEventFilter*              _pEventFilter;
    QWidget*                    _pHighlightWidget;
};


class PlainViewImpl : public ViewImpl
{
public:
    PlainViewImpl(View* pView);
};


}  // namespace Omm
}  // namespace Gui

#endif

