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

#include "ViewImpl.h"

class QMainWindow;
class QApplication;
class QString;


namespace Omm {
namespace Gui {

class Application;

class ApplicationImpl
{
public:
    friend class Application;
    
    ApplicationImpl(Application* pApplication);
    virtual ~ApplicationImpl();

    void resize(int width, int height);
    int width();
    int height();
    void setFullscreen(bool fullscreen);
    int run(int argc, char** argv);

    Application*            _pApplication;
    QMainWindow*            _pMainWindow;
    QApplication*           _pQtApplication;
    QString*                _pFullscreenStyleSheet;
    int                     _width;
    int                     _height;
};


}  // namespace Omm
}  // namespace Gui

#endif

