// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 21 June, 2024

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
