SET( JAMM_SYSTEM_PLATIN_UPNP_SRCS
    Neptune.cpp
    NptBase64.cpp
    NptBufferedStreams.cpp
    NptCommon.cpp
    NptConsole.cpp
    NptDataBuffer.cpp
    NptDebug.cpp
    NptDirectory.cpp
    NptFile.cpp
    NptHttp.cpp
    NptList.cpp
    NptLogging.cpp
    NptMessaging.cpp
    NptNetwork.cpp
    NptQueue.cpp
    NptRingBuffer.cpp
    NptSimpleMessageQueue.cpp
    NptSockets.cpp
    NptStreams.cpp
    NptStrings.cpp
    NptSystem.cpp
    NptThreads.cpp
    NptTime.cpp
    NptUri.cpp
    NptUtils.cpp
    NptXml.cpp
    NptZip.cpp
    Platinum.cpp
    PltAction.cpp
    PltArgument.cpp
    PltCtrlPoint.cpp
    PltCtrlPointTask.cpp
    PltDatagramStream.cpp
    PltDeviceData.cpp
    PltDeviceHost.cpp
    PltDownloader.cpp
    PltEvent.cpp
    PltHttpClientTask.cpp
    PltHttp.cpp
    PltHttpServer.cpp
    PltHttpServerTask.cpp
    PltLeaks.cpp
    PltLog.cpp
    PltMetadataHandler.cpp
    PltRingBufferStream.cpp
    PltService.cpp
    PltSsdp.cpp
    PltStateVariable.cpp
    PltStreamPump.cpp
    PltTaskManager.cpp
    PltThreadTask.cpp
    PltUPnP.cpp
    System/Posix/NptPosixDirectory.cpp
    System/Posix/NptPosixQueue.cpp
    System/Posix/NptPosixSystem.cpp
    System/Posix/NptPosixThreads.cpp
    System/Posix/NptSelectableMessageQueue.cpp
    System/StdC/NptStdcConsole.cpp
    System/StdC/NptStdcDebug.cpp
    System/StdC/NptStdcEnvironment.cpp
    System/StdC/NptStdcFile.cpp
    System/Bsd/NptBsdNetwork.cpp
    System/Bsd/NptBsdSockets.cpp
    Devices/MediaRenderer/PltMediaRenderer.cpp
    Devices/MediaRenderer/PltMediaRendererSCPDs.cpp
    Devices/MediaRenderer/PltMediaController.cpp
    Devices/MediaServer/PltMediaCache.cpp
    Devices/MediaServer/PltMediaPlaylist.cpp
    Devices/MediaServer/PltMediaBrowser.cpp
    Devices/MediaServer/PltDidl.cpp
    Devices/MediaServer/PltMediaServer.cpp
    Devices/MediaServer/PltSyncMediaBrowser.cpp
    Devices/MediaServer/PltMediaItem.cpp
    Devices/MediaServer/PltMediaServerSCPDs.cpp
    Devices/MediaServer/PltFileMediaServer.cpp
    Devices/MediaConnect/PltMediaConnect.cpp
    Devices/MediaConnect/PltMediaConnectSCPDs.cpp
)

ADD_DEFINITIONS(
#     -D_REENTRANT -DNPT_DEBUG -DNPT_CONFIG_ENABLE_LOGGING -DNPT_CONFIG_ENABLE_ZIP
#     -D_REENTRANT -DNPT_DEBUG -DNPT_CONFIG_ENABLE_LOGGING
    -D_REENTRANT
)

ADD_LIBRARY( jamm-system-platin_upnp
    SHARED
    ${JAMM_SYSTEM_PLATIN_UPNP_SRCS}
)

TARGET_LINK_LIBRARIES( jamm-system-platin_upnp
#     -ldbus-1
#     z
)

INSTALL( TARGETS
    jamm-system-platin_upnp
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
