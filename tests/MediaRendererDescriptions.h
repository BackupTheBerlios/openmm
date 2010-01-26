#ifndef MEDIARENDERER_DESCRIPTIONS_H
#define MEDIARENDERER_DESCRIPTIONS_H

std::string MediaRenderer::m_deviceDescription =
"<?xml version=\"1.0\"?>\
<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\
  <specVersion>\
    <major>1</major>\
    <minor>0</minor>\
  </specVersion>\
  <device>\
    <deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>\
    <friendlyName>JammR</friendlyName>\
    <manufacturer>joerg at hakker dot de</manufacturer>\
    <manufacturerURL>http://hakker.de/jamm</manufacturerURL>\
    <modelName>Jamm Media Renderer</modelName>\
    <modelNumber>0.3</modelNumber>\
    <modelDescription>Jamm UPnP Suite's Media Renderer</modelDescription>\
    <modelURL>http://hakker.de/jamm</modelURL>\
    <UDN>uuid:23b0189c-549f-11dc-a7c7-001641597c49</UDN>\
    <serviceList>\
      <service>\
        <serviceType>urn:schemas-upnp-org:service:RenderingControl:1</serviceType>\
        <serviceId>urn:upnp-org:serviceId:RenderingControl</serviceId>\
        <SCPDURL>/RenderingControl.xml</SCPDURL>\
        <controlURL>/RenderingControl/Control</controlURL>\
        <eventSubURL>/RenderingControl/Event</eventSubURL>\
      </service>\
      <service>\
        <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\
        <serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>\
        <SCPDURL>/ConnectionManager.xml</SCPDURL>\
        <controlURL>/ConnectionManager/Control</controlURL>\
        <eventSubURL>/ConnectionManager/Event</eventSubURL>\
      </service>\
      <service>\
        <serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>\
        <serviceId>urn:upnp-org:serviceId:AVTransport</serviceId>\
        <SCPDURL>/AVTransport.xml</SCPDURL>\
        <controlURL>/AVTransport/Control</controlURL>\
        <eventSubURL>/AVTransport/Event</eventSubURL>\
      </service>\
    </serviceList>\
  </device>\
</root>\
";

std::string AVTransport::m_description =
"<?xml version=\"1.0\"?>\
<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\
  <specVersion>\
    <major>1</major>\
    <minor>0</minor>\
  </specVersion>\
  <actionList>\
    <action>\
      <name>SetAVTransportURI</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>CurrentURI</name>\
          <direction>in</direction>\
          <relatedStateVariable>AVTransportURI</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>CurrentURIMetaData</name>\
          <direction>in</direction>\
          <relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>GetMediaInfo</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>NrTracks</name>\
          <direction>out</direction>\
          <relatedStateVariable>NumberOfTracks</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>MediaDuration</name>\
          <direction>out</direction>\
          <relatedStateVariable>CurrentMediaDuration</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>CurrentURI</name>\
          <direction>out</direction>\
          <relatedStateVariable>AVTransportURI</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>CurrentURIMetaData</name>\
          <direction>out</direction>\
          <relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>NextURI</name>\
          <direction>out</direction>\
          <relatedStateVariable>NextAVTransportURI</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>NextURIMetaData</name>\
          <direction>out</direction>\
          <relatedStateVariable>NextAVTransportURIMetaData</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>PlayMedium</name>\
          <direction>out</direction>\
          <relatedStateVariable>PlaybackStorageMedium</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>RecordMedium</name>\
          <direction>out</direction>\
          <relatedStateVariable>RecordStorageMedium</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>WriteStatus</name>\
          <direction>out</direction>\
          <relatedStateVariable>RecordMediumWriteStatus</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>GetTransportInfo</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>CurrentTransportState</name>\
          <direction>out</direction>\
          <relatedStateVariable>TransportState</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>CurrentTransportStatus</name>\
          <direction>out</direction>\
          <relatedStateVariable>TransportStatus</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>CurrentSpeed</name>\
          <direction>out</direction>\
          <relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>GetPositionInfo</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>Track</name>\
          <direction>out</direction>\
          <relatedStateVariable>CurrentTrack</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>TrackDuration</name>\
          <direction>out</direction>\
          <relatedStateVariable>CurrentTrackDuration</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>TrackMetaData</name>\
          <direction>out</direction>\
          <relatedStateVariable>CurrentTrackMetaData</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>TrackURI</name>\
          <direction>out</direction>\
          <relatedStateVariable>CurrentTrackURI</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>RelTime</name>\
          <direction>out</direction>\
          <relatedStateVariable>RelativeTimePosition</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>AbsTime</name>\
          <direction>out</direction>\
          <relatedStateVariable>AbsoluteTimePosition</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>RelCount</name>\
          <direction>out</direction>\
          <relatedStateVariable>RelativeCounterPosition</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>AbsCount</name>\
          <direction>out</direction>\
          <relatedStateVariable>AbsoluteCounterPosition</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>GetDeviceCapabilities</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>PlayMedia</name>\
          <direction>out</direction>\
          <relatedStateVariable>PossiblePlaybackStorageMedia</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>RecMedia</name>\
          <direction>out</direction>\
          <relatedStateVariable>PossibleRecordStorageMedia</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>RecQualityModes</name>\
          <direction>out</direction>\
          <relatedStateVariable>PossibleRecordQualityModes</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>GetTransportSettings</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>PlayMode</name>\
          <direction>out</direction>\
          <relatedStateVariable>CurrentPlayMode</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>RecQualityMode</name>\
          <direction>out</direction>\
          <relatedStateVariable>CurrentRecordQualityMode</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>Stop</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>Play</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>Speed</name>\
          <direction>in</direction>\
          <relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>Pause</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>Seek</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>Unit</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_SeekMode</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>Target</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_SeekTarget</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>Next</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
    <action>\
      <name>Previous</name>\
      <argumentList>\
        <argument>\
          <name>InstanceID</name>\
          <direction>in</direction>\
          <relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
        </argument>\
      </argumentList>\
    </action>\
  </actionList>\
  <serviceStateTable>\
    <stateVariable sendEvents=\"no\">\
      <name>TransportState</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>STOPPED</allowedValue>\
        <allowedValue>PAUSED_PLAYBACK</allowedValue>\
        <allowedValue>PAUSED_RECORDING</allowedValue>\
        <allowedValue>PLAYING</allowedValue>\
        <allowedValue>RECORDING</allowedValue>\
        <allowedValue>TRANSITIONING</allowedValue>\
        <allowedValue>NO_MEDIA_PRESENT</allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>TransportStatus</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>OK</allowedValue>\
        <allowedValue>ERROR_OCCURRED</allowedValue>\
        <allowedValue> vendor-defined </allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>PlaybackStorageMedium</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>UNKNOWN</allowedValue>\
        <allowedValue>DV</allowedValue>\
        <allowedValue>MINI-DV</allowedValue>\
        <allowedValue>VHS</allowedValue>\
        <allowedValue>W-VHS</allowedValue>\
        <allowedValue>S-VHS</allowedValue>\
        <allowedValue>D-VHS</allowedValue>\
        <allowedValue>VHSC</allowedValue>\
        <allowedValue>VIDEO8</allowedValue>\
        <allowedValue>HI8</allowedValue>\
        <allowedValue>CD-ROM</allowedValue>\
        <allowedValue>CD-DA</allowedValue>\
        <allowedValue>CD-R</allowedValue>\
        <allowedValue>CD-RW</allowedValue>\
        <allowedValue>VIDEO-CD</allowedValue>\
        <allowedValue>SACD</allowedValue>\
        <allowedValue>MD-AUDIO</allowedValue>\
        <allowedValue>MD-PICTURE</allowedValue>\
        <allowedValue>DVD-ROM</allowedValue>\
        <allowedValue>DVD-VIDEO</allowedValue>\
        <allowedValue>DVD-R</allowedValue>\
        <allowedValue>DVD+RW</allowedValue>\
        <allowedValue>DVD-RW</allowedValue>\
        <allowedValue>DVD-RAM</allowedValue>\
        <allowedValue>DVD-AUDIO</allowedValue>\
        <allowedValue>DAT</allowedValue>\
        <allowedValue>LD</allowedValue>\
        <allowedValue>HDD</allowedValue>\
        <allowedValue>MICRO-MV</allowedValue>\
        <allowedValue>NETWORK</allowedValue>\
        <allowedValue>NONE</allowedValue>\
        <allowedValue>NOT_IMPLEMENTED</allowedValue>\
        <allowedValue> vendor-defined </allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>RecordStorageMedium</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>UNKNOWN</allowedValue>\
        <allowedValue>DV</allowedValue>\
        <allowedValue>MINI-DV</allowedValue>\
        <allowedValue>VHS</allowedValue>\
        <allowedValue>W-VHS</allowedValue>\
        <allowedValue>S-VHS</allowedValue>\
        <allowedValue>D-VHS</allowedValue>\
        <allowedValue>VHSC</allowedValue>\
        <allowedValue>VIDEO8</allowedValue>\
        <allowedValue>HI8</allowedValue>\
        <allowedValue>CD-ROM</allowedValue>\
        <allowedValue>CD-DA</allowedValue>\
        <allowedValue>CD-R</allowedValue>\
        <allowedValue>CD-RW</allowedValue>\
        <allowedValue>VIDEO-CD</allowedValue>\
        <allowedValue>SACD</allowedValue>\
        <allowedValue>MD-AUDIO</allowedValue>\
        <allowedValue>MD-PICTURE</allowedValue>\
        <allowedValue>DVD-ROM</allowedValue>\
        <allowedValue>DVD-VIDEO</allowedValue>\
        <allowedValue>DVD-R</allowedValue>\
        <allowedValue>DVD+RW</allowedValue>\
        <allowedValue>DVD-RW</allowedValue>\
        <allowedValue>DVD-RAM</allowedValue>\
        <allowedValue>DVD-AUDIO</allowedValue>\
        <allowedValue>DAT</allowedValue>\
        <allowedValue>LD</allowedValue>\
        <allowedValue>HDD</allowedValue>\
        <allowedValue>MICRO-MV</allowedValue>\
        <allowedValue>NETWORK</allowedValue>\
        <allowedValue>NONE</allowedValue>\
        <allowedValue>NOT_IMPLEMENTED</allowedValue>\
        <allowedValue> vendor-defined </allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>PossiblePlaybackStorageMedia</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>PossibleRecordStorageMedia</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentPlayMode</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>NORMAL</allowedValue>\
        <allowedValue>SHUFFLE</allowedValue>\
        <allowedValue>REPEAT_ONE</allowedValue>\
        <allowedValue>REPEAT_ALL</allowedValue>\
        <allowedValue>RANDOM</allowedValue>\
        <allowedValue>DIRECT_1</allowedValue>\
        <allowedValue>INTRO</allowedValue>\
      </allowedValueList>\
      <defaultValue>NORMAL</defaultValue>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>TransportPlaySpeed</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>1</allowedValue>\
        <allowedValue> vendor-defined </allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>RecordMediumWriteStatus</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>WRITABLE</allowedValue>\
        <allowedValue>PROTECTED</allowedValue>\
        <allowedValue>NOT_WRITABLE</allowedValue>\
        <allowedValue>UNKNOWN</allowedValue>\
        <allowedValue>NOT_IMPLEMENTED</allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentRecordQualityMode</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>0:EP</allowedValue>\
        <allowedValue>1:LP</allowedValue>\
        <allowedValue>2:SP</allowedValue>\
        <allowedValue>0:BASIC</allowedValue>\
        <allowedValue>1:MEDIUM</allowedValue>\
        <allowedValue>2:HIGH</allowedValue>\
        <allowedValue>NOT_IMPLEMENTED</allowedValue>\
        <allowedValue> vendor-defined </allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>PossibleRecordQualityModes</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>NumberOfTracks</name>\
      <dataType>ui4</dataType>\
      <allowedValueRange>\
        <minimum>0</minimum>\
        <maximum> vendor-defined </maximum>\
      </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentTrack</name>\
      <dataType>ui4</dataType>\
      <allowedValueRange>\
        <minimum>0</minimum>\
        <maximum> vendor-defined </maximum>\
        <step>1</step>\
      </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentTrackDuration</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentMediaDuration</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentTrackMetaData</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentTrackURI</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>AVTransportURI</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>AVTransportURIMetaData</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>NextAVTransportURI</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>NextAVTransportURIMetaData</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>RelativeTimePosition</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>AbsoluteTimePosition</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>RelativeCounterPosition</name>\
      <dataType>i4</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>AbsoluteCounterPosition</name>\
      <dataType>i4</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>CurrentTransportActions</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"yes\">\
      <name>LastChange</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>A_ARG_TYPE_SeekMode</name>\
      <dataType>string</dataType>\
      <allowedValueList>\
        <allowedValue>ABS_TIME</allowedValue>\
        <allowedValue>REL_TIME</allowedValue>\
        <allowedValue>ABS_COUNT</allowedValue>\
        <allowedValue>REL_COUNT</allowedValue>\
        <allowedValue>TRACK_NR</allowedValue>\
        <allowedValue>CHANNEL_FREQ</allowedValue>\
        <allowedValue>TAPE-INDEX</allowedValue>\
        <allowedValue>FRAME</allowedValue>\
      </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>A_ARG_TYPE_SeekTarget</name>\
      <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
      <name>A_ARG_TYPE_InstanceID</name>\
      <dataType>ui4</dataType>\
    </stateVariable>\
  </serviceStateTable>\
</scpd>\
";

std::string ConnectionManager::m_description =
"<?xml version=\"1.0\"?>\
<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\
    <specVersion>\
         <major>1</major>\
         <minor>0</minor>\
    </specVersion>\
    <actionList>\
         <action>\
              <name>GetProtocolInfo</name>\
              <argumentList>\
                    <argument>\
                         <name>Source</name>\
                         <direction>out</direction>\
<relatedStateVariable>SourceProtocolInfo</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>Sink</name>\
                         <direction>out</direction>\
<relatedStateVariable>SinkProtocolInfo</relatedStateVariable>\
                    </argument>\
              </argumentList>\
         </action>\
          <action>\
              <name>GetCurrentConnectionIDs</name>\
              <argumentList>\
                    <argument>\
                         <name>ConnectionIDs</name>\
                         <direction>out</direction>\
<relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>\
                    </argument>\
              </argumentList>\
          </action>\
          <action>\
              <name>GetCurrentConnectionInfo</name>\
              <argumentList>\
                    <argument>\
                         <name>ConnectionID</name>\
                         <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>RcsID</name>\
                         <direction>out</direction>\
<relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>AVTransportID</name>\
                         <direction>out</direction>\
<relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>ProtocolInfo</name>\
                         <direction>out</direction>\
<relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>PeerConnectionManager</name>\
                         <direction>out</direction>\
<relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>PeerConnectionID</name>\
                         <direction>out</direction>\
<relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>Direction</name>\
                         <direction>out</direction>\
<relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>\
                    </argument>\
                    <argument>\
                         <name>Status</name>\
                         <direction>out</direction>\
<relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>\
                    </argument>\
              </argumentList>\
          </action>\
    </actionList>\
    <serviceStateTable>\
          <stateVariable sendEvents=\"yes\">\
              <name>SourceProtocolInfo</name>\
              <dataType>string</dataType>\
          </stateVariable>\
          <stateVariable sendEvents=\"yes\">\
              <name>SinkProtocolInfo</name>\
              <dataType>string</dataType>\
          </stateVariable>\
          <stateVariable sendEvents=\"yes\">\
              <name>CurrentConnectionIDs</name>\
              <dataType>string</dataType>\
          </stateVariable>\
          <stateVariable sendEvents=\"no\">\
              <name>A_ARG_TYPE_ConnectionStatus</name>\
              <dataType>string</dataType>\
              <allowedValueList>\
                    <allowedValue>OK</allowedValue>\
                    <allowedValue>ContentFormatMismatch</allowedValue>\
                    <allowedValue>InsufficientBandwidth</allowedValue>\
                    <allowedValue>UnreliableChannel</allowedValue>\
                    <allowedValue>Unknown</allowedValue>\
              </allowedValueList>\
          </stateVariable>\
          <stateVariable sendEvents=\"no\">\
              <name>A_ARG_TYPE_ConnectionManager</name>\
              <dataType>string</dataType>\
          </stateVariable>\
          <stateVariable sendEvents=\"no\">\
              <name>A_ARG_TYPE_Direction</name>\
              <dataType>string</dataType>\
              <allowedValueList>\
                    <allowedValue>Input</allowedValue>\
                    <allowedValue>Output</allowedValue>\
              </allowedValueList>\
          </stateVariable>\
          <stateVariable sendEvents=\"no\">\
              <name>A_ARG_TYPE_ProtocolInfo</name>\
              <dataType>string</dataType>\
          </stateVariable>\
          <stateVariable sendEvents=\"no\">\
              <name>A_ARG_TYPE_ConnectionID</name>\
              <dataType>i4</dataType>\
          </stateVariable>\
          <stateVariable sendEvents=\"no\">\
              <name>A_ARG_TYPE_AVTransportID</name>\
              <dataType>i4</dataType>\
          </stateVariable>\
          <stateVariable sendEvents=\"no\">\
              <name>A_ARG_TYPE_RcsID</name>\
              <dataType>i4</dataType>\
          </stateVariable>\
    </serviceStateTable>\
</scpd>\
";

std::string RenderingControl::m_description =
"<?xml version=\"1.0\"?>\
<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\
  <specVersion>\
    <major>1</major>\
    <minor>0</minor>\
  </specVersion>\
  <actionList>\
    <action>\
    <name>ListPresets</name>\
       <argumentList>\
         <argument>\
            <name>InstanceID</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>CurrentPresetNameList</name>\
            <direction>out</direction>\
            <relatedStateVariable>PresetNameList</relatedStateVariable>\
         </argument>\
       </argumentList>\
    </action>\
    <action>\
    <name>SelectPreset</name>\
       <argumentList>\
         <argument>\
            <name>InstanceID</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>PresetName</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_PresetName</relatedStateVariable>\
         </argument>\
       </argumentList>\
    </action>\
    <action>\
    <name>GetMute</name>\
       <argumentList>\
         <argument>\
            <name>InstanceID</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>Channel</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>CurrentMute</name>\
            <direction>out</direction>\
            <relatedStateVariable>Mute</relatedStateVariable>\
         </argument>\
       </argumentList>\
    </action>\
    <action>\
    <name>SetMute</name>\
       <argumentList>\
         <argument>\
            <name>InstanceID</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>Channel</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>DesiredMute</name>\
            <direction>in</direction>\
            <relatedStateVariable>Mute</relatedStateVariable>\
         </argument>\
       </argumentList>\
    </action>\
    <action>\
    <name>GetVolume</name>\
       <argumentList>\
         <argument>\
            <name>InstanceID</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>Channel</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>CurrentVolume</name>\
            <direction>out</direction>\
            <relatedStateVariable>Volume</relatedStateVariable>\
         </argument>\
       </argumentList>\
    </action>\
    <action>\
    <name>SetVolume</name>\
       <argumentList>\
         <argument>\
            <name>InstanceID</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>Channel</name>\
            <direction>in</direction>\
<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\
         </argument>\
         <argument>\
            <name>DesiredVolume</name>\
            <direction>in</direction>\
            <relatedStateVariable>Volume</relatedStateVariable>\
         </argument>\
       </argumentList>\
    </action>\
  </actionList>\
  <serviceStateTable>\
    <stateVariable sendEvents=\"no\">\
       <name>PresetNameList</name>\
       <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"yes\">\
       <name>LastChange</name>\
       <dataType>string</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>Brightness</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>Contrast</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>Sharpness</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>RedVideoGain</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>GreenVideoGain</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>BlueVideoGain</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>RedVideoBlackLevel</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>GreenVideoBlackLevel</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>BlueVideoBlackLevel</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>ColorTemperature</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>HorizontalKeystone</name>\
       <dataType>i2</dataType>\
       <allowedValueRange>\
         <minimum>Vendor defined (Must be &lt;= 0)</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>VerticalKeystone</name>\
       <dataType>i2</dataType>\
       <allowedValueRange>\
         <minimum>Vendor defined (Must be &lt;= 0)</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>Mute</name>\
       <dataType>boolean</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>Volume</name>\
       <dataType>ui2</dataType>\
       <allowedValueRange>\
         <minimum>0</minimum>\
         <maximum>100</maximum>\
         <step>1</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>VolumeDB</name>\
       <dataType>i2</dataType>\
       <allowedValueRange>\
         <minimum>Vendor defined</minimum>\
         <maximum>Vendor defined</maximum>\
         <step>Vendor defined</step>\
       </allowedValueRange>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>Loudness</name>\
       <dataType>boolean</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>A_ARG_TYPE_Channel</name>\
       <dataType>string</dataType>\
       <allowedValueList>\
         <allowedValue>Master</allowedValue>\
         <allowedValue>LF</allowedValue>\
         <allowedValue>RF</allowedValue>\
         <allowedValue>CF</allowedValue>\
         <allowedValue>LFE</allowedValue>\
         <allowedValue>LS</allowedValue>\
         <allowedValue>RS</allowedValue>\
         <allowedValue>LFC</allowedValue>\
         <allowedValue>RFC</allowedValue>\
         <allowedValue>SD</allowedValue>\
         <allowedValue>SL</allowedValue>\
         <allowedValue>SR </allowedValue>\
         <allowedValue>T</allowedValue>\
         <allowedValue>B</allowedValue>\
         <allowedValue>Vendor defined</allowedValue>\
       </allowedValueList>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>A_ARG_TYPE_InstanceID</name>\
       <dataType>ui4</dataType>\
    </stateVariable>\
    <stateVariable sendEvents=\"no\">\
       <name>A_ARG_TYPE_PresetName</name>\
       <dataType>string</dataType>\
       <allowedValueList>\
         <allowedValue>FactoryDefaults</allowedValue>\
         <allowedValue>InstallationDefaults</allowedValue>\
         <allowedValue>Vendor defined</allowedValue>\
       </allowedValueList>\
    </stateVariable>\
  </serviceStateTable>\
</scpd>\
";

#endif

