//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 09.06.2021.
//
#include "PreviewWidget.h"

#include <QApplication>

int main(int argc, char **argv) {

    QApplication a(argc, argv);
    PreviewWidget *p = new PreviewWidget(2);

    p->show();
    p->dropPreview();
    p->addFrame(QImage("/home/victor/cap0/калибр1.tif"));
    p->addFrame(QImage("/home/victor/cap0/калибр10.tif"));
    p->addLabel("Here we are");
    p->showPreview();


    return QApplication::exec();
}
