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

#ifndef ListItemImpl_INCLUDED
#define ListItemImpl_INCLUDED

#include <QtGui>
#include "ViewImpl.h"

namespace Omm {
namespace Gui {

class View;


//class ListItemImpl : public QWidget, public ViewImpl
class ListItemImpl : public ViewImpl
{
    friend class ListItemView;

private:
    ListItemImpl(View* pView, View* pParent = 0);
    ~ListItemImpl();

    void setLabel(const std::string& text);

//    virtual void mousePressEvent(QMouseEvent* pMouseEvent);
//    virtual void showView();
//    virtual void hideView();
//    virtual int widthView();
//    virtual int heightView();
//    virtual void resizeView(int width, int height);
//    virtual void moveView(int x, int y);


    QWidget*                        _pWidget;
    QHBoxLayout*                    _pLayout;
    QLabel*                         _pNameLabel;
};


}  // namespace Omm
}  // namespace Gui


#endif

