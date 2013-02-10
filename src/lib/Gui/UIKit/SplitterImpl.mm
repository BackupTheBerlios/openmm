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

#include "SplitterImpl.h"
#include "Gui/Splitter.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


SplitterViewImpl::SplitterViewImpl(View* pView, View::Orientation orientation)
{
    // TODO: implement UIKit SplitterViewImpl::SplitterViewImpl()
}


SplitterViewImpl::~SplitterViewImpl()
{
}


void
SplitterViewImpl::setOrientation(View::Orientation orientation)
{
    // TODO: implement UIKit SplitterViewImpl::insertView()
}


void
SplitterViewImpl::insertView(View* pView, int index)
{
    // TODO: implement UIKit SplitterViewImpl::insertView()
}


std::vector<float>
SplitterViewImpl::getSizes()
{
    // TODO: implement UIKit SplitterViewImpl::getSizes()
}


void
SplitterViewImpl::setSizes(const std::vector<float>& sizes)
{
    // TODO: implement UIKit SplitterViewImpl::setSizes()
}


void
SplitterViewImpl::setSize(int index, float size, bool visibleViewsOnly)
{
    // TODO: implement UIKit SplitterViewImpl::setSize()
}

}  // namespace Omm
}  // namespace Gui
