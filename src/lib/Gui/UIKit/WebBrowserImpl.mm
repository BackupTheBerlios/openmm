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
    UIWebView* pNativeView = [[UIWebView alloc] init];
    initViewImpl(pView, pNativeView);
}


WebBrowserViewImpl::~WebBrowserViewImpl()
{
}


void
WebBrowserViewImpl::setUri(const std::string& uri)
{
    NSString* pUrlString = [[NSString alloc] initWithUTF8String:uri.c_str()];
//    NSURL* pUrl = [NSURL URLWithString:pUrlString];
//    NSURLRequest* pUrlRequest = [NSURLRequest requestWithURL:pUrl];
//    [static_cast<UIWebView*>(_pNativeView) loadRequest:pUrlRequest];
    [static_cast<UIWebView*>(_pNativeView) loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:pUrlString]]];
}


}  // namespace Omm
}  // namespace Gui
