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

#ifndef TreeClusterImpl_INCLUDED
#define TreeClusterImpl_INCLUDED

#include <map>
#include <vector>

#include "Gui/View.h"
#include "ViewImpl.h"
#include "SplitterImpl.h"
#include "AbstractClusterImpl.h"

namespace Omm {
namespace Gui {


class SplitterView;


//class TreeClusterViewImpl : public AbstractClusterViewImpl, public PlainViewImpl
class TreeClusterViewImpl : public AbstractClusterViewImpl, public SplitterViewImpl
{
    friend class TreeClusterController;

public:
    TreeClusterViewImpl(View* pView, bool createInitialCluster = true);

    virtual void init();
    virtual void insertView(View* pView, const std::string& name = "", int index = 0);
    virtual void removeView(View* pView);
    virtual int getViewCount();
    virtual int getCurrentViewIndex(); /// current view has focus
    virtual void setCurrentViewIndex(int index);
    virtual int getIndexFromView(View* pView);

    virtual void setHandlesHidden(bool hidden = true);
    virtual const int getHandleHeight();

private:
    void split(bool horizontal);
    void moveFirstCluster();
    void updateClusterController();
    void updateLayout();

    bool                                _createInitialCluster;
    ClusterView*                        _pCluster;
    ClusterView*                        _pFirstCluster;
    ClusterView*                        _pSecondCluster;

    static int                          _genericClusterNumber;
    static int                          _treeClusterNumber;
};


}  // namespace Omm
}  // namespace Gui

#endif
