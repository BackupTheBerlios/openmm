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
#include <qt4/QtCore/qnamespace.h>
#include <qt4/QtCore/qcoreevent.h>

#include "SelectionViewImpl.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


class QtSelectionView : public QWidget
{
public:

    virtual void paintEvent(QPaintEvent* pEvent)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        float lineWidth = 1.0;
        float rX = 4.0;
        float rY = 4.0;
        float w = width();
        float h = height();
        QPainterPath roundRectPath;
        roundRectPath.moveTo(0.0, rY);
        roundRectPath.lineTo(0.0, h - rY);
        roundRectPath.arcTo(0.0, h - 2 * rY, 2 * rX, 2 * rY, 180.0, 90.0);
        roundRectPath.lineTo(w - rX, h);
        roundRectPath.arcTo(w - 2 * rX, h - 2 * rY, 2 * rX, 2 * rY, 270.0, 90.0);
        roundRectPath.lineTo(w, rY);
        roundRectPath.arcTo(w - 2 * rX, 0.0, 2 * rX, 2 * rY, 0.0, 90.0);
        roundRectPath.lineTo(rX, 0.0);
        roundRectPath.arcTo(0.0, 0.0, 2 * rX, 2 * rY, 90.0, 90.0);
        roundRectPath.closeSubpath();

        QPen pen;
        QColor col(114, 4, 4);
        pen.setWidth(_type == 0 ? 2 * lineWidth : lineWidth);
        pen.setColor(col);
        painter.setPen(pen);
        painter.drawPath(roundRectPath);
        if (_type == 1) {
            QLinearGradient gradient(0, 0, 0, h);
            gradient.setColorAt(0, QColor(0, 0, 0, 0));
            gradient.setColorAt(1, QColor(114, 4, 4, 40));
            painter.fillPath(roundRectPath, QBrush(gradient));
//            painter.fillPath(roundRectPath, QColor(114, 4, 4, 40));
        }
    }

    int     _type;
};


SelectionViewImpl::SelectionViewImpl(View* pView, int type)
{
//    LOG(gui, debug, "label view impl ctor");
    QtSelectionView* pNativeView = new QtSelectionView;

    initViewImpl(pView, pNativeView);
    pNativeView->setAutoFillBackground(false);
    pNativeView->setAttribute(Qt::WA_TransparentForMouseEvents);
    pNativeView->_type = type;
}


SelectionViewImpl::~SelectionViewImpl()
{
}


}  // namespace Omm
}  // namespace Gui
