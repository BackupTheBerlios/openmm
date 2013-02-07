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
#include <vector>

#include "GenericSplitterImpl.h"
#include "Gui/GuiLogger.h"
#include "Gui/Layout.h"
#include "Gui/Drag.h"
#include "Gui/Splitter.h"
#include "Qt/ViewImpl.h"

namespace Omm {
namespace Gui {

class SplitterBar : public View, public Controller
{
    friend class GenericSplitterViewImpl;

    SplitterBar(View* pParent, GenericSplitterViewImpl* pViewImpl, int index) : View(pParent), _pViewImpl(pViewImpl), _index(index)
    {
        setName("splitter bar");
        attachController(this);
    }


    virtual void mouseMoved(const Position& pos)
    {
        if (_pViewImpl->_orientation == View::Horizontal) {
            float size = (float)(posX() + pos.x() - _pViewImpl->_views[_index]->posX()) / _pViewImpl->_pView->width();
//            LOG(gui, debug, "posX(): " + Poco::NumberFormatter::format(posX()));
//            LOG(gui, debug, "pos.x(): " + Poco::NumberFormatter::format(pos.x()));
//            LOG(gui, debug, "_views[_index]->posX(): " + Poco::NumberFormatter::format(_pViewImpl->_views[_index]->posX()));
//            LOG(gui, debug, "_pView->width(): " + Poco::NumberFormatter::format(_pViewImpl->_pView->width()));
//            LOG(gui, debug, "index: " + Poco::NumberFormatter::format(_index));
//            LOG(gui, debug, "size: " + Poco::NumberFormatter::format(size));
            _pViewImpl->setSize(_index, size);
        }
        else {
            _pViewImpl->setSize(_index, (float)(posY() + pos.y() - _pViewImpl->_views[_index]->posY()) / _pViewImpl->_pView->height());
        }
    }

    GenericSplitterViewImpl*    _pViewImpl;
    int                         _index;
};


class GenericSplitterLayout : public Layout
{
    friend class GenericSplitterViewImpl;

    GenericSplitterLayout(GenericSplitterViewImpl* pViewImpl) : _pViewImpl(pViewImpl) {}

    virtual void layoutView()
    {
        LOG(gui, debug, "Splitter view impl layout view ...");

        if (!_pViewImpl->_views.size()) {
            return;
        }
        int viewIndex = 0;
        int sumSize = 0;
        int totalBarWidth = _pViewImpl->_views.size() > 1 ? _pViewImpl->_barWidth * (_pViewImpl->_views.size() - 1) : 0;
        for (std::vector<View*>::iterator it = _pViewImpl->_views.begin(); it != _pViewImpl->_views.end(); ++viewIndex, ++it) {
            if (_pViewImpl->_orientation == View::Horizontal) {
                int width = 0;
                if (viewIndex < _pViewImpl->_sizes.size()) {
                    width = (_pView->width() - totalBarWidth) * _pViewImpl->_sizes[viewIndex];
                }
                else {
                    width = (_pView->width() - totalBarWidth) / _pViewImpl->_views.size();
                }
                int pos = sumSize + viewIndex * _pViewImpl->_barWidth;
                sumSize += width;
                (*it)->resize(width, _pView->height());
                (*it)->move(pos, 0);
                if (viewIndex) {
                    _pViewImpl->_bars[viewIndex - 1]->resize(_pViewImpl->_barWidth, _pView->height());
                    _pViewImpl->_bars[viewIndex - 1]->move(pos - _pViewImpl->_barWidth, 0);
                }
            }
            else {
                int height = 0;
                if (viewIndex < _pViewImpl->_sizes.size()) {
                    height = (_pView->height() - totalBarWidth) * _pViewImpl->_sizes[viewIndex];
                }
                else {
                    height = (_pView->height() - totalBarWidth) / _pViewImpl->_views.size();
                }
                int pos = sumSize + viewIndex * _pViewImpl->_barWidth;
                sumSize += height;
                (*it)->resize(_pView->width(), height);
                (*it)->move(0, pos);
                if (viewIndex) {
                    _pViewImpl->_bars[viewIndex - 1]->resize(_pView->width(), _pViewImpl->_barWidth);
                    _pViewImpl->_bars[viewIndex - 1]->move(0, pos - _pViewImpl->_barWidth);
                }
            }
        }

        LOG(gui, debug, "Splitter view impl layout view finished.");
    }

    GenericSplitterViewImpl*    _pViewImpl;
};


GenericSplitterViewImpl::GenericSplitterViewImpl(View* pView, View::Orientation orientation) :
PlainViewImpl(pView),
_orientation(orientation),
_barWidth(10)
{
    LOG(gui, debug, "Splitter view impl ctor.");
}


GenericSplitterViewImpl::~GenericSplitterViewImpl()
{
}


void
GenericSplitterViewImpl::init()
{
    _pView->setLayout(new GenericSplitterLayout(this));
}


void
GenericSplitterViewImpl::setOrientation(View::Orientation orientation)
{
    _orientation = orientation;
}


void
GenericSplitterViewImpl::insertView(View* pView, int index)
{
    LOG(gui, debug, "Splitter view impl insert view \"" + pView->getName() + "\" at index: " + Poco::NumberFormatter::format(index));

    pView->setParent(_pView);
    if (index < _views.size()) {
        _views.insert(_views.begin() + index, pView);
    }
    else {
        index = _views.size();
        _views.push_back(pView);
    }
    if (_views.size() > 1) {
        _bars.push_back(new SplitterBar(_pView, this, 0));
    }
    _sizes.clear();
    for (int i = 0; i < _views.size(); ++i) {
        _sizes.push_back(1.0 / _views.size());
        if (i < _views.size() - 1) {
            _bars[i]->_index = i;
        }
    }
    _pView->updateLayout();

    LOG(gui, debug, "Splitter view impl insert view finished");
}


std::vector<float>
GenericSplitterViewImpl::getSizes()
{
    return _sizes;
}


void
GenericSplitterViewImpl::setSizes(const std::vector<float>& sizes)
{
    _sizes = sizes;
    _pView->updateLayout();
}


void
GenericSplitterViewImpl::setSize(int index, float size)
{
    if (index < 0 || index >= _views.size() - 1 || size < 0.0 || size > 1.0) {
        return;
    }
    if (_views.size() != _sizes.size()) {
        return;
    }
    float sumSizes = _sizes[index] + _sizes[index + 1];
    if (size <= sumSizes) {
        _sizes[index] = size;
        _sizes[index + 1] = sumSizes - size;
    }
    else {

    }
    _pView->updateLayout();
}

}  // namespace Omm
}  // namespace Gui
