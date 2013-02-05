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
#include "QtDragImpl.h"
#include "Log.h"
#include "Gui/GuiLogger.h"

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
    void raiseView();
    void syncView();

signals:
    void showViewSignal();
    void hideViewSignal();
    void raiseViewSignal();
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

    virtual void showEvent(QShowEvent* event)
    {
        _pViewImpl->presented();
    }

    virtual void resizeEvent(QResizeEvent* pEvent)
    {
        if (pEvent->oldSize().height() > 0) {
            _pViewImpl->resized(pEvent->size().width(), pEvent->size().height());
        }
        W::resizeEvent(pEvent);
    }

    virtual void mousePressEvent(QMouseEvent* pMouseEvent)
    {
//        LOG(gui, debug, "mouse press event");
        if (pMouseEvent->button() == Qt::LeftButton) {
            _dragStartPosition = pMouseEvent->pos();
        }
        _pViewImpl->selected();
//        W::mousePressEvent(pMouseEvent);
    }

    virtual void mouseReleaseEvent(QMouseEvent* pMouseEvent)
    {
        _pViewImpl->released();
    }

    virtual void mouseDoubleClickEvent(QMouseEvent* pMouseEvent)
    {
        _pViewImpl->activated();
    }

    virtual void mouseMoveEvent(QMouseEvent* pMouseEvent)
    {
//        LOG(gui, debug, "mouse move event");
        if (!(pMouseEvent->buttons() & Qt::LeftButton)) {
            _pViewImpl->mouseHovered(Position(pMouseEvent->x(), pMouseEvent->y()));
            return;
        }
        _pViewImpl->mouseMoved(Position(pMouseEvent->x(), pMouseEvent->y()));
        if ((pMouseEvent->pos() - _dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
            return;
        }
        _pViewImpl->dragStarted();
//        W::mouseMoveEvent(pMouseEvent);
    }

    virtual void dragEnterEvent(QDragEnterEvent* pDragEvent)
    {
        LOG(gui, debug, "DRAG ENTER EVENT");
        // if dragging started not in an omm application, mime data is not of type QtMimeData and pDrag is set to 0.
        const QtMimeData* pMime = dynamic_cast<const QtMimeData*>(pDragEvent->mimeData());
        if (pMime) {
            _pViewImpl->dragEntered(Position(pDragEvent->pos().x(), pDragEvent->pos().y()), pMime->_pDragImpl->getDrag());
            pDragEvent->acceptProposedAction();
        }
    }

    virtual void dragMoveEvent(QDragMoveEvent* pDragEvent)
    {
        LOG(gui, debug, "DRAG MOVE EVENT");
        const QtMimeData* pMime = dynamic_cast<const QtMimeData*>(pDragEvent->mimeData());
        if (pMime) {
            _pViewImpl->dragMoved(Position(pDragEvent->pos().x(), pDragEvent->pos().y()), pMime->_pDragImpl->getDrag());
            pDragEvent->acceptProposedAction();
        }
    }

    virtual void dragLeaveEvent()
    {
        LOG(gui, debug, "DRAG LEAVE EVENT");
        _pViewImpl->dragLeft();
    }

    virtual void dropEvent(QDropEvent* pDropEvent)
    {
        LOG(gui, debug, "DROP EVENT");
        const QtMimeData* pMime = dynamic_cast<const QtMimeData*>(pDropEvent->mimeData());
        if (pMime) {
            _pViewImpl->dropped(Position(pDropEvent->pos().x(), pDropEvent->pos().y()), pMime->_pDragImpl->getDrag());
            pDropEvent->acceptProposedAction();
        }
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
    QPoint      _dragStartPosition;
};


class QtEventFilter : public QObject
{
public:
    QtEventFilter(ViewImpl* pViewImpl) : _pViewImpl(pViewImpl)
    {
    }

private:
    virtual bool eventFilter(QObject* pObject, QEvent* pEvent)
    {
        if (pEvent->type() == QEvent::KeyPress) {
            QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(pEvent);
            _pViewImpl->keyPressed(pKeyEvent->key());
            return true;
        }
    }

    ViewImpl*      _pViewImpl;
};


}  // namespace Omm
}  // namespace Gui

#endif
