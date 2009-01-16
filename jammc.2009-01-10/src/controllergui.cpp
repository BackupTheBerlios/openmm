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
#include <QtDebug>

#include "controllergui.h"

ControllerGui::ControllerGui(QWidget *parent)
: QTabWidget(parent)
{
    ui.setupUi(this);
    ui.m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui.m_playButton->setText("");
    ui.m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui.m_stopButton->setText("");
    ui.m_pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui.m_pauseButton->setText("");
    ui.m_skipForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui.m_skipForwardButton->setText("");
    ui.m_skipBackwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui.m_skipBackwardButton->setText("");
    ui.m_seekForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui.m_seekForwardButton->setText("");
    ui.m_seekBackwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui.m_seekBackwardButton->setText("");
    
    QButtonGroup* m_buttonGroup = new QButtonGroup(ui.m_controllerGroup);
    m_buttonGroup->addButton(ui.m_playButton);
    m_buttonGroup->addButton(ui.m_stopButton);
    m_buttonGroup->addButton(ui.m_pauseButton);
    m_buttonGroup->addButton(ui.m_skipForwardButton);
    m_buttonGroup->addButton(ui.m_skipBackwardButton);
    m_buttonGroup->addButton(ui.m_seekForwardButton);
    m_buttonGroup->addButton(ui.m_seekBackwardButton);
/*    connect(&m_controller, SIGNAL(rendererAddedRemoved(QString, QString, bool)),
            this, SLOT(rendererAddedRemoved(QString, QString, bool)));*/
    connect(ui.m_playButton, SIGNAL(pressed()), SIGNAL(playButtonPressed()));
    connect(ui.m_stopButton, SIGNAL(pressed()), SIGNAL(stopButtonPressed()));
    connect(ui.m_pauseButton, SIGNAL(pressed()), SIGNAL(pauseButtonPressed()));
    
    setWindowTitle("JammC");
}


void
ControllerGui::rendererAddedRemoved(QString uuid, QString name, bool add)
{
    if (add) {
//         m_rendererLabel->setText(name);
    }
    else {
//         m_rendererLabel->setText("");
    }
    qDebug() << "ControllerGui::rendererAddedRemoved()" << (add?"added":"removed") << "renderer:" << name << uuid;
}
