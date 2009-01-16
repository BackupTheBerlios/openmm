#include <QApplication>

#include "controllergui.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    ControllerGui window;
    window.show();
    return app.exec();
}
