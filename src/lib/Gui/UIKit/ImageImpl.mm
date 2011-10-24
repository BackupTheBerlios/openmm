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


@interface OmmGuiImage : UIView
{
    Omm::Gui::ImageViewImpl*    _pImageViewImpl;
    UIImageView*                _pImageView;
}

@end


@implementation OmmGuiImage


- (id)initWithImpl:(Omm::Gui::ImageViewImpl*)pImpl
{
    Omm::Gui::Log::instance()->gui().debug("OmmGuiImage initWithImpl ...");
    if (self = [super init]) {
        _pImageViewImpl = pImpl;
        _pImageView = [UIImageView alloc];
    }
    return self;
}


- (id)setData:(NSData*)pImageData
{
    Omm::Gui::Log::instance()->gui().debug("OmmGuiImage setData ...");
    UIImage* pImage = [[UIImage alloc] initWithData:pImageData];
    [_pImageView initWithImage:pImage];
//    _pImageView.contentMode = UIViewContentModeScaleAspectFit;
//    _pImageView.frame = CGRectMake(0.0, 0.0, 100.0, 100.0);
    [self addSubview:_pImageView];
}


- (UIImage*)getImage
{
    return _pImageView.image;
}

//- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
//{
////    Omm::Gui::Log::instance()->gui().debug("Label view impl touch began");
//    _pImageViewImpl->pushed();
//    [super touchesBegan:touches withEvent:event];
//}

@end


namespace Omm {
namespace Gui {


ImageViewImpl::ImageViewImpl(View* pView)
{
    Log::instance()->gui().debug("image view impl ctor");
    OmmGuiImage* pNativeView = [[OmmGuiImage alloc] initWithImpl:this];

    initViewImpl(pView, pNativeView);
}


ImageViewImpl::~ImageViewImpl()
{
}


void
ImageViewImpl::setData(const std::string& data)
{
    Log::instance()->gui().debug("image view impl set data");
    NSData* pImageData = [NSData dataWithBytes:data.data() length:data.size()];
    if (pImageData == nil) {
        Log::instance()->gui().error("no image data");
        return;
    }
    [static_cast<OmmGuiImage*>(_pNativeView) setData:pImageData];
}


void
ImageViewImpl::setAlignment(View::Alignment alignment)
{
    // FIXME: implement setAlignment() with UIImage
    switch(alignment) {
        case View::AlignLeft:
//            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignLeft);
            break;
        case View::AlignCenter:
//            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignCenter);
            break;
        case View::AlignRight:
//            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignRight);
            break;
    }
}


void*
ImageViewImpl::getImage()
{
    return [static_cast<OmmGuiImage*>(_pNativeView) getImage];
}

}  // namespace Omm
}  // namespace Gui
