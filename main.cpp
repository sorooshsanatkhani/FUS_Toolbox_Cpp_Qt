#include "stdafx.h"
#include "FUSMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FUSMainWindow w;
    w.show();
    return a.exec();
}
