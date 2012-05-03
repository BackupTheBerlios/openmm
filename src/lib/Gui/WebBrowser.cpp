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

#include <Poco/NumberFormatter.h>

#include "Gui/WebBrowser.h"
#include "Gui/GuiLogger.h"
#include "WebBrowserImpl.h"


namespace Omm {
namespace Gui {


WebBrowserModel::WebBrowserModel()
{
}


std::string
WebBrowserModel::getUri()
{
    return _uri;
}


void
WebBrowserModel::setUri(const std::string& uri)
{
//    Omm::Gui::Log::instance()->gui().debug("WebBrowser model set label");
    _uri = uri;
}


WebBrowserView::WebBrowserView(View* pParent) :
View(pParent, false)
{
//    Omm::Gui::Log::instance()->gui().debug("WebBrowser view ctor.");
    setName("WebBrowser view");

    _minWidth = 150;
    _minHeight = 100;
    _prefWidth = 150;
    _prefHeight = 100;
    _pImpl = new WebBrowserViewImpl(this);
}


void
WebBrowserView::syncViewImpl()
{
//    Omm::Gui::Log::instance()->gui().debug("WebBrowser view sync view: " + getName() + " ...");
    if (!_pModel) {
        return;
    }
    WebBrowserViewImpl* pImpl = static_cast<WebBrowserViewImpl*>(_pImpl);
    WebBrowserModel* pWebBrowserModel = static_cast<WebBrowserModel*>(_pModel);
    pImpl->setUri(pWebBrowserModel->getUri());
}


void
WebBrowser::setUri(const std::string& uri)
{
    WebBrowserModel::setUri(uri);
    syncView();
}


} // namespace Gui
} // namespace Omm
