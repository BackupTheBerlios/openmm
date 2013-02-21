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

#include <Poco/NumberFormatter.h>

#include "Gui/TextLine.h"
#include "Gui/GuiLogger.h"
#include "TextLineImpl.h"
#include "Log.h"


namespace Omm {
namespace Gui {


//const std::string&
//TextLineModel::getTextLine() const
//{
//    return _label;
//}


std::string
TextLineModel::getTextLine()
{
    return _line;
}


void
TextLineModel::setTextLine(const std::string& line)
{
//    LOG(gui, debug, "TextLine model set label");
    _line = line;
}


TextLineView::TextLineView(View* pParent) :
View(pParent, false)
{
//    LOG(gui, debug, "TextLine view ctor.");
    setName("label view");

    _minWidth = 50;
    _minHeight = 10;
    _prefWidth = 150;
    _prefHeight = 20;
    _pImpl = new TextLineViewImpl(this);
}


void
TextLineView::setAlignment(Alignment alignment)
{
    static_cast<TextLineViewImpl*>(_pImpl)->setAlignment(alignment);
}


void
TextLineView::syncViewImpl()
{
    LOG(gui, debug, "TextLine view sync view: " + getName());
    LOG(gui, debug, "TextLine view sync view with model: " + Poco::NumberFormatter::format(_pModel));
    TextLineModel* pTextLineModel = static_cast<TextLineModel*>(_pModel);
    TextLineViewImpl* pImpl = static_cast<TextLineViewImpl*>(_pImpl);
    if (pTextLineModel) {
        pImpl->setTextLine(pTextLineModel->getTextLine());
    }
}


class TextLineWidgetController : public TextLineController
{
    friend class TextLine;

    TextLineWidgetController(TextLineModel* pModel) : _pModel(pModel) {}

    virtual void editedText(const std::string& text)
    {
        _pModel->setTextLine(text);
    }

    virtual void changedText(const std::string& text)
    {
        _pModel->setTextLine(text);
    }

    TextLineModel*  _pModel;
};


TextLine::TextLine(View* pParent) :
Widget<TextLineView, TextLineController, TextLineModel>(pParent)
{
    attachController(new TextLineWidgetController(static_cast<TextLineModel*>(_pModel)));
}


void
TextLine::setTextLine(const std::string& line)
{
//    LOG(gui, debug, "TextLine model set label");
    TextLineModel::setTextLine(line);
    syncView();
}


} // namespace Gui
} // namespace Omm
