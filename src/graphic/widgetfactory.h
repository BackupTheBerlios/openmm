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
#ifndef WIDGETFACTORY_H
#define WIDGETFACTORY_H

#include "sharedlibrary.h"
#include "pagestackwidget.h"
#include "pagewidget.h"
#include "menuwidget.h"
#include "listbrowserwidget.h"
#include "popupmenuwidget.h"

#include <string>
using namespace std;

/**
Factory to produce instances for all widgets of one toolkit. Currently there are widgets for the Qt toolkit implemented.

	@author Jörg Bakker <joerg@hakker.de>
*/
class WidgetFactory{
public:
    enum ToolkitT {ToolkitQt};

    static WidgetFactory* instance();

    void setToolkit(ToolkitT toolkit);

    PageStackWidget* createPageStackWidget();
    PageWidget* createPageWidget();
    MenuWidget* createMenuWidget(Page *parent);
    ListBrowserWidget* createListBrowserWidget(Page *parent, vector<string> *cols);
    PopupMenuWidget* createPopupMenuWidget(Page *parent);

protected:
    WidgetFactory();
    ~WidgetFactory();

private:
    static WidgetFactory *m_instance;
    ToolkitT              m_toolkit;
    SharedLibrary        *m_lib;
    string                m_toolkitLibName;
    char*                 m_pageStackCtorName;
    char*                 m_pageCtorName;
    char*                 m_menuCtorName;
    char*                 m_listBrowserCtorName;
    char*                 m_popupMenuCtorName;
    PageStackWidget*    (*m_pageStackCtor)();
    PageStackWidget*    (*m_pageCtor)();
    PageStackWidget*    (*m_menuCtor)(Page *parent);
    PageStackWidget*    (*m_listBrowserCtor)(Page *parent, vector<string> *cols);
    PageStackWidget*    (*m_popupMenuCtor)(Page *parent);
};

#endif
