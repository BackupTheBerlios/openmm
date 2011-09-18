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

#include <Omm/Util.h>

#include "ListImpl.h"
#include "Gui/List.h"
#include "Gui/GuiLogger.h"


//@interface OmmGuiListView : UIScrollView<UIScrollViewDelegate>
//@interface OmmGuiListView : UIScrollView
@interface OmmGuiListViewDelegate : NSObject<UIScrollViewDelegate>
{
    Omm::Gui::ListViewImpl* _pListViewImpl;
}

@end


@implementation OmmGuiListViewDelegate

- (void)setImpl:(Omm::Gui::ListViewImpl*)pImpl
{
    _pListViewImpl = pImpl;
}


- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
//    Omm::Gui::Log::instance()->gui().debug("list view impl scrolling ...");
    _pListViewImpl->viewScrolled();
}

@end


namespace Omm {
namespace Gui {


ListViewImpl::ListViewImpl(View* pView)
{

//    OmmGuiListView* pNativeView = [[OmmGuiListView alloc] initWithFrame:CGRectMake(0.0, 50.0, 300.0, 400.0)];

    UIScrollView* pNativeView = [[UIScrollView alloc] init];
//    UIScrollView* pNativeView = [[UIScrollView alloc] initWithFrame:CGRectMake(0.0, 50.0, 300.0, 400.0)];

    OmmGuiListViewDelegate* pListViewDelegate = [[OmmGuiListViewDelegate alloc] init];
    [pListViewDelegate setImpl:this];
    pNativeView.delegate = pListViewDelegate;
    pNativeView.backgroundColor = [UIColor blueColor];

    initViewImpl(pView, pNativeView);
}


ListViewImpl::~ListViewImpl()
{
}


int
ListViewImpl::visibleRows()
{
    UIScrollView* pNativeView = static_cast<UIScrollView*>(getNativeView());

    ListView* pListView =  static_cast<ListView*>(_pView);
    int rows = pNativeView.frame.size.height / pListView->_itemViewHeight + 2;
    Omm::Gui::Log::instance()->gui().debug("list view impl number of visible rows: " + Poco::NumberFormatter::format(rows));
    return rows;
}


void
ListViewImpl::addItemView(View* pView)
{
    UIScrollView* pNativeView = static_cast<UIScrollView*>(getNativeView());

    ListView* pListView =  static_cast<ListView*>(_pView);
    pView->resize(pNativeView.frame.size.width, pListView->_itemViewHeight);
    [pNativeView addSubview:static_cast<UIView*>(pView->getNativeView())];
}


void
ListViewImpl::moveItemView(int row, View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("list view impl move item widget to row: " + Poco::NumberFormatter::format(row));
    ListView* pListView =  static_cast<ListView*>(_pView);
    pView->move(0, pListView->_itemViewHeight * row);
}


void
ListViewImpl::updateScrollWidgetSize()
{
    UIScrollView* pNativeView = static_cast<UIScrollView*>(getNativeView());

    ListView* pListView =  static_cast<ListView*>(_pView);
    ListModel* pListModel = static_cast<ListModel*>(_pView->getModel());
    pNativeView.contentSize = CGSizeMake(pNativeView.frame.size.width, pListModel->totalItemCount() * pListView->_itemViewHeight);
}


int
ListViewImpl::getOffset()
{
    return static_cast<UIScrollView*>(getNativeView()).contentOffset.y;
}


void
ListViewImpl::viewScrolled()
{
    ListView* pListView = static_cast<ListView*>(_pView);
    pListView->scrolledToRow(getOffset() / pListView->_itemViewHeight);
}


void
ListViewImpl::resized(int width, int height)
{
    ListView* pListView =  static_cast<ListView*>(_pView);
    int rows = height / pListView->_itemViewHeight;
    Omm::Gui::Log::instance()->gui().debug("list view impl resize: " + Poco::NumberFormatter::format(rows));
//    _pScrollWidget->resize(width, _pScrollWidget->height());
    pListView->resize(rows, width);
}


}  // namespace Omm
}  // namespace Gui
