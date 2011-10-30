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

#ifndef ScrollAreaImpl_INCLUDED
#define ScrollAreaImpl_INCLUDED

#include "ViewImpl.h"

namespace Omm {
namespace Gui {

class View;


class ScrollAreaViewImpl : public ViewImpl
{
    friend class ScrollAreaView;
    friend class QtScrollArea;
    friend class ScrollAreaSignalProxy;

public:
    ScrollAreaViewImpl(View* pView);
    virtual ~ScrollAreaViewImpl();

protected:
    int getViewportWidth();
    int getViewportHeight();
    int getXOffset();
    int getYOffset();
    int getScrollAreaWidth();
    int getScrollAreaHeight();
    void resizeScrollArea(int width, int height);
    // need to override addSubview to operate on widget inside QScrollArea
    virtual void addSubview(View* pView);

private:
    QWidget*                 _pScrollWidget;
};


}  // namespace Omm
}  // namespace Gui

#endif

