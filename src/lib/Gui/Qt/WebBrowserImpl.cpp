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

#include <QtWebKit>

#include "WebBrowserImpl.h"
#include "Gui/WebBrowser.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


WebBrowserViewImpl::WebBrowserViewImpl(View* pView)
{
//    LOG(gui, debug, "WebBrowser view impl ctor");
    QWebView* pNativeView = new QWebView;
    QWebSettings* pSettings = QWebSettings::globalSettings();
    pSettings->setFontFamily(QWebSettings::StandardFont, "Helvetica");
    pSettings->setFontSize(QWebSettings::DefaultFontSize, 12);

    initViewImpl(pView, pNativeView);
}


WebBrowserViewImpl::~WebBrowserViewImpl()
{
}


void
WebBrowserViewImpl::setUri(const std::string& uri)
{
    static_cast<QWebView*>(_pNativeView)->setUrl(QUrl(QString::fromStdString(uri)));
}


}  // namespace Omm
}  // namespace Gui
