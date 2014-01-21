#include <QApplication>
#include <QDesktopWidget>
//#include "window.h"
#include "controller.h"
#include "serialtalker.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Controller c;

   SerialTalker serialTalker;
   serialTalker.init();
   //serialTalker.readSpeed();
    //Window window;
    //window.resize(window.sizeHint());
    //window.show();
    c.run();
    return app.exec();
}

