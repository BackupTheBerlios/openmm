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

#include <QtGui>

#include "SelectionViewImpl.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


class QtSelectionView : public QWidget
{
public:
//    void resizeEvent(QResizeEvent* pEvent)
//    {
//        if (pEvent->oldSize().height() > 0) {
//            LOG(gui, debug, "selection view resize");
//            mask(pEvent->size().width(), pEvent->size().height());
//        }
//    }

//    void showEvent(QShowEvent* event)
//    {
//        mask(width(), height());
////        repaint();
////        QWidget::show();
//    }

    void mask(int w, int h)
    {
        LOG(gui, debug, "selection view mask");

        QRegion outerRegion(rect());
        QRect innerRect(x() + 2, y() + 2, w - 4, h - 4);
        QRegion innerRegion(innerRect);
        setMask(outerRegion.xored(innerRegion));
    }
};


SelectionViewImpl::SelectionViewImpl(View* pView)
{
//    LOG(gui, debug, "label view impl ctor");
    QtSelectionView* pNativeView = new QtSelectionView;

//    pNativeView->hide();
//    pNativeView->mask(pNativeView->width(), pNativeView->height());

    initViewImpl(pView, pNativeView);
}


SelectionViewImpl::~SelectionViewImpl()
{
}


//void
//SelectionViewImpl::resize(int width, int height)
//{
//    static_cast<QtSelectionView*>(_pNativeView)->mask(width, height);
//}

}  // namespace Omm
}  // namespace Gui
