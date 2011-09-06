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

#include <Poco/NumberFormatter.h>

#include <Omm/Util.h>

#include "ListImpl.h"
#include "Gui/List.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


class QtScrollArea : public QScrollArea
{
    friend class ListViewImpl;

    QtScrollArea(QWidget* pParent = 0) : QScrollArea(pParent) {}
    
    void resizeEvent(QResizeEvent* pEvent)
    {
        if (pEvent->oldSize().height() > 0) {
            _pListViewImpl->resized(pEvent->size().width(), pEvent->size().height());
        }
    }

    ListViewImpl*   _pListViewImpl;
};


ListViewImpl::ListViewImpl(View* pView, View* pParent) :
ViewImpl(pView, new QtScrollArea(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0))),
_pScrollWidget(0)
{
    QtScrollArea* pNativeView = static_cast<QtScrollArea*>(_pNativeView);
    pNativeView->_pListViewImpl = this;

    _pScrollWidget = new QWidget;
    _pScrollWidget->resize(pNativeView->viewport()->size());
//    _pNativeView->setBackgroundRole(QPalette(Qt::white));
//    _pNativeView->setPalette(QPalette(Qt::white));
//    _pNativeView->setAutoFillBackground(true);

    pNativeView->setWidget(_pScrollWidget);

    connect(this, SIGNAL(moveWidgetSignal(int, View*)), this, SLOT(moveWidgetSlot(int, View*)));
    connect(pNativeView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolledSlot(int)));
}


ListViewImpl::~ListViewImpl()
{
}


int
ListViewImpl::visibleRows()
{
    QScrollArea* pNativeView = static_cast<QScrollArea*>(_pNativeView);

    Omm::Gui::Log::instance()->gui().debug("list view impl visible rows");
    Omm::Gui::Log::instance()->gui().debug("list view impl viewport width: " + Poco::NumberFormatter::format(pNativeView->viewport()->geometry().width())
            + ", height: " + Poco::NumberFormatter::format(pNativeView->viewport()->geometry().height()));
    
    ListView* pListView =  static_cast<ListView*>(_pView);
    int rows = pNativeView->viewport()->geometry().height() / pListView->_itemViewHeight + 2;
    Omm::Gui::Log::instance()->gui().debug("list view impl number of visible rows: " + Poco::NumberFormatter::format(rows));
    return rows;
}


void
ListViewImpl::addItemView(View* pView)
{
    QScrollArea* pNativeView = static_cast<QScrollArea*>(_pNativeView);

    Omm::Gui::Log::instance()->gui().debug("list view impl add item view");
    Omm::Gui::Log::instance()->gui().debug("list view impl viewport width: "
            + Poco::NumberFormatter::format(pNativeView->viewport()->geometry().width())
            + ", height: " + Poco::NumberFormatter::format(pNativeView->viewport()->geometry().height()));

    ListView* pListView =  static_cast<ListView*>(_pView);
    static_cast<QWidget*>(pView->getNativeView())->resize(pNativeView->viewport()->geometry().width(), pListView->_itemViewHeight);
    static_cast<QWidget*>(pView->getNativeView())->setParent(_pScrollWidget);
}


void
ListViewImpl::moveItemView(int row, View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("list view impl move item widget to row: " + Poco::NumberFormatter::format(row));
    emit moveWidgetSignal(row, pView);
}


void
ListViewImpl::updateScrollWidgetSize()
{
    QScrollArea* pNativeView = static_cast<QScrollArea*>(_pNativeView);

    Omm::Gui::Log::instance()->gui().debug("list view impl update scroll widget size");
    Omm::Gui::Log::instance()->gui().debug("list view impl width: "
            + Poco::NumberFormatter::format(pNativeView->geometry().width())
            + ", height: " + Poco::NumberFormatter::format(pNativeView->geometry().height()));

    ListView* pListView =  static_cast<ListView*>(_pView);
    ListModel* pListModel = static_cast<ListModel*>(_pView->getModel());
   _pScrollWidget->resize(pNativeView->geometry().width(), pListModel->totalItemCount() * pListView->_itemViewHeight);
}


int
ListViewImpl::getOffset()
{
//    Omm::Gui::Log::instance()->gui().debug("scroll widget offset: " + Poco::NumberFormatter::format(_pScrollWidget->geometry().y()));
    return _pScrollWidget->geometry().y();
}


void
ListViewImpl::moveWidgetSlot(int row, View* pView)
{
    ListView* pListView =  static_cast<ListView*>(_pView);
    static_cast<QWidget*>(pView->getNativeView())->move(0, pListView->_itemViewHeight * row);
}


void
ListViewImpl::viewScrolledSlot(int value)
{
    ListView* pListView =  static_cast<ListView*>(_pView);
    pListView->scrolledToRow(-getOffset() / pListView->_itemViewHeight);
}


void
ListViewImpl::resized(int width, int height)
{
    ListView* pListView =  static_cast<ListView*>(_pView);
    int rows = height / pListView->_itemViewHeight;
    Omm::Gui::Log::instance()->gui().debug("list view impl resize: " + Poco::NumberFormatter::format(rows));
    _pScrollWidget->resize(width, _pScrollWidget->height());
    pListView->resize(rows, width);
}


}  // namespace Omm
}  // namespace Gui
