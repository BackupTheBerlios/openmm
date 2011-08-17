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

#ifndef Widget_INCLUDED
#define Widget_INCLUDED

#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>


namespace Omm {
namespace Gui {

class WidgetImpl;
//template<class C> NativeWidget;

class Widget
{
    friend class WidgetImpl;
    
public:
    Widget(Widget* pParent = 0);
    virtual ~Widget();

    void* getNativeWidget();

    virtual void showWidget();
    virtual void hideWidget();

    class SelectNotification : public Poco::Notification
    {
    public:
        SelectNotification();
    };

    void registerEventNotificationHandler(const Poco::AbstractObserver& observer);

protected:
    virtual void select();

    Poco::NotificationCenter    _eventNotificationCenter;

//private:
    void*                       _pImpl;
//    WidgetImpl*                 _pImpl;
//    template<class C> NativeWidget* _pNative;
};


}  // namespace Omm
}  // namespace Gui

#endif
