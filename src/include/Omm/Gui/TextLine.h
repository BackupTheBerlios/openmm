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

#ifndef TextLine_INCLUDED
#define TextLine_INCLUDED

#include <string>
#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {


class TextLineController : public Controller
{
    friend class TextLineViewImpl;
    friend class TextLineSignalProxy;

protected:
    virtual void editedText(const std::string& text) {}
    virtual void changedText(const std::string& text) {}
};


class TextLineModel : public Model
{
public:
//    virtual const std::string& getTextLine() const;
    virtual std::string getTextLine();
    void setTextLine(const std::string& line);

private:
    std::string _line;
};


class TextLineView : public View
{
    friend class TextLineModel;

public:
    TextLineView(View* pParent = 0);

    void setAlignment(Alignment alignment);

//private:
    virtual void syncViewImpl();
};


class TextLine : public Widget<TextLineView, TextLineController, TextLineModel>
{
public:
    TextLine(View* pParent = 0);

    void setTextLine(const std::string& line);
};


}  // namespace Omm
}  // namespace Gui

#endif
