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

#ifndef ApplicationImpl_INCLUDED
#define ApplicationImpl_INCLUDED

#include <set>

#include "ViewImpl.h"

class QMainWindow;
class QApplication;
class QToolBar;
class QStatusBar;
class QString;


namespace Omm {
namespace Gui {

class Application;
class QtEventFilter;


class ApplicationImpl
{
public:
    friend class Application;

    ApplicationImpl(Application* pApplication);
    virtual ~ApplicationImpl();

    void show(bool show);
    void resize(int width, int height);
    int width();
    int height();
    void setFullscreen(bool fullscreen);
    void addControlPanel(View* pView);
    void showControlPanels(bool show);
    int run(int argc, char** argv);
    void quit();

    Application*            _pApplication;
    QMainWindow*            _pMainWindow;
    QApplication*           _pQtApplication;
    std::set<QToolBar*>     _pToolBars;
    QtEventFilter*          _pEventFilter;
    QString*                _pFullscreenStyleSheet;
    bool                    _visible;
    int                     _width;
    int                     _height;
    bool                    _fullscreen;
};


}  // namespace Omm
}  // namespace Gui

#endif

