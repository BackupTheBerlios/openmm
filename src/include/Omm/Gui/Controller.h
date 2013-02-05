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

#ifndef Controller_INCLUDED
#define Controller_INCLUDED

#include <vector>
#include "Geometry.h"

namespace Omm {
namespace Gui {

class View;
class Model;
class Drag;

#define UPDATE_MODEL(CLASS, METHOD, ...) for (ModelIterator it = beginModel(); it != endModel(); ++it) \
{ CLASS* pCLASS = dynamic_cast<CLASS*>(*it); if (pCLASS) { pCLASS->METHOD(__VA_ARGS__); } }


class Controller
{
    friend class View;
    friend class Model;
    friend class ViewImpl;
    friend class ButtonViewImpl;

public:
    typedef enum {KeyReturn, KeyBack, KeyLeft, KeyRight, KeyUp, KeyDown,
            KeyMenu, KeyVolUp, KeyVolDown, KeyChanUp, KeyChanDown,
            KeyForward, KeyBackward, KeyPlay, KeyStop, KeyPause,
            KeyPlayPause, KeyMute, KeyRecord,
            KeyPowerOff, KeyWakeUp, KeyEject, KeyLast,
            KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ,
            KeyK, KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT,
            KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ
    } KeyCode;

    void attachModel(Model* pModel);
    void detachModel(Model* pModel);

    void syncModelViews();

//protected:
    virtual void presented() {}
    virtual void resized(int width, int height) {}
    virtual void selected() {}
    virtual void released() {}
    virtual void activated() {}
    virtual void keyPressed(KeyCode key) {}
    virtual void mouseHovered(const Position& pos) {}
    virtual void mouseMoved(const Position& pos) {}
    virtual void dragStarted() {}
    virtual void dragEntered(const Position& pos, Drag* pDrag) {}
    virtual void dragMoved(const Position& pos, Drag* pDrag) {}
    virtual void dragLeft() {}
    virtual void dropped(const Position& pos, Drag* pDrag) {}
    virtual void removedSubview(View* pView) {}
    virtual void addedSubview(View* pView) {}

    typedef std::vector<Model*>::iterator ModelIterator;
    ModelIterator beginModel();
    ModelIterator endModel();

    std::vector<Model*>     _models;
};


}  // namespace Omm
}  // namespace Gui

#endif
