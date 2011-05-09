/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#ifndef QtVisual_INCLUDED
#define	QtVisual_INCLUDED

#include <QLabel>

#include <Omm/Sys.h>

class QtVisual : public QLabel, public Omm::Sys::Visual
{
    Q_OBJECT

    friend class QtAvInterface;

public:
    QtVisual(QWidget* pParent = 0);
    virtual ~QtVisual();

    virtual void show();
    virtual void hide();

    virtual void* getWindow();
    virtual VisualType getType();
    virtual void renderImage(const std::string& imageData);
    virtual void blank();

signals:
    void showMenu(bool show);
    void signalShowImage(const std::string& imageData);

private slots:
    void slotShowImage(const std::string& imageData);

private:
#ifdef __LINUX__
    Poco::UInt32    _x11Window;
#endif
};


#endif

