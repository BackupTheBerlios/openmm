#include <QtGui>
#include <qcoreapplication.h>
#include <phonon/phonon>

int
main(int argc, char** argv) {
//    QCoreApplication app(argc, argv);
//    Phonon::MediaObject* music = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource("/home/jb/music/01-jens_buchert_-_sine_and_worm-gem.mp3"));
//    music->play();
//    app.exec();

    QApplication app(argc, argv);
    QMainWindow window;
    QWidget widget;
    window.setCentralWidget(&widget);
    Phonon::VideoPlayer* player = new Phonon::VideoPlayer(Phonon::VideoCategory, &widget);
//    player->play(Phonon::MediaSource("/home/jb/video/citybahnhof_700kbps.avi"));
//    player->play(Phonon::MediaSource("/home/jb/music/01-jens_buchert_-_sine_and_worm-gem.mp3"));
//    player->play(Phonon::MediaSource("http://192.168.178.1:49100/mediapath/Ut165-USB2FlashStorage-01/FRITZ/mediabox/citybahnhof_700kbps.avi"));
    // phonon with gstreamer backend is very slow with analyzing an mpeg-ts.
    player->play(Phonon::MediaSource("http://192.168.178.26:8888/0$0"));
    window.show();
    app.exec();
}
