#include "window.h"
#include "hook.h"
#include "datafile.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // start app
    QApplication a(argc, argv);
    Window *w = new Window(nullptr,&a);
    LoadSettings();
    InstallHook(w);
    int result = a.exec();
    RemoveHook();
    qDebug() << "Closing...";
    SaveSettings();
    w->sendTrayMessage("Hotpad closed");
    return result;
}
