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

#ifndef ClusterImpl_INCLUDED
#define ClusterImpl_INCLUDED

#include "ViewImpl.h"
#include "../AbstractClusterImpl.h"

namespace Omm {
namespace Gui {

class View;

class ClusterViewImpl : public AbstractClusterViewImpl, public ViewImpl
{
private:
    friend class ClusterView;

    ClusterViewImpl(View* pView);
    virtual ~ClusterViewImpl();

    virtual void insertView(View* pView, const std::string& label, int index);
    virtual void removeView(View* pView);
    virtual void showViewAtIndex(View* pView, int index);
    virtual void hideView(View* pView);
    virtual std::string getConfiguration();
    virtual void setConfiguration(const std::string& configuration);

    virtual int getViewCount();
    virtual int getCurrentViewIndex();
    virtual void setCurrentViewIndex(int index);
    virtual int getIndexFromView(View* pView);
    virtual View* getViewFromIndex(int index);

    virtual void setHandlesHidden(bool hidden = true);
    virtual const int getHandleHeight();

    std::vector<View*>  _views;
};


}  // namespace Omm
}  // namespace Gui

#endif

