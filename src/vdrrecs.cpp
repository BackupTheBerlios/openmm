/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg@hakker.de   							   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "vdrrecs.h"

VdrRecs::VdrRecs(QString videoPath, QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_videoPath = videoPath;
    m_videoDir = QDir(m_videoPath);
    m_videoDir.setFilter(QDir::Dirs);
}


VdrRecs::~VdrRecs()
{
}


void
VdrRecs::getRecs(Tv *tv)
{
    const QFileInfoList *list = m_videoDir.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *cur;
    QDir d = m_videoDir;

//    while ((cur = it.current()) != 0 && cur->fileName() != "." && cur->fileName() != "..") {
    while ((cur = it.current()) != 0) {
        ++it;
        if (cur->fileName() == "." || cur->fileName() == "..") {
            continue;
        }
        qDebug("VdrRecs::getRecs() changing dir to %s", cur->fileName().latin1());
        d.cd(cur->fileName());
        // do something in the directory of this recording
        QStringList s = d.entryList(QDir::Dirs);
        if (s.count() > 3) {
            // should be only one subdir in the recordings directory
            qDebug("VdrRecs::getRecs() too much directories in %s", cur->filePath().latin1());
        }
        if (s[0] == "_") {
            // serial with several recordings
            d.cd("_");
            getRecInfo(tv, d);
            d.cdUp();
        }
        else {
            getRecInfo(tv, d);
        }
        d.cdUp();
    }
}


void
VdrRecs::getRecInfo(Tv *tv, QDir d)
{
    // go through all directories, each of them contains the actual recordings 001.vdr, 002.vdr, ...
    QDir drecs = d;
    QStringList recList = d.entryList("*.rec");
    for ( QStringList::Iterator it = recList.begin(); it != recList.end(); ++it ) {
        qDebug("VdrRecs::getRecInfo() getting infos for recording: %s", (*it).latin1());
        // get first info from directory name: date, time of recording
        QString day = (*it).section('.', 0, 0);
        QString hour = (*it).section('.', 1, 1);
        QString min = (*it).section('.', 2, 2);
        if (!drecs.cd((*it)))
            qDebug("VdrRecs::getRecInfo() failed to change directory to: %s", (*it).latin1());
        else
            qDebug("VdrRecs::getRecInfo() changing to directory: %s", (*it).latin1());
        // TODO: get title of recording from info.vdr (last parameter of TvRec ctor)
        //       alternatively: map Svdrp infos to disc infos and get title from Svdrp
        TvRec *recEntry = new TvRec("", day, hour + ":" + min, "");
        tv->appendRec(recEntry);

        QString digits = QString("[0123456789]");
        QString vdrRecFilter = digits + digits + digits + ".vdr";
        drecs.setFilter(QDir::Files);

        const QFileInfoList *list = drecs.entryInfoList(vdrRecFilter);
        QFileInfoListIterator itt(*list);
        QFileInfo *cur;
        QStringList recFileList;
        while ((cur = itt.current()) != 0) {
            ++itt;
            qDebug("VdrRecs::getRecInfo() file: %s", cur->absFilePath().latin1());
            recFileList.append(cur->absFilePath());
        }
        recEntry->setRecFiles(recFileList);
        drecs.cdUp();
    }
}
