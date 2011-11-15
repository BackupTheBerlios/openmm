#ifndef UPNPAV_DESCRIPTIONS_H
#define UPNPAV_DESCRIPTIONS_H

// FIXME: respect attributes (with namespace) when rewriting device description
// <dlna:X_DLNADOC xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">DMS-1.50</dlna:X_DLNADOC>

#include "Upnp.h"
#include "UpnpInternal.h"

namespace Omm {
namespace Av {


class MediaRendererDescriptions : public DescriptionProvider
{
public:
    MediaRendererDescriptions();
};


class MediaServerDescriptions : public DescriptionProvider
{
public:
    MediaServerDescriptions();
};


} // namespace Av
} // namespace Omm

#endif

