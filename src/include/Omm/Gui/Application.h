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

#ifndef Application_INCLUDED
#define Application_INCLUDED


namespace Omm {
namespace Gui {

class View;
class ApplicationImpl;


class Application
{
    friend class ApplicationImpl;

public:
    Application();
    virtual ~Application();

    int run(int argc = 0, char** argv = 0);

    void resizeMainView(int width, int height);
    void scaleMainView(float factor);
    void setFullscreen(bool fullscreen);
    View* getMainView();
    int width();
    int height();
    void setToolBar(View* pView);
    void showToolBar(bool show = true);

    virtual View* createMainView() = 0;
    virtual void presentedMainView() {}
    virtual void finishedEventLoop() {}

private:
    void createdMainView();

    ApplicationImpl*           _pImpl;
    View*                      _pMainView;
    float                      _scaleFactor;
};


}  // namespace Omm
}  // namespace Gui

#endif
