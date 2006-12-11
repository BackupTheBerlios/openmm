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
#include "debug.h"

#include <dlfcn.h>

// TODO: this looks a bit like too much duplicate code, ...

WidgetFactory *WidgetFactory::m_instance = 0;

WidgetFactory::WidgetFactory()
{
}


WidgetFactory::~WidgetFactory()
{
    delete m_lib;
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
    TRACE("WidgetFactory::setToolkit()");
    m_toolkit = toolkit;
    m_pageStackCtorName = "createPageStackWidget";
    m_pageCtorName = "createPageWidget";
    m_menuCtorName = "createMenuWidget";
    m_listBrowserCtorName = "createListBrowserWidget";
    m_popupMenuCtorName = "createPopupMenuWidget";

    switch(m_toolkit) {
    case ToolkitQt:
        //m_toolkitLibName = "libgraphic-qt.so";
        m_toolkitLibName = "/home/jb/devel/cc/jambin/src/graphic/qt/libjam-graphic-qt.so";
    default:
        break;
    }
    m_lib = new SharedLibrary(m_toolkitLibName);

    *(void **) (&m_pageStackCtor) = m_lib->resolve(m_pageStackCtorName);
    *(void **) (&m_pageCtor) = m_lib->resolve(m_pageCtorName);
    *(void **) (&m_menuCtor) = m_lib->resolve(m_menuCtorName);
    *(void **) (&m_listBrowserCtor) = m_lib->resolve(m_listBrowserCtorName);
    *(void **) (&m_popupMenuCtor) = m_lib->resolve(m_popupMenuCtorName);
}


PageStackWidget*
WidgetFactory::createPageStackWidget()
{
    TRACE("WidgetFactory::createPageStackWidget()");
    switch(m_toolkit) {
    case ToolkitQt:
        TRACE("WidgetFactory::createPageStackWidget() for toolkit Qt");
        //return new QtPageStack();
        return (PageStackWidget*) (*m_pageStackCtor)();
    default:
        return 0;
    }
}


PageWidget* 
WidgetFactory::createPageWidget()
{
    TRACE("WidgetFactory::createPage()");
    switch(m_toolkit) {
    case ToolkitQt:
        TRACE("WidgetFactory::createPage() for toolkit Qt");
        //return new QtPage();
        return (PageWidget*) (*m_pageCtor)();
    default:
        return 0;
    }
}


MenuWidget* 
WidgetFactory::createMenuWidget(Page *parent)
{
    TRACE("WidgetFactory::createMenu()");
    switch(m_toolkit) {
    case ToolkitQt:
        TRACE("WidgetFactory::createMenu() for toolkit Qt");
        //return new QtMenu(parent);
        return (MenuWidget*) (*m_menuCtor)(parent);
    default:
        return 0;
    }
}


ListBrowserWidget* 
WidgetFactory::createListBrowserWidget(Page *parent, QStringList *cols)
{
    TRACE("WidgetFactory::createListBrowser()");
    switch(m_toolkit) {
    case ToolkitQt:
        TRACE("WidgetFactory::createListBrowser() for toolkit Qt");
        //return new QtListBrowser(parent, cols);
        return (ListBrowserWidget*) (*m_listBrowserCtor)(parent, cols);
    default:
        return 0;
    }
}


PopupMenuWidget* 
WidgetFactory::createPopupMenuWidget(Page *parent)
{
    TRACE("WidgetFactory::createPopupMenu()");
    switch(m_toolkit) {
    case ToolkitQt:
        TRACE("WidgetFactory::createPopupMenu() for toolkit Qt");
        //return new QtPopupMenu(parent);
        return (PopupMenuWidget*) (*m_popupMenuCtor)(parent);
    default:
        return 0;
    }
}
