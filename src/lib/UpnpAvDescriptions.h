#ifndef UPNPAV_DESCRIPTIONS_H
#define UPNPAV_DESCRIPTIONS_H

// FIXME: respect attributes (with namespace) when rewriting device description
// <dlna:X_DLNADOC xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">DMS-1.50</dlna:X_DLNADOC>

#include "Upnp.h"

namespace Omm {
namespace Av {

class Descriptions
{
public:
    static std::string MediaRenderer;
    static std::string MediaServer;
    static std::string AVTransport;
    static std::string ConnectionManager;
    static std::string RenderingControl;
    static std::string ContentDirectory;
};


class MediaRendererDescriptions : public DeviceDescriptionProvider
{
public:
    MediaRendererDescriptions();
};


class MediaServerDescriptions : public DeviceDescriptionProvider
{
public:
    MediaServerDescriptions();
};


} // namespace Av
} // namespace Omm

#endif

