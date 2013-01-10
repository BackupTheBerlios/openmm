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

#include <Poco/NumberFormatter.h>

#include "ImageImpl.h"
#include "Gui/Image.h"
#include "Gui/GuiLogger.h"


namespace Omm {
namespace Gui {


ImageModelImpl::ImageModelImpl(Model* pModel) :
ModelImpl(pModel)
{
}


void
ImageModelImpl::setData(const std::string& data)
{
    NSData* pImageData = [NSData dataWithBytes:data.data() length:data.size()];
    if (pImageData == nil) {
        LOG(gui, error, "no image data");
        return;
    }
    UIImage* pImage = [[UIImage alloc] initWithData:pImageData];
    if (pImage == nil) {
        LOG(gui, error, "failed to create UIImage from data");
        return;
    }
    _pNativeModel = pImage;
}


ImageViewImpl::ImageViewImpl(View* pView)
{
    LOG(gui, debug, "image view impl ctor");
    UIImageView* pNativeView = [[UIImageView alloc] init];

    initViewImpl(pView, pNativeView);
}


ImageViewImpl::~ImageViewImpl()
{
}


void
ImageViewImpl::syncViewImpl()
{
    static_cast<UIImageView*>(_pNativeView).image = static_cast<UIImage*>(_pView->getNativeModel());
}


void
ImageViewImpl::setAlignment(View::Alignment alignment)
{
    // FIXME: implement setAlignment() with UIImage
    switch(alignment) {
        case View::AlignLeft:
//            static_cast<UIImageView*>(_pNativeView).textAlignment = UITextAlignmentLeft;
            break;
        case View::AlignCenter:
//            static_cast<UIImageView*>(_pNativeView).textAlignment = UITextAlignmentCenter;
            break;
        case View::AlignRight:
//            static_cast<UIImageView*>(_pNativeView).textAlignment = UITextAlignmentRight;
            break;
    }
}


void
ImageViewImpl::scaleBestFit(int width, int height)
{
    static_cast<UIImageView*>(_pNativeView).contentMode = UIViewContentModeScaleAspectFit;
}


}  // namespace Omm
}  // namespace Gui
