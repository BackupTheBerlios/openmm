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
        setBackgroundColor(Color(100, 100, 100, 255));
        hide(false);
        attachController(this);
    }


    virtual void mouseMoved(const Position& pos)
    {
        float size = 0.0;
        if (_pViewImpl->_orientation == View::Horizontal) {
            size = (float)(posX() + pos.x() - _pViewImpl->_views[_index]->posX()) / _pViewImpl->_pView->width();
        }
        else {
            size = (float)(posY() + pos.y() - _pViewImpl->_views[_index]->posY()) / _pViewImpl->_pView->height();
        }
        LOG(gui, debug, "splitter bar pos [" + Poco::NumberFormatter::format(posX()) + ", " + Poco::NumberFormatter::format(posY()) + "]");
        LOG(gui, debug, "mouse pos [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
        LOG(gui, debug, "_views[_index]->pos [" + Poco::NumberFormatter::format(_pViewImpl->_views[_index]->posX()) + ", " + Poco::NumberFormatter::format(_pViewImpl->_views[_index]->posY()) + "]");
        LOG(gui, debug, "_pView->width / height: [" + Poco::NumberFormatter::format(_pViewImpl->_pView->width()) + ", " + Poco::NumberFormatter::format(_pViewImpl->_pView->height()) + "]");
        LOG(gui, debug, "index: " + Poco::NumberFormatter::format(_index));
        LOG(gui, debug, "size: " + Poco::NumberFormatter::format(size));
        _pViewImpl->setSize(_index, size, true);
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
        _pViewImpl->printSizes();

        if (!_pViewImpl->_views.size()) {
            return;
        }
        int viewIndex = 0;
        int viewsVisibleCount = 0;
        float viewsVisibleSizeFactor = 0.0;
        std::vector<int> visibleIndex;
        for (std::vector<View*>::iterator it = _pViewImpl->_views.begin(); it != _pViewImpl->_views.end(); ++viewIndex, ++it) {
            if ((*it)->isVisible()) {
                viewsVisibleCount++;
                viewsVisibleSizeFactor += _pViewImpl->_sizes[viewIndex];
                visibleIndex.push_back(viewIndex);
            }
            if (viewIndex < _pViewImpl->_views.size() - 1) {
                _pViewImpl->_bars[viewIndex]->hide(false);
            }
        }
//        if (_pViewImpl->_sizeForVisibleViewsOnly) {
//            viewsVisibleSizeFactor = 1.0;
//        }
        int viewsVisibleSize = 0;
        int countVisibleIndex = 0;
        viewIndex = 0;
        int totalBarWidth = viewsVisibleCount > 1 ? _pViewImpl->_barWidth * (viewsVisibleCount - 1) : 0;
        for (std::vector<View*>::iterator it = _pViewImpl->_views.begin(); it != _pViewImpl->_views.end(); ++viewIndex, ++it) {
            if ((*it)->isVisible()) {
                countVisibleIndex++;
                int pos = 0;
                if (_pViewImpl->_orientation == View::Horizontal) {
                    int width = (_pView->width() - totalBarWidth) * _pViewImpl->_sizes[viewIndex] / viewsVisibleSizeFactor;
                    pos = viewsVisibleSize + (countVisibleIndex - 1) * _pViewImpl->_barWidth;
                    (*it)->resize(width, _pView->height());
                    (*it)->move(pos, 0);
                    viewsVisibleSize += width;
                }
                else {
                    int height = (_pView->height() - totalBarWidth) * _pViewImpl->_sizes[viewIndex] / viewsVisibleSizeFactor;
                    pos = viewsVisibleSize + (countVisibleIndex - 1) * _pViewImpl->_barWidth;
                    (*it)->resize(_pView->width(), height);
                    (*it)->move(0, pos);
                    viewsVisibleSize += height;
                }
            }
            if (countVisibleIndex < visibleIndex.size() && (*it)->isVisible()) {
                _pViewImpl->showBarAt(viewIndex, viewsVisibleSize + (countVisibleIndex - 1) * _pViewImpl->_barWidth);
            }
        }

        LOG(gui, debug, "Splitter view impl layout view finished.");
    }

    GenericSplitterViewImpl*    _pViewImpl;
};


class SplitterViewController : public Controller
{
    friend class GenericSplitterViewImpl;

    SplitterViewController(View* pSplitter) : _pSplitter(pSplitter) {}

    virtual void shown()
    {
        _pSplitter->updateLayout();
    }

    virtual void hidden()
    {
        _pSplitter->updateLayout();
    }

    View* _pSplitter;
};


GenericSplitterViewImpl::GenericSplitterViewImpl(View* pView, View::Orientation orientation) :
PlainViewImpl(pView),
_orientation(orientation),
#ifdef __IPHONE__
_barWidth(20)
#else
_barWidth(10)
#endif
//_sizeForVisibleViewsOnly(false)
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
    if (index > _views.size()) {
        index = _views.size() - 1;
    }
    _views.insert(_views.begin() + index, pView);
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
    pView->attachController(new SplitterViewController(_pView));
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
    if (_views.size() != _sizes.size()) {
        LOG(gui, error, "Splitter view impl set wrong number of size factors");
        return;
    }
    float sum = 0.0;
    for (std::vector<float>::const_iterator it = sizes.begin(); it != sizes.end(); ++it) {
        sum += *it;
    }
    if (sum < 0.0 || sum > 1.0) {
        LOG(gui, error, "Splitter view impl sum of size factors is not in [0.0, 1.0]");
        return;
    }
    _sizes = sizes;
//    _sizeForVisibleViewsOnly = false;
    _pView->updateLayout();
}


void
GenericSplitterViewImpl::setSize(int index, float size, bool visibleViewsOnly)
{
    if (index < 0 || index >= _views.size() - 1 || size < 0.0 || size > 1.0) {
        return;
    }
    int nextVisibleIndex = index + 1;
//    if (visibleViewsOnly) {
        while (nextVisibleIndex < _views.size() && !_views[nextVisibleIndex]->isVisible()) {
            nextVisibleIndex++;
            LOG(gui, debug, "next visible index (loop): " + Poco::NumberFormatter::format(nextVisibleIndex));
        }
        if (!_views[nextVisibleIndex]->isVisible()) {
            return;
        }
//    }
    LOG(gui, debug, "next visible index: " + Poco::NumberFormatter::format(nextVisibleIndex));

    float sumSizes = _sizes[index] + _sizes[nextVisibleIndex];
    if (size <= sumSizes) {
        _sizes[index] = size;
        _sizes[nextVisibleIndex] = sumSizes - size;
    }
    else {
        // TODO: also resize all other views of splitter view
    }
//    _sizeForVisibleViewsOnly = visibleViewsOnly;
    _pView->updateLayout();
}


void
GenericSplitterViewImpl::showBarAt(int index, int pos)
{
    LOG(gui, debug, "show bar at ...");

    if (_orientation == View::Horizontal) {
        _bars[index]->move(pos, 0);
        _bars[index]->resize(_barWidth, _pView->height());
    }
    else {
        _bars[index]->move(0, pos);
        _bars[index]->resize(_pView->width(), _barWidth);
    }
    _bars[index]->show(false);

    LOG(gui, debug, "show bar at finished.");
}


void
GenericSplitterViewImpl::printSizes()
{
    std::string sizesString;
    for (std::vector<float>::const_iterator it = _sizes.begin(); it != _sizes.end(); ++it) {
        sizesString += Poco::NumberFormatter::format(*it) + " ";
    }
    LOG(gui, debug, "splitter sizes: " + sizesString);
}

}  // namespace Omm
}  // namespace Gui
