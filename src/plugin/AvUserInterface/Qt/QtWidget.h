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

#ifndef QtWidget_INCLUDED
#define QtWidget_INCLUDED

#include <QtGui>

#include <Omm/Util.h>


class QtWidget : public QWidget, public Omm::Util::Widget
{
public:
    QtWidget(QWidget* pParent = 0);

    virtual void showWidget();
    virtual void hideWidget();

private:
    virtual void mousePressEvent(QMouseEvent* pMouseEvent);
};


class QtListWidget : public QtWidget
{
    Q_OBJECT

    friend class QtMediaServerGroup;

public:
    QtListWidget(QWidget* pParent = 0);

signals:
    void showWidgetSignal();
    void hideWidgetSignal();
    void configureWidget();
    // NOTE: unconfigureWidget() isn't needed. Widget must only be configured correct,
    // that means, values have to be cached to allow for fast scrolling (don't retrieve
    // them via network for example).
    void unconfigureWidget();

public slots:
    virtual void configure() {}
    virtual void unconfigure() {}
};


class QtSimpleListWidget : public QtListWidget
{
    Q_OBJECT

public:
    QtSimpleListWidget(QWidget* pParent = 0);
    virtual ~QtSimpleListWidget();

protected:
    void setLabel(const std::string& text);

private:
    QHBoxLayout*                    _pLayout;
    QLabel*                         _pNameLabel;
};

#endif

