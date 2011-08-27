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

#ifndef View_INCLUDED
#define View_INCLUDED


namespace Omm {
namespace Gui {

class ViewImpl;
class Model;


class View
{
    friend class ViewImpl;
    friend class Model;
    
public:
    View(View* pParent = 0);
    virtual ~View();

    void* getNativeView();
    View* getParent();

    void show();
    void hide();
    int width();
    int height();
    void resize(int width, int height);
    void move(int x, int y);

    Model* getModel();
    virtual void setModel(Model* pModel);

protected:
    View(ViewImpl* pViewImpl, View* pParent = 0);

    virtual void syncView() {}
    virtual void select() {}

    View*                       _pParent;
    ViewImpl*                   _pImpl;
    Model*                      _pModel;
};


class Model
{
    friend class View;
    
protected:
    View*     _pView;
};


}  // namespace Omm
}  // namespace Gui

#endif
