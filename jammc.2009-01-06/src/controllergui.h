/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CONTROLLERGUI_H
#define CONTROLLERGUI_H

#include <QWidget>

#include "upnpcontroller.h"

class QLabel;

class ControllerGui : public QWidget
{
    Q_OBJECT

public:
    ControllerGui(QWidget *parent = 0);

private slots:
    void rendererAddedRemoved(QString uuid, QString name, bool add);

private:
    QLabel *m_rendererLabel;

    UpnpController m_controller;
};

#endif //CONTROLLERGUI_H