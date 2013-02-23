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

class AbstractClusterViewImpl;

/// Cluster is a composite view where all subviews can be arranged, hidden or shown individually
/// Each subview is assigned a handle to manage it interactively.
/// There are several types of clusters, currently tab views and column clusters.

class ClusterController : public Controller
{
    friend class ClusterView;
    friend class GenericClusterViewImpl;
    friend class ColumnClusterViewImpl;

protected:
    virtual void insertedView(View* pView) {}
    virtual void movedView(View* pView) {}
    virtual void changedConfiguration() {}
    // TODO: shownSubview(), hiddenSubview()
//    virtual void shownSubview(View* pView);
//    virtual void hiddenSubview(View* pView);
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

    ClusterView(View* pParent = 0, const std::string& type = "");
    virtual ~ClusterView();

    void insertView(View* pView, const std::string& label, int index = 0);
    /// Inserts view pView at index index into cluster, sets cluster as parent
    /// and makes it visible showing label on the attached handle.
    /// Index must be less than the number of visible views (otherwise ignored).
    /// Removing a view can only be done by assigning a new parent (e.g. setParent()
    /// or insertView()).

    void showViewAtIndex(View* pView, int index = 0);
    /// If view pView is already inserted, but hidden, it shows the view at index.
    /// If view pView is not inserted this does nothing.

    void hideView(View* pView);
    /// If view pView is already inserted and visible, it is hidden, but remains inserted
    /// If view pView is not inserted this does nothing.

    std::string getConfiguration();
    /// Returns the layout configuration of the visible views. Format of the layout configuration string depends
    /// on the cluster type and includes topological and possible metrical information.

    void setConfiguration(const std::string& configuration);
    /// Sets the layout configuration of the visible views. Any views that are not inserted
    /// are ignored.

    int getViewCount();
    /// Returns the number of visible views.

    int getCurrentViewIndex();
    /// Returns the index of the view, that is visible (and has focus if only one view is visible at a time).

    void setCurrentViewIndex(int index);
    /// Makes the view with index visible (and gives focus to it if only one view is visible at a time).

    int getIndexFromView(View* pView);
    /// Returns the index of view pView, -1 if view is not visible, and -2 if view is not in the cluster.

    View* getViewFromIndex(int index);
    /// Returns a pointer to the view with index and 0 if view is not visible.

    void setHandlesHidden(bool hidden = true);
    /// Hides/shows all handles.

    const int getHandleHeight();
    /// Returns the height of the handles.

    AbstractClusterViewImpl*    _pClusterImpl;
};


class Cluster : public Widget<ClusterView, ClusterController, Model>
{
public:
    Cluster(View* pParent = 0) : Widget<ClusterView, ClusterController, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
