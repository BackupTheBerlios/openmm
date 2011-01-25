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

#include <libavformat/avformat.h>
#include <stdio.h>

int
main(int argc, char** argv)
{
    AVFormatContext extFormatContext;
    AVFormatContext* pFormatContext = avformat_alloc_context();
    printf("relative address of external AVFormatContext.timestamp: %li\n", (void*)&(extFormatContext.debug) - (void*)&extFormatContext);
    printf("size of external AVFormatContext: %lu\n", sizeof(extFormatContext));
    printf("relative address of internal AVFormatContext.timestamp: %li\n", (void*)&(pFormatContext->debug) - (void*)pFormatContext);
    printf("size of internal AVFormatContext: %lu\n", sizeof(*pFormatContext));
    
    printf("size of AVPacket: %lu\n", sizeof(AVPacket));
    printf("size of AVStream: %lu\n", sizeof(AVStream));
    printf("size of AVFormatParameters: %lu\n", sizeof(AVFormatParameters));
    
    long externalRelativeTimestampAddress = (void*)&(extFormatContext.timestamp) - (void*)&extFormatContext;
    long internalRelativeTimestampAddress = (void*)&(pFormatContext->timestamp) - (void*)pFormatContext;
    
    av_free(pFormatContext);
    return (externalRelativeTimestampAddress == internalRelativeTimestampAddress);
}