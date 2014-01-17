#include <QApplication>
#include <QDesktopWidget>
//#include "window.h"
#include "controller.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Controller c;


    //Window window;
    //window.resize(window.sizeHint());
    //window.show();
    c.run();
    return app.exec();
}

