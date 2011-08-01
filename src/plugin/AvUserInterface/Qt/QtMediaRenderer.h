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

#ifndef QtMediaRenderer_INCLUDED
#define QtMediaRenderer_INCLUDED

#include <QtGui>

#include <Omm/UpnpAvController.h>

#include "QtWidgetDeviceGroup.h"

class QtMediaRendererWidget;


class QtMediaRenderer : public Omm::Av::CtlMediaRenderer
{
    friend class QtMediaRendererWidget;
    friend class QtMediaRendererGroup;

public:
    QtMediaRenderer();
    ~QtMediaRenderer();


    void setDeviceWidget(QtMediaRendererWidget* pWidget);
    QtMediaRendererWidget* getDeviceWidget();
    
    virtual void initController();

private:
    QtMediaRendererWidget*            _pMediaRendererWidget;
};


class QtMediaRendererWidget : public QLabel
{
    Q_OBJECT

    friend class QtMediaRenderer;
    friend class QtMediaRendererGroup;
    
public:
//    QtMediaRendererWidget();

signals:
    void showWidget();
    void hideWidget();

private:
    virtual void paintEvent(QPaintEvent* event);
    
    QtMediaRenderer*                _pMediaRenderer;
};



#endif

