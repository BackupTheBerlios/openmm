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

#ifndef ListImpl_INCLUDED
#define ListImpl_INCLUDED

#include <QtGui>
#include "ViewImpl.h"

namespace Omm {
namespace Gui {

class View;
class ListView;


class ListViewImpl : public QGraphicsView, public ViewImpl
{
    Q_OBJECT

    friend class ListView;

signals:
    void moveWidgetSignal(int targetRow, View* pView);
    void extendPoolSignal();

private slots:
    void moveWidgetSlot(int targetRow, View* pView);
    void extendPoolSlot();

private:
    ListViewImpl(View* pView, bool movableViews = false, View* pParent = 0);
    virtual ~ListViewImpl();

    int visibleRows();
    void addItemView(View* pView);
    void moveItemView(int row, View* pView);
    void extendViewPool();

    QGraphicsScene*                                   _pGraphicsScene;
    std::map<View*, QGraphicsProxyWidget*>            _proxyWidgets;
    bool                                              _movableViews;
};


}  // namespace Omm
}  // namespace Gui

#endif

