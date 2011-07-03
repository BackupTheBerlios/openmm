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

#ifndef UpnpAv_INCLUDED
#define UpnpAv_INCLUDED

#include <string>

#include "UpnpTypes.h"

namespace Omm {
namespace Av {


class ServiceType
{
public:
    const static std::string RC_1;
    const static std::string AVT_1;
    const static std::string CM_1;
    const static std::string CD_1;
};


class AvTransportEventedStateVar
{
public:
    const static std::string TRANSPORT_STATE;
    const static std::string TRANSPORT_STATUS;
    const static std::string PLAYBACK_STORAGE_MEDIUM;
    const static std::string POSSIBLE_PLAYBACK_STORAGE_MEDIA;
    const static std::string POSSIBLE_RECORD_STORAGE_MEDIA;
    const static std::string CURRENT_PLAY_MODE;
    const static std::string TRANSPORT_PLAY_SPEED;
    const static std::string RECORD_MEDIUM_WRITE_STATUS;
    const static std::string POSSIBLE_RECORD_QUALITY_MODES;
    const static std::string CURRENT_RECORD_QUALITY_MODE;
    const static std::string NUMBER_OF_TRACKS;
    const static std::string CURRENT_TRACK;
    const static std::string CURRENT_TRACK_DURATION;
    const static std::string CURRENT_MEDIA_DURATION;
    const static std::string CURRENT_TRACK_URI;
    const static std::string CURRENT_TRACK_META_DATA;
    const static std::string AVTRANSPORT_URI;
    const static std::string AVTRANSPORT_URI_META_DATA;
    const static std::string NEXT_AVTRANSPORT_URI;
    const static std::string NEXT_AVTRANSPORT_URI_META_DATA;
    const static std::string CURRENT_TRANSPORT_ACTIONS;
};


class AvTransportArgument
{
public:
    const static std::string TRANSPORT_STATE_STOPPED;
    const static std::string TRANSPORT_STATE_PLAYING;
    const static std::string TRANSPORT_STATE_TRANSITIONING;
    const static std::string TRANSPORT_STATE_PAUSED_PLAYBACK;
    const static std::string TRANSPORT_STATE_PAUSED_RECORDING;
    const static std::string TRANSPORT_STATE_RECORDING;
    const static std::string TRANSPORT_STATE_NO_MEDIA_PRESENT;
    const static std::string TRANSPORT_STATUS_OK;
    const static std::string TRANSPORT_STATUS_ERROR_OCCURRED;
    const static std::string PLAYBACK_STORAGE_MEDIUM_UNKNOWN;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DV;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MINI_DV;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_W_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_S_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_D_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VHSC;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VIDEO8;
    const static std::string PLAYBACK_STORAGE_MEDIUM_HI8;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_ROM;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_DA;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_R;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_RW;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VIDEO_CD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_SACD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MD_AUDIO;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MD_PICTURE;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_ROM;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_VIDEO;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_R;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_PLUS_RW;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_RW;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_RAM;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_AUDIO;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DAT;
    const static std::string PLAYBACK_STORAGE_MEDIUM_LD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_HDD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MICRO_MV;
    const static std::string PLAYBACK_STORAGE_MEDIUM_NETWORK;
    const static std::string PLAYBACK_STORAGE_MEDIUM_NONE;
    const static std::string PLAYBACK_STORAGE_MEDIUM_NOT_IMPLEMENTED;
    const static std::string CURRENT_PLAY_MODE_NORMAL;
    const static std::string CURRENT_PLAY_MODE_SHUFFLE;
    const static std::string CURRENT_PLAY_MODE_REPEAT_ONE;
    const static std::string CURRENT_PLAY_MODE_REPEAT_ALL;
    const static std::string CURRENT_PLAY_MODE_RANDOM;
    const static std::string CURRENT_PLAY_MODE_DIRECT_1;
    const static std::string CURRENT_PLAY_MODE_INTRO;
    const static std::string TRANSPORT_PLAY_SPEED_1;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_WRITABLE;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_PROTECTED;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_NOT_WRITABLE;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_UNKOWN;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_NOT_IMPLEMENTED;
    const static std::string CURRENT_RECORD_QUALITY_MODE_0_EP;
    const static std::string CURRENT_RECORD_QUALITY_MODE_1_LP;
    const static std::string CURRENT_RECORD_QUALITY_MODE_2_SP;
    const static std::string CURRENT_RECORD_QUALITY_MODE_0_BASIC;
    const static std::string CURRENT_RECORD_QUALITY_MODE_1_MEDIUM;
    const static std::string CURRENT_RECORD_QUALITY_MODE_2_HIGH;
    const static std::string CURRENT_RECORD_QUALITY_MODE_NOT_IMPLEMENTED;
    const static std::string CURRENT_TRACK_DURATION_0;
    const static std::string CURRENT_TRACK_META_DATA_NOT_IMPLEMENTED;
    const static std::string RELATIVE_TIME_POSITION_NOT_IMPLEMENTED;
    const static std::string CURRENT_TRANSPORT_ACTIONS_NOT_IMPLEMENTED;
    const static std::string SEEK_MODE_TRACK_NR;
    const static std::string SEEK_MODE_ABS_TIME;
    const static std::string SEEK_MODE_REL_TIME;
    const static std::string SEEK_MODE_ABS_COUNT;
    const static std::string SEEK_MODE_REL_COUNT;
    const static std::string SEEK_MODE_CHANNEL_FREQ;
    const static std::string SEEK_MODE_TAPE_INDEX;
    const static std::string SEEK_MODE_FRAME;
    const static i4 RELATIVE_COUNTER_POSITION_UNDEFINED;
};


class RenderingControlEventedStateVar
{
public:
    const static std::string PRESET_NAME_LIST;
    const static std::string BRIGHTNESS;
    const static std::string CONTRAST;
    const static std::string SHARPNESS;
    const static std::string RED_VIDEO_GAIN;
    const static std::string GREEN_VIDEO_GAIN;
    const static std::string BLUE_VIDEO_GAIN;
    const static std::string RED_BLACK_LEVEL;
    const static std::string GREEN_BLACK_LEVEL;
    const static std::string BLUE_BLACK_LEVEL;
    const static std::string COLOR_TEMPERATURE;
    const static std::string HORIZONTAL_KEYSTONE;
    const static std::string VERTICAL_KEYSTONE;
    const static std::string MUTE;
    const static std::string VOLUME;
    const static std::string VOLUME_DB;
    const static std::string LOUDNESS;
};


class AvChannel
{
public:
    const static std::string MASTER;
    const static std::string LF;
    const static std::string RF;
    const static std::string CF;
    const static std::string LFE;
    const static std::string LS;
    const static std::string RS;
    const static std::string LFC;
    const static std::string RFC;
    const static std::string SD;
    const static std::string SL;
    const static std::string SR;
    const static std::string T;
    const static std::string B;
};


class PresetName
{
public:
    const static std::string FACTORY_DEFAULTS;
    const static std::string INSTALLATION_DEFAULTS;
};


}  // namespace Omm
}  // namespace Av


#endif

