/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

#ifndef Util_INCLUDED
#define Util_INCLUDED

#include <stack>

#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/ClassLoader.h>
#include <Poco/Environment.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>


namespace Omm {
namespace Util {

class Log
{
public:
    static Log* instance();

    Poco::Logger& util();
    Poco::Logger& plugin();

private:
    Log();

    static Log*     _pInstance;
    Poco::Logger*   _pUtilLogger;
    Poco::Logger*   _pPluginLogger;
};


class Home
{
public:
    static const std::string getHomePath();
    static const std::string getCachePath();
    static const std::string getConfigPath();

private:
    static std::string          _home;
    static std::string          _cache;
    static std::string          _config;
    static const std::string    _defaultHome;
    static Poco::FastMutex      _lock;
};


template<class C>
class PluginLoader
{
public:
    PluginLoader() :
        _pPluginLoader(new Poco::ClassLoader<C>),
#ifdef __WINDOWS__
        _pluginPath(":.")
#else
        _pluginPath(":/usr/local/lib/omm:/usr/lib/omm")
#endif
    {
    }


    ~PluginLoader()
    {
        delete _pPluginLoader;
    }


    C* load(const std::string& objectName, const std::string& className = "", const std::string& prefixName = "")
    {
        Log::instance()->util().information("plugin loader trying to load " + objectName);

        loadPlugin(objectName);
        Poco::StringTokenizer nameSplitter(objectName, "-");

        std::string classBase = className;
        if (className == "") {
            classBase = nameSplitter[0];
        }
        else if (Poco::icompare(className, nameSplitter[0])) {
            Log::instance()->util().error("wrong plugin library base class: " + className);
            throw Poco::NotFoundException();
        }

        std::string classPrefix = prefixName;
        if (prefixName == "") {
            try {
                classPrefix = nameSplitter[1];
            }
            catch (Poco::RangeException) {
                Log::instance()->util().error("wrong plugin library name: " + objectName);
                throw Poco::NotFoundException();
            }
        }
        Log::instance()->util().information("plugin loader successfully loaded " + objectName);

        return create(classPrefix, classBase);
    }


private:

    void loadPlugin(const std::string& name)
    {
        try {
            _pluginPath = Poco::Environment::get("OMM_PLUGIN_PATH") + _pluginPath;
            Log::instance()->util().debug("plugin loader OMM_PLUGIN_PATH is: " + _pluginPath);
        }
        catch (Poco::NotFoundException) {
            Log::instance()->util().debug("plugin loader OMM_PLUGIN_PATH not set, standard search path is: " + _pluginPath);
        }
        Poco::StringTokenizer pathSplitter(_pluginPath, ":");
        Poco::StringTokenizer::Iterator it;
        for (it = pathSplitter.begin(); it != pathSplitter.end(); ++it) {
            if (*it == "") {
                continue;
            }
            try {
                _pPluginLoader->loadLibrary((*it) + "/libommplugin-" + name + Poco::SharedLibrary::suffix());
            }
            catch (Poco::NotFoundException) {
                continue;
            }
            catch (Poco::LibraryLoadException) {
                continue;
            }
            break;
        }
        if (it == pathSplitter.end()) {
            throw Poco::NotFoundException();
        }
    }


    C* create(const std::string& classPrefix, const std::string& classBase)
    {
        C* pRes;
        std::string className = camelCase(classPrefix) + camelCase(classBase);
        try {
            pRes = _pPluginLoader->create(className);
        }
        catch (Poco::NotFoundException) {
            Log::instance()->util().error("could not create object of class " + className);
            throw Poco::NotFoundException();
        }
        return pRes;
    }


    std::string camelCase(const std::string& name)
    {
        return Poco::toUpper(name.substr(0, 1)) + name.substr(1);
    }


    Poco::ClassLoader<C>*    _pPluginLoader;
    std::string              _pluginPath;
};


class ConfigurablePlugin
    /// interface for plugins, that can be parametrized with options
{
public:
    virtual void setOption(const std::string& key, const std::string& value) {}
};


class Startable : Poco::Runnable
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;

    void startAsThread();
    void stopThread();

private:
    void run();

    Poco::Thread        _thread;
};


}  // namespace Omm
}  // namespace Util

#endif
