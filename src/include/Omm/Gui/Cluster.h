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

#ifndef Cluster_INCLUDED
#define Cluster_INCLUDED

#include <map>
#include <vector>

#include "View.h"
#include "Controller.h"
#include "Model.h"
#include "Image.h"

namespace Omm {
namespace Gui {


class ClusterController : public Controller
{
    friend class ClusterView;
    friend class GenericClusterViewImpl;
    friend class ColumnClusterViewImpl;

protected:
    virtual void insertedView(View* pView) {}
    virtual void movedView(View* pView) {}
    virtual void sizeConstraintReached(View* pView) {}
};


class ClusterView : public View
{
    friend class ClusterViewImpl;
    friend class ClusterStackedLayout;
    friend class SubClusterView;

public:
    static const std::string Native;
    static const std::string Generic;
    static const std::string Column;

    ClusterView(View* pParent = 0, const std::string& type = Native);
    virtual ~ClusterView();

    void insertView(View* pView, const std::string& label = "", int index = 0);
    std::string getConfiguration();
    void setConfiguration(const std::string& configuration);

    int getViewCount();
    int getCurrentViewIndex(); /// current view has focus
    void setCurrentViewIndex(int index);
    int getIndexFromView(View* pView);
    View* getViewFromIndex(int index);

    void setHandlesHidden(bool hidden = true);
    const int getHandleHeight();

private:
    virtual void removedSubview(View* pView);
};


class Cluster : public Widget<ClusterView, ClusterController, Model>
{
public:
    Cluster(View* pParent = 0) : Widget<ClusterView, ClusterController, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
