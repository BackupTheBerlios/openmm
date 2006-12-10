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
#include "widgetfactory.h"
#include "qtpagestack.h"
#include "qtpage.h"
#include "qtmenu.h"
#include "qtlistbrowser.h"
#include "qtpopupmenu.h"

#include <dlfcn.h>

// TODO: this looks a bit like too much similar code, ... (templates somehow?)

WidgetFactory *WidgetFactory::m_instance = 0;

WidgetFactory::WidgetFactory()
{
}


WidgetFactory::~WidgetFactory()
{
//    dlclose(m_toolkitLib);
}


WidgetFactory*
WidgetFactory::instance()
{
    if (m_instance == 0) {
        m_instance = new WidgetFactory();
    }
    return m_instance;
}


void
WidgetFactory::setToolkit(ToolkitT toolkit)
{
    m_toolkit = toolkit;
}


PageStackWidget*
WidgetFactory::createPageStackWidget()
{
    qDebug("WidgetFactory::createPageStackWidget()");
    switch(m_toolkit) {
    case ToolkitQt:
        qDebug("WidgetFactory::createPageStackWidget() for toolkit Qt");
        return new QtPageStack();
    default:
        return 0;
    }
}


PageWidget* 
WidgetFactory::createPageWidget()
{
    qDebug("WidgetFactory::createPage()");
    switch(m_toolkit) {
    case ToolkitQt:
        qDebug("WidgetFactory::createPage() for toolkit Qt");
        return new QtPage();
    default:
        return 0;
    }
}


MenuWidget* 
WidgetFactory::createMenuWidget()
{
    qDebug("WidgetFactory::createMenu()");
    switch(m_toolkit) {
    case ToolkitQt:
        qDebug("WidgetFactory::createMenu() for toolkit Qt");
        return new QtMenu();
    default:
        return 0;
    }
}


ListBrowserWidget* 
WidgetFactory::createListBrowserWidget(QStringList *cols)
{
    qDebug("WidgetFactory::createListBrowser()");
    switch(m_toolkit) {
    case ToolkitQt:
        qDebug("WidgetFactory::createListBrowser() for toolkit Qt");
        return new QtListBrowser(cols);
    default:
        return 0;
    }
}


PopupMenuWidget* 
WidgetFactory::createPopupMenuWidget(Page *parent)
{
    qDebug("WidgetFactory::createPopupMenu()");
    switch(m_toolkit) {
    case ToolkitQt:
        qDebug("WidgetFactory::createPopupMenu() for toolkit Qt");
        return new QtPopupMenu(parent);
    default:
        return 0;
    }
}
