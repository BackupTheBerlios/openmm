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

#include <QtGui>
#include <Poco/NumberFormatter.h>

#include "TextLineImpl.h"
#include "QtTextLineImpl.h"
#include "Gui/TextLine.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


TextLineViewImpl::TextLineViewImpl(View* pView)
{
//    Omm::Gui::Log::instance()->gui().debug("label view impl ctor");
    QLineEdit* pNativeView = new QLineEdit;
    TextLineSignalProxy* pSignalProxy = new TextLineSignalProxy(this);

    initViewImpl(pView, pNativeView, pSignalProxy);
}


TextLineViewImpl::~TextLineViewImpl()
{
}


void
TextLineViewImpl::setTextLine(const std::string& line)
{
    static_cast<QLineEdit*>(_pNativeView)->setText(QTextCodec::codecForName("UTF-8")->toUnicode(line.c_str()));
}


void
TextLineViewImpl::setAlignment(View::Alignment alignment)
{
    switch(alignment) {
        case View::AlignLeft:
            static_cast<QLineEdit*>(_pNativeView)->setAlignment(Qt::AlignLeft);
            break;
        case View::AlignCenter:
            static_cast<QLineEdit*>(_pNativeView)->setAlignment(Qt::AlignCenter);
            break;
        case View::AlignRight:
            static_cast<QLineEdit*>(_pNativeView)->setAlignment(Qt::AlignRight);
            break;
    }
}


void
TextLineSignalProxy::init()
{
    SignalProxy::init();
    connect(_pViewImpl->getNativeView(), SIGNAL(returnPressed()), this, SLOT(editingFinished()));
    connect(_pViewImpl->getNativeView(), SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(const QString&)));
}


void
TextLineSignalProxy::editingFinished()
{
    std::string text(static_cast<QLineEdit*>(_pViewImpl->getNativeView())->text().toStdString());
    Omm::Gui::Log::instance()->gui().debug("text line view impl, calling editedText virtual method: " + text);
    PROXY_NOTIFY_CONTROLLER(TextLineController, editedText, text);
}


void
TextLineSignalProxy::textEdited(const QString& text)
{
    Omm::Gui::Log::instance()->gui().debug("text line view impl, calling changedText virtual method: " + text.toStdString());
    PROXY_NOTIFY_CONTROLLER(TextLineController, changedText, text.toStdString());
}

}  // namespace Omm
}  // namespace Gui
