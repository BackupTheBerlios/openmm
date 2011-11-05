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

#ifndef QtViewImpl_INCLUDED
#define QtViewImpl_INCLUDED

#include <QtGui>
#include "ViewImpl.h"


namespace Omm {
namespace Gui {


class SignalProxy : public QObject
{
    Q_OBJECT

public:
    SignalProxy(ViewImpl* pViewImpl);

    virtual void init();

    void showView();
    void hideView();
    void syncView();

signals:
    void showViewSignal();
    void hideViewSignal();
    void syncViewSignal();

private slots:
    void syncViewSlot();

protected:
    ViewImpl*   _pViewImpl;
};


// template classes are not supported by Q_OBJECT, so we have to separate events
// that come through a signal or by callback.
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

//    void keyPressEvent(QKeyEvent* pKeyEvent)
//    {
//        _pViewImpl->keyPressed(pKeyEvent->key());
//    }

    QSize sizeHint() const
    {
//        return QSize(800, 40);
//        return QSize(_pViewImpl->getView()->width(View::Pref), _pViewImpl->getView()->height(View::Pref));
        return QSize(_pViewImpl->getView()->width(), _pViewImpl->getView()->height());
    }

    ViewImpl*   _pViewImpl;
};


class QtEventFilter : public QObject
{
public:
    QtEventFilter(ViewImpl* pViewImpl) : _pViewImpl(pViewImpl)
    {
    }

private:
    virtual bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(event);
            _pViewImpl->keyPressed(pKeyEvent->key());
            return true;
        }
    }

    ViewImpl*      _pViewImpl;
};


}  // namespace Omm
}  // namespace Gui

#endif
