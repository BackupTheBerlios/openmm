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

#import <UIKit/UIKit.h>

#include "WebBrowserImpl.h"
#include "Gui/WebBrowser.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


WebBrowserViewImpl::WebBrowserViewImpl(View* pView)
{
//    Omm::Gui::Log::instance()->gui().debug("Label view impl ctor");
    UIWebView* pNativeView = [[UIWebView alloc] init];
//    pNativeView.backgroundColor = [[UIColor alloc] initWithWhite:1.0 alpha:0.0];
//    [pNativeView setTextColor:[UIColor blackColor]];
//    [pNativeView setImpl:this];

    initViewImpl(pView, pNativeView);
}


WebBrowserViewImpl::~WebBrowserViewImpl()
{
}


void
WebBrowserViewImpl::setUri(const std::string& uri)
{
    NSString* pUrl = [[NSString alloc] initWithUTF8String:uri.c_str()];
    [static_cast<UIWebView*>(_pNativeView) loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:pUrl]]];
}


}  // namespace Omm
}  // namespace Gui
