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

#ifndef Selector_INCLUDED
#define Selector_INCLUDED

#include <string>

#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "Image.h"


namespace Omm {
namespace Gui {


class SelectorController : public Controller
{
    friend class SelectorViewImpl;
    friend class SelectorSignalProxy;

protected:
    virtual void selected(int index) {}
};


class SelectorModel : public Model
{
public:
    virtual int totalItemCount() { return 0; }
    virtual std::string getItemLabel(int index) { return ""; }
    virtual Image* getItemImage(int index) { return 0; }
};


class SelectorView : public View
{
    friend class SelectorModel;

public:
    SelectorView(View* pParent = 0);

    int getCurrentIndex();
    void setCurrentIndex(int index);

private:
    virtual void syncViewImpl();
};


class Selector : public Widget<SelectorView, SelectorController, SelectorModel>
{
public:
    Selector(View* pParent = 0) : Widget<SelectorView, SelectorController, SelectorModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
