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

#ifndef WebBrowser_INCLUDED
#define WebBrowser_INCLUDED

#include <string>
#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {


class WebBrowserController : public Controller
{
};


class WebBrowserModel : public Model
{
public:
    WebBrowserModel();

    virtual std::string getUri();
    void setUri(const std::string& uri);

private:
    std::string _uri;
};


class WebBrowserView : public View
{
    friend class WebBrowserModel;

public:
    WebBrowserView(View* pParent = 0);

//private:
    virtual void syncViewImpl();
};


class WebBrowser : public Widget<WebBrowserView, WebBrowserController, WebBrowserModel>
{
public:
    WebBrowser(View* pParent = 0) : Widget<WebBrowserView, WebBrowserController, WebBrowserModel>(pParent) {}

    void setUri(const std::string& uri);
};


}  // namespace Omm
}  // namespace Gui

#endif
