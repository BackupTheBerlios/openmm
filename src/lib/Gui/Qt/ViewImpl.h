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

#include <QtGui>
#include "Gui/View.h"

namespace Omm {
namespace Gui {


class ViewImpl : public QObject
{
    Q_OBJECT
    
    friend class View;
    template <class W> friend class QtViewImpl;
    
public:
    virtual ~ViewImpl();

    View* getView();
    QWidget* getNativeView();
    void setNativeView(QWidget* pView);
    virtual void addSubview(View* pView);
    virtual void showView();
    virtual void hideView();
    virtual int widthView();
    virtual int heightView();
    virtual void resizeView(int width, int height);
    virtual void moveView(int x, int y);
    virtual void setHighlighted(bool highlighted);
    void setBackgroundColor(const Color& color);

protected:
    void initViewImpl(View* pView, QWidget* pNative);

    View*                       _pView;
    QWidget*                    _pNativeView;

signals:
    void showViewSignal();
    void hideViewSignal();

private:
    void presented();
    void resized(int width, int height);
    void selected();
};


template<class W>
class QtViewImpl : public W
{
public:
    QtViewImpl(ViewImpl* pViewImpl, QWidget* pParent = 0) : W(pParent), _pViewImpl(pViewImpl) {}

    void showEvent(QShowEvent* event)
    {
        _pViewImpl->presented();
    }

    void resizeEvent(QResizeEvent* pEvent)
    {
        if (pEvent->oldSize().height() > 0) {
            _pViewImpl->resized(pEvent->size().width(), pEvent->size().height());
        }
    }

    void mousePressEvent(QMouseEvent* pMouseEvent)
    {
        _pViewImpl->selected();
//        W::mousePressEvent(pMouseEvent);
    }

    ViewImpl*   _pViewImpl;
};


class PlainViewImpl : public ViewImpl
{
public:
    PlainViewImpl(View* pView);
};


}  // namespace Omm
}  // namespace Gui

#endif

