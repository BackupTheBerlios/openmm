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


class ViewImpl : public QObject
{
    Q_OBJECT
    
    friend class View;
    
public:
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
    virtual void setSelected(bool selected);
    void setBackgroundColor(const Color& color);

protected:
    void initViewImpl(View* pView, QWidget* pNative);

    View*                       _pView;
    QWidget*                    _pNativeView;

signals:
    void showViewSignal();
};


class PlainViewImpl : public QWidget, public ViewImpl
{
public:
    PlainViewImpl(View* pView);

    virtual void mousePressEvent(QMouseEvent* pMouseEvent);
};







}  // namespace Omm
}  // namespace Gui

#endif

