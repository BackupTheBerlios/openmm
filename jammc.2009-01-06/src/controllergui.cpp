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
#include <QtGui>
#include <QtDebug>

#include "controllergui.h"

ControllerGui::ControllerGui(QWidget *parent)
: QWidget(parent)
{
    sleep(1);
    m_controller.getServers();

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QTreeView* browserView = new QTreeView(this);
    browserView->setModel(m_controller.getBrowserModel());
    m_rendererLabel = new QLabel(this);
    vLayout->addWidget(browserView);
    vLayout->addWidget(m_rendererLabel);
    
    connect(&m_controller, SIGNAL(rendererAddedRemoved(QString, QString, bool)),
            this, SLOT(rendererAddedRemoved(QString, QString, bool)));
    
    setWindowTitle("JammC");
}


void ControllerGui::rendererAddedRemoved(QString uuid, QString name, bool add)
{
    if (add) {
        m_rendererLabel->setText(name);
    }
    else {
        m_rendererLabel->setText("");
    }
    qDebug() << "ControllerGui::rendererAddedRemoved()" << (add?"added":"removed") << "renderer:" << name << uuid;
}
