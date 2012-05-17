/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#ifndef UpnpAvPrivate_INCLUDED
#define UpnpAvPrivate_INCLUDED

#include <string>
#include <map>

#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>

#include "Upnp.h"
#include "UpnpInternal.h"

namespace Omm {
namespace Av {


class LastChangeSet
{
public:
    typedef std::map<std::string, std::map<std::string, std::string> >::iterator StateVarIterator;
    StateVarIterator beginStateVar();
    StateVarIterator endStateVar();

    typedef std::map<std::string, std::string>::iterator StateVarValIterator;
    StateVarValIterator beginStateVarVal(const std::string& stateVar);
    StateVarValIterator endStateVarVal(const std::string& stateVar);

    void setStateVarAttribute(const std::string& name, const std::string& attr, Variant& val);
    void writeStateVar(Poco::XML::Node* pNode);
    void clear();

    void readStateVars(Poco::XML::Node* pInstanceId);

private:
    std::map<std::string, std::map<std::string, std::string> >   _stateVars;
};


class LastChangeReader
{
public:
    typedef std::vector<LastChangeSet>::iterator ChangeSetIterator;
    ChangeSetIterator beginChangeSet();
    ChangeSetIterator endChangeSet();

    void read(const std::string& message);

private:
    Poco::AutoPtr<Poco::XML::Document>                                          _pDoc;
    std::vector<LastChangeSet>                                                  _changeSet;
};


class LastChange : public StateVar
{
public:
    LastChange(Service*& pServiceRef);
    virtual ~LastChange();

    void addInstance();

    virtual const std::string& getValue();
    void setStateVar(const ui4& InstanceID, const std::string& name, Variant& val);

protected:
    void notify();
    void write();
    void writeMessageHeader();
    void writeMessageClose();
    void writeMessageData();
    void clear();

    virtual void writeSchemeAttribute() = 0;

    void setStateVarAttribute(const ui4& InstanceID, const std::string& name, const std::string& attr, Variant& val);

    Poco::AutoPtr<Poco::XML::Element>                                           _pMessage;

private:
    Service*&                                                                   _pServiceRef;
    std::string                                                                 _message;
    Poco::AutoPtr<Poco::XML::Document>                                          _pDoc;
    std::vector<LastChangeSet>                                                  _changeSet;
    std::vector<LastChangeSet>                                                  _initialSet;
};


class AvTransportLastChange : public LastChange
{
public:
    AvTransportLastChange(Service*& pServiceRef);

private:
    virtual void writeSchemeAttribute();
};


class RenderingControlLastChange : public LastChange
{
public:
    RenderingControlLastChange(Service*& pServiceRef);

    void setChannelStateVar(const ui4& InstanceID, const std::string& channel, const std::string& name, Variant& val);

private:
    virtual void writeSchemeAttribute();
};


}  // namespace Omm
}  // namespace Av


#endif

