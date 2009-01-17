#include <QApplication>

#include "upnpcontroller.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    UpnpController upnpController;
    upnpController.showMainWindow();
    return app.exec();
}
