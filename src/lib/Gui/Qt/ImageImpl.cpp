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

#include <QtGui>
#include <Poco/NumberFormatter.h>

#include "ImageImpl.h"
#include "QtImageImpl.h"
#include "Gui/Image.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


ImageModelSignalProxy::ImageModelSignalProxy(ImageModelImpl* pModelImpl) :
_pModelImpl(pModelImpl)
{
    connect(this, SIGNAL(loadDataSignal()), this, SLOT(loadDataSlot()));
}


void
ImageModelSignalProxy::loadData()
{
    emit loadDataSignal();
}


void
ImageModelSignalProxy::loadDataSlot()
{
    LOG(gui, debug, "Image model load data slot");

    QPixmap* pImage = new QPixmap;
    pImage->loadFromData((const uchar*)_pModelImpl->_pData->data(), _pModelImpl->_pData->size(), 0);
    _pModelImpl->_pNativeModel = pImage;
}


ImageModelImpl::ImageModelImpl(Model* pModel) :
ModelImpl(pModel)
{
    _pSignalProxy = new ImageModelSignalProxy(this);
    // move image data model signal proxy object to main thread
    // in Qt, each object's slots are processed in the thread the object is created
    // other signal proxies are created when the view is created (always the main thread), so no problem there
    _pSignalProxy->moveToThread(QApplication::instance()->thread());
}


ImageModelImpl::~ImageModelImpl()
{
    delete _pSignalProxy;
}


void
ImageModelImpl::setData(const std::string& data)
{
    _pData = &data;
    _pSignalProxy->loadData();

//    QPixmap* pImage = new QPixmap;
//    pImage->loadFromData((const uchar*)data.data(), data.size(), 0);
//    _pNativeModel = pImage;
}


ImageViewImpl::ImageViewImpl(View* pView)
{
//    LOG(gui, debug, "image view impl ctor");
    QLabel* pNativeView = new QLabel;
//    pNativeView->setScaledContents(true);
//    _pImage = new QPixmap;

    initViewImpl(pView, pNativeView);
}


ImageViewImpl::~ImageViewImpl()
{
}


//void
//ImageViewImpl::setData(const std::string& data)
//{
//    QPixmap* pImage = static_cast<QPixmap*>(_pImage);
//    pImage->loadFromData((const uchar*)data.data(), data.size(), 0);
////    static_cast<QLabel*>(_pNativeView)->setPixmap(*pImage);
////    static_cast<QLabel*>(_pNativeView)->setPixmap(pImage->scaledToWidth(_pView->width()));
////    static_cast<QLabel*>(_pNativeView)->setPixmap(pImage->scaled(_pView->width(View::Pref), _pView->height(View::Pref), Qt::KeepAspectRatio));
//    if (!pImage->isNull()) {
//        static_cast<QLabel*>(_pNativeView)->setPixmap(pImage->scaled(_pView->width(), _pView->height(), Qt::KeepAspectRatio));
//    }
//}


void
ImageViewImpl::syncViewImpl()
{
    QPixmap* pImage = static_cast<QPixmap*>(_pView->getNativeModel());
    if (pImage && !pImage->isNull()) {
    //    static_cast<QLabel*>(_pNativeView)->setPixmap(*pImage);
    //    static_cast<QLabel*>(_pNativeView)->setPixmap(pImage->scaledToWidth(_pView->width()));
    //    static_cast<QLabel*>(_pNativeView)->setPixmap(pImage->scaled(_pView->width(View::Pref), _pView->height(View::Pref), Qt::KeepAspectRatio));
        static_cast<QLabel*>(_pNativeView)->setPixmap(pImage->scaled(_pView->width(), _pView->height(), Qt::KeepAspectRatio));
    }
}


void
ImageViewImpl::setAlignment(View::Alignment alignment)
{
    switch(alignment) {
        case View::AlignLeft:
            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignLeft);
            break;
        case View::AlignCenter:
            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignCenter);
            break;
        case View::AlignRight:
            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignRight);
            break;
    }
}


void
ImageViewImpl::scaleBestFit(int width, int height)
{
    QPixmap* pImage = static_cast<QPixmap*>(_pView->getNativeModel());
    if (pImage && !pImage->isNull()) {
        static_cast<QLabel*>(_pNativeView)->setPixmap(pImage->scaled(width, height, Qt::KeepAspectRatio));
    }
}


}  // namespace Omm
}  // namespace Gui
