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

#include <QtGui>
#include "ViewImpl.h"

namespace Omm {
namespace Gui {

class View;


class ScrollAreaViewImpl : public ViewImpl
{
    Q_OBJECT

    friend class ScrollAreaView;
    friend class QtScrollArea;

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

private slots:
    void viewScrolledXSlot(int value);
    void viewScrolledYSlot(int value);

private:
    void resized(int width, int height);
    void presented();

    QWidget*                 _pScrollWidget;
};


}  // namespace Omm
}  // namespace Gui

#endif

