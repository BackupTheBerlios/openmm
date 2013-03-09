/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2013                                                       |
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

#ifndef QtImageImpl_INCLUDED
#define QtImageImpl_INCLUDED

#include <QObject>

namespace Omm {
namespace Gui {

class ImageModelImpl;

class ImageModelSignalProxy : public QObject
{
    Q_OBJECT

    friend class ImageModelImpl;

public:
    ImageModelSignalProxy(ImageModelImpl* pModelImpl);

    void loadData();

signals:
    void loadDataSignal();

private slots:
    void loadDataSlot();

private:
    ImageModelImpl* _pModelImpl;
};


}  // namespace Omm
}  // namespace Gui

#endif
