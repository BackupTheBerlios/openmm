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

#include "Gui/View.h"

namespace Omm {
namespace Gui {


class ViewImpl
{
    friend class View;
    
public:
    virtual ~ViewImpl();

    View* getView();
    void* getNativeView();
    void* getNativeViewController();
    void setNativeView(void* pView);
    virtual void showView();
    virtual void hideView();
    virtual int widthView();
    virtual int heightView();
    virtual void resizeView(int width, int height);
    virtual void moveView(int x, int y);
    virtual void setHighlighted(bool highlighted);
    void setBackgroundColor(const Color& color);

    View*                       _pView;
protected:
    void initViewImpl(View* pView, void* pNative);

    void*                       _pNativeView;
    void*                       _pNativeViewController;
    void*                       _pNativeViewSelectorDispatcher;
};


class PlainViewImpl : public ViewImpl
{
public:
    PlainViewImpl(View* pView);
};


}  // namespace Omm
}  // namespace Gui

#endif

