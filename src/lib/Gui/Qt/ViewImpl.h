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

class NativeView;


class ViewImpl
{
    friend class View;
    
public:
    ViewImpl(View* pView);
    virtual ~ViewImpl();

    View* getView();
    QWidget* getNativeView();
    void setNativeView(QWidget* pView);
    virtual void showView();
    virtual void hideView();
    virtual int widthView();
    virtual int heightView();
    virtual void resizeView(int width, int height);
    virtual void moveView(int x, int y);
    virtual void selected();
 
protected:
    ViewImpl(QWidget* pNativeWidget);
    ViewImpl(View* pView, QWidget* pNativeWidget);

    View*                       _pView;
    QWidget*                    _pNativeView;
};


class NativeView : public QWidget
{
    friend class ViewImpl;
    
public:
    NativeView(ViewImpl* pViewImpl, View* pParent = 0);

    virtual void mousePressEvent(QMouseEvent* pMouseEvent);

    ViewImpl*                 _pViewImpl;
};







}  // namespace Omm
}  // namespace Gui

#endif

