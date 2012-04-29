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

#include <Omm/Sys.h>


namespace Omm {
namespace Gui {

class View;
class ApplicationImpl;


class Application : public Sys::SignalHandler
{
    friend class ApplicationImpl;

public:
    Application();
    virtual ~Application();

    int runEventLoop(int argc = 0, char** argv = 0);
    void quit();

    void showMainView(bool show = true);
    void resizeMainView(int width, int height);
    void scaleMainView(float factor);
    void setFullscreen(bool fullscreen);
    View* getMainView();
    int width();
    int height();
    void setToolBar(View* pView);
    void showToolBar(bool show = true);
    void setStatusBar(View* pView);
    void showStatusBar(bool show = true);

    virtual View* createMainView() = 0;
    virtual void presentedMainView() {}
    virtual void finishedEventLoop() {}
    virtual void start() {}
    virtual void stop() {}

private:
    void createdMainView();
    virtual void receivedSignal(SignalType signal);

    ApplicationImpl*           _pImpl;
    View*                      _pMainView;
    float                      _scaleFactor;
};


}  // namespace Omm
}  // namespace Gui

#endif
