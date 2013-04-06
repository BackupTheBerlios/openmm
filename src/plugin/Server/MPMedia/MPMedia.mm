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

#include <Poco/ClassLibrary.h>
#include <Poco/File.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/BufferedStreamBuf.h>

#include <Omm/UpnpAvObject.h>
#include <Omm/AvStream.h>
#include <Omm/Util.h>
#include <sstream>

#include "MPMedia.h"

#import <MediaPlayer/MPMediaQuery.h>
#import <MediaPlayer/MPMediaItem.h>
#import <MediaPlayer/MPMediaItemCollection.h>
#import <AVFoundation/AVAsset.h>
#import <AVFoundation/AVAssetReader.h>
#import <AVFoundation/AVAssetReaderOutput.h>


class AVAssetReaderStreamBuf : public Poco::BufferedStreamBuf
{
public:
    AVAssetReaderStreamBuf(AVAssetReaderOutput* pAssetReaderOutput, int bufSize) : Poco::BufferedStreamBuf(bufSize, std::ios_base::in), _pAssetReaderOutput(pAssetReaderOutput), _bufSize(bufSize), _offset(0)
    {
        CMSampleBufferRef pSampleBuf = [_pAssetReaderOutput copyNextSampleBuffer];
        _pBlockBuf = CMSampleBufferGetDataBuffer(pSampleBuf);
    }

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel AVAssetReaderOutput stream read from device, num bytes: " + Poco::NumberFormatter::format(length));
        int res = 0;
        int bytesAvailable = CMBlockBufferGetDataLength(_pBlockBuf);
        int bytesLeft = bytesAvailable - _offset;
        if (length <= bytesLeft) {
            CMBlockBufferCopyDataBytes(_pBlockBuf, _offset, length, buffer);
            res = length;
            _offset += length;
        }
        else {
            CMBlockBufferCopyDataBytes(_pBlockBuf, _offset, bytesLeft, buffer);
            res = bytesLeft;
            CMSampleBufferRef pSampleBuf = [_pAssetReaderOutput copyNextSampleBuffer];
            _pBlockBuf = CMSampleBufferGetDataBuffer(pSampleBuf);
            _offset = 0;
        }

        return res;
    }

private:
    AVAssetReaderOutput*    _pAssetReaderOutput;
    CMBlockBufferRef        _pBlockBuf;
    const int               _bufSize;
    std::size_t             _offset;
};


class AVAssetReaderIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    AVAssetReaderIStream(AVAssetReaderOutput* pAssetReaderOutput, int bufSize = 4096) : _streamBuf(pAssetReaderOutput, bufSize), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    AVAssetReaderStreamBuf   _streamBuf;
};


MPMediaModel::MPMediaModel()
{
}


MPMediaModel::~MPMediaModel()
{
}


std::string
MPMediaModel::getModelClass()
{
    return "MPMediaModel";
}


Omm::ui4
MPMediaModel::getSystemUpdateId(bool checkMod)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    return [[[MPMediaLibrary defaultMediaLibrary] lastModifiedDate] timeIntervalSinceReferenceDate];

    [pool release];
}


Omm::Av::CsvList
MPMediaModel::getQueryProperties()
{
    return Omm::Av::CsvList(Omm::Av::AvProperty::ARTIST, Omm::Av::AvProperty::ALBUM, Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER);
}


void
MPMediaModel::scan()
{
    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel scan ...");
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    MPMediaQuery* query = [[MPMediaQuery alloc] init];
    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel allocated query.");
//    [query setGroupingType: MPMediaGroupingArtist];
//    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel grouped query.");
    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel number of collections: " + Poco::NumberFormatter::format([[query collections] count]));
    for (MPMediaItemCollection* itemCollection in [query collections]) {
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel number of items in collection: " + Poco::NumberFormatter::format([itemCollection count]));
        for (MPMediaItem* item in [itemCollection items]) {
            std::string itemId(Poco::NumberFormatter::format([[item valueForProperty:MPMediaItemPropertyPersistentID] unsignedLongLongValue]));
            LOGNS(Omm::Av, upnpav, debug, "MPMediaModel add path: " + itemId);
            addPath(itemId);
//            std::string title([[item valueForProperty:MPMediaItemPropertyTitle] UTF8String]);
//            LOGNS(Omm::Av, upnpav, debug, "MPMediaModel title: " + title);
        }
//        [itemCollection enumerateValuesForProperties:[NSSet setWithObjects:MPMediaItemPropertyTitle,nil]
//                                 usingBlock:^(NSString* property, id value, BOOL* stop) {
//                                     if ([property isEqualToString:MPMediaItemPropertyTitle]) {
//                                         if (value) {
//                                             LOGNS(Omm::Av, upnpav, debug, "MPMediaModel title: " + [value UTF8String]);
//                                         }
//                                     }
//                                 }];
    }

    [pool release];
    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel scan finished.");
}


Omm::Av::ServerObject*
MPMediaModel::getMediaObject(const std::string& path)
{
    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel get media object with path: " + path);
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    NSNumber* itemId = [NSNumber numberWithUnsignedLongLong:Poco::NumberParser::parseUnsigned64(path)];
    MPMediaPropertyPredicate* predicate = [MPMediaPropertyPredicate predicateWithValue:itemId forProperty:MPMediaItemPropertyPersistentID];
    MPMediaQuery* query = [[MPMediaQuery alloc] initWithFilterPredicates:[NSSet setWithObject:predicate]];
    // result of query should be only one item, as persistent ID is unique.
    MPMediaItem* item = [[[[query collections] lastObject] items] lastObject];

    Omm::Av::ServerItem* pItem = getServerContainer()->createMediaItem();

    // FIXME: set correct class
    pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_ITEM));

    pItem->setTitle([[item valueForProperty:MPMediaItemPropertyTitle] UTF8String]);
    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object set title: " + std::string([[item valueForProperty:MPMediaItemPropertyTitle] UTF8String]));
    const char* pArtist = [[item valueForProperty:MPMediaItemPropertyArtist] UTF8String];
    if (pArtist) {
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object set artist: " + std::string(pArtist));
        pItem->setUniqueProperty(Omm::Av::AvProperty::ARTIST, pArtist);
    }
    const char* pAlbum = [[item valueForProperty:MPMediaItemPropertyAlbumTitle] UTF8String];
    if (pAlbum) {
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object set album: " + std::string(pAlbum));
        pItem->setUniqueProperty(Omm::Av::AvProperty::ALBUM, pAlbum);
    }
    NSNumber* pTrack = [item valueForProperty:MPMediaItemPropertyAlbumTrackNumber];
    if (pTrack) {
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object set track number: " + Poco::NumberFormatter::format([pTrack unsignedIntegerValue]));
        pItem->setUniqueProperty(Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER, Poco::NumberFormatter::format([pTrack unsignedIntegerValue]));
    }
    const char* pGenre = [[item valueForProperty:MPMediaItemPropertyGenre] UTF8String];
    if (pGenre) {
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object set genre: " + std::string(pGenre));
        pItem->setUniqueProperty(Omm::Av::AvProperty::GENRE, pGenre);
    }

    //  NSNumber NSTimeInterval

    Omm::Av::ServerObjectResource* pResource = pItem->createResource();
//    pResource->setSize(getSize(path));
    NSNumber* pDuration = [item valueForProperty:MPMediaItemPropertyPlaybackDuration];
    if (pDuration) {
        // FIXME: duration is wrong
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object set track length: " + Poco::NumberFormatter::format([pTrack doubleValue]));
        pResource->setAttribute(Omm::Av::AvProperty::DURATION, Omm::Av::AvTypeConverter::writeDuration([pTrack doubleValue]));
    }
    // FIXME: set correct mime type
    pResource->setProtInfo("http-get:*:" + Omm::Av::Mime::AUDIO_MPEG + ":*");
    pItem->addResource(pResource);
//        // TODO: add icon property
////        pItem->_icon = pItem->_path;

    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel get media object finished.");

    [pool release];
    return pItem;
}


std::streamsize
MPMediaModel::getSize(const std::string& path)
{
    return 0;
}


bool
MPMediaModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
//    return true;
    return false;
}


std::istream*
MPMediaModel::getStream(const std::string& path, const std::string& resourcePath)
{
    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel get stream of media object with path: " + path);

//    return new AVAssetReaderIStream(path);

//    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    std::istream* pRes = 0;

    NSNumber* itemId = [NSNumber numberWithUnsignedLongLong:Poco::NumberParser::parseUnsigned64(path)];
    MPMediaPropertyPredicate* predicate = [MPMediaPropertyPredicate predicateWithValue:itemId forProperty:MPMediaItemPropertyPersistentID];
    MPMediaQuery* query = [[MPMediaQuery alloc] initWithFilterPredicates:[NSSet setWithObject:predicate]];
    // result of query should be only one item, as persistent ID is unique.
    MPMediaItem* item = [[[[query collections] lastObject] items] lastObject];
    if (item) {
        LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object found item with persistent id: " + Poco::NumberFormatter::format(Poco::NumberParser::parseUnsigned64(path)));
        NSURL* pUrl = [item valueForProperty:MPMediaItemPropertyAssetURL];
        if (!pUrl) {
            LOGNS(Omm::Av, upnpav, error, "MPMediaModel media object has no asset url, path: " + path);
        }
        else {
            LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object asset url: " + std::string([[pUrl absoluteString] UTF8String]));
            AVAsset* pAsset;
            id savedException = nil;
            @try {
                pAsset = [AVAsset assetWithURL:pUrl];
                LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object alloc asset, path: " + path);
                LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object asset number of tracks: " + Poco::NumberFormatter::format([[pAsset tracks] count]));

                NSError* pError;
                _pAssetReader = [AVAssetReader assetReaderWithAsset:pAsset error:&pError];
                LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object allocated asset reader");
                AVAssetReaderTrackOutput* pAssetReaderOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[pAsset.tracks lastObject] outputSettings:nil];
                [_pAssetReader addOutput:pAssetReaderOutput];
                if (![_pAssetReader startReading]) {
                    LOGNS(Omm::Av, upnpav, error, "MPMediaModel media object asset failed to start reading track output");
                }
                else {
                    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel media object allocated asset reader track output");
                    pRes = new AVAssetReaderIStream(pAssetReaderOutput);
                }
            }
            @catch (NSException* exception) {
                LOGNS(Omm::Av, upnpav, error, "MPMediaModel media object failed to alloc asset: " + std::string([[exception reason] UTF8String]));
                savedException = [exception retain];
                @throw;
            }
            @finally {
//                [pool release];
                [savedException autorelease];
            }
        }
    }

//    [pool release];
    return pRes;
}


void
MPMediaModel::freeStream(std::istream* pIstream)
{
    LOGNS(Omm::Av, upnpav, debug, "deleting file stream");

    [_pAssetReader cancelReading];
    // TODO: also release memory pool of stream (where AVAssetReaderOutput is allocated)
    delete pIstream;
}


//MPMediaItem*
//MPMediaModel::getMediaItem(const std::string& path)
//{
//    LOGNS(Omm::Av, upnpav, debug, "MPMediaModel get media object with path: " + path);
//    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
//
//    NSNumber* itemId = [NSNumber numberWithUnsignedLongLong:Poco::NumberParser::parseUnsigned64(path)];
//    MPMediaPropertyPredicate* predicate = [MPMediaPropertyPredicate predicateWithValue:itemId forProperty:MPMediaItemPropertyPersistentID];
//    MPMediaQuery* query = [[MPMediaQuery alloc] initWithFilterPredicates:[NSSet setWithObject:predicate]];
//    // result of query should be only one item, as persistent ID is unique.
//    return item = [[[[query collections] lastObject] items] lastObject];
//}


void
MPMediaModel::setClass(Omm::Av::ServerItem* pItem, Omm::AvStream::Meta::ContainerFormat format)
{
    switch (format) {
        case Omm::AvStream::Meta::CF_UNKNOWN:
            pItem->setClass(Omm::Av::AvClass::OBJECT);
            break;
        case Omm::AvStream::Meta::CF_AUDIO:
            pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_ITEM));
            break;
        case Omm::AvStream::Meta::CF_VIDEO:
            pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_ITEM));
            break;
        case Omm::AvStream::Meta::CF_IMAGE:
            pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::IMAGE_ITEM));
            break;
        case Omm::AvStream::Meta::CF_PLAYLIST:
            pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::PLAYLIST_ITEM));
            break;
    }
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(MPMediaModel)
POCO_END_MANIFEST
#endif
