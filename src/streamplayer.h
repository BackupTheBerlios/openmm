/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
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
#ifndef STREAMPLAYER_H
#define STREAMPLAYER_H

#include "page.h"
#include "mediaplayer.h"
#include "title.h"

#include <qstring.h>

class MediaPlayer;

// TODO: rework implementation of singleton with inheritance.
//       QWidget as singleton can have two different parent widgets ...?
//       Where to store: StreamPlayer* streamPlayerInstance;

/**
@author Jörg Bakker
*/
class StreamPlayer : public QObject, public Page
{
    Q_OBJECT

public:
    static StreamPlayer *instance();
    static void setKeyHandler(MediaPlayer *player);
    static bool isPlaying() { return m_isPlaying; };

public slots:
    void play(Title *title);
    void stop();

    virtual void showOsd(QString text, uint duration);
    virtual void hideOsd();

protected:
    StreamPlayer();
    ~StreamPlayer();

    void keyPressEvent(QKeyEvent *k);
    virtual void exitPage();

    virtual void initStream();
    virtual void closeStream();
    virtual void playStream(Mrl *mrl);
    virtual void stopStream();

private:
    static StreamPlayer *m_instance;
    static bool m_isPlaying;
    static MediaPlayer *m_keyHandler;
};

#endif
