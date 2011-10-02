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

#include <fstream>

#include <Poco/NumberFormatter.h>

#include "Gui/Image.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ImageImpl.h"
#endif
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/ImageImpl.h"
#endif


namespace Omm {
namespace Gui {


const std::string&
ImageModel::getData() const
{
    return _data;
}


void
ImageModel::setData(const std::string& data)
{
//    Omm::Gui::Log::instance()->gui().debug("Image model set data");
    _data = data;

//    UPDATE_VIEWS(ImageView, setData, data);
    syncViews();
}


void
ImageModel::setFile(const std::string& fileName)
{
//    Omm::Gui::Log::instance()->gui().debug("Image model set file");
    std::ifstream ifs(fileName.c_str());
    const int bufSize = 512;
    char data[bufSize];
    while(ifs) {
        ifs.read(data, bufSize);
        _data += std::string(data, ifs.gcount());
    }
    syncViews();
}


ImageView::ImageView(View* pParent) :
View(pParent, false)
{
//    Omm::Gui::Log::instance()->gui().debug("Image view ctor.");
    _minWidth = 20;
    _minHeight = 20;
    _prefWidth = 50;
    _prefHeight = 50;
    _pImpl = new ImageViewImpl(this);
    setAlignment(View::AlignCenter);
}


void
ImageView::setAlignment(Alignment alignment)
{
    static_cast<ImageViewImpl*>(_pImpl)->setAlignment(alignment);
}


void
ImageView::syncView(Model* pModel)
{
    Omm::Gui::Log::instance()->gui().debug("Image view sync view: " + getName());
    ImageModel* pImageModel = static_cast<ImageModel*>(pModel);
    ImageViewImpl* pImpl = static_cast<ImageViewImpl*>(_pImpl);
    pImpl->setData(pImageModel->getData());
}


} // namespace Gui
} // namespace Omm
