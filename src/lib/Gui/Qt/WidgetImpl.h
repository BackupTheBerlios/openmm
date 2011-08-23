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

#ifndef WidgetImpl_INCLUDED
#define WidgetImpl_INCLUDED

#include <QtGui>
#include "Gui/Widget.h"

namespace Omm {
namespace Gui {

class NativeWidget;


class WidgetImpl
{
    friend class Widget;
    
public:
    WidgetImpl(Widget* pWidget);
    virtual ~WidgetImpl();

    Widget* getWidget();
    QWidget* getNativeWidget();
    void setNativeWidget(QWidget* pWidget);
    virtual void showWidget();
    virtual void hideWidget();
    virtual void resizeWidget(int width, int height);
    virtual void select();
 
protected:
    WidgetImpl(QWidget* pNativeWidget);
    WidgetImpl(Widget* pWidget, QWidget* pNativeWidget);

    void postNotification(Poco::Notification::Ptr pNotification);

    Widget*                     _pWidget;
    QWidget*                    _pNativeWidget;
};


class NativeWidget : public QWidget
{
    friend class WidgetImpl;
    
public:
    NativeWidget(WidgetImpl* pWidgetImpl, Widget* pParent = 0);

    virtual void mousePressEvent(QMouseEvent* pMouseEvent);

    WidgetImpl*                 _pWidgetImpl;
};







}  // namespace Omm
}  // namespace Gui

#endif

