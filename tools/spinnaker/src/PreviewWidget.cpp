//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 09.06.2021.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PreviewWidget.h" resolved

#include <QKeyEvent>

#include "PreviewWidget.h"
#include "ui_PreviewWidget.h"


PreviewWidget::PreviewWidget(int framesNo, QWidget *parent) :
        QWidget(parent), ui(new Ui::PreviewWidget), framesNo(framesNo) {
    ui->setupUi(this);

    mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(&preview);

    this->setLayout(mainLayout);

}

PreviewWidget::~PreviewWidget() {
    delete ui;
}

void PreviewWidget::dropPreview(bool refresh) {
    if(refresh) {
        preview.setPixmap(QPixmap(640 * framesNo, 480));
    }
    framesCount = 0;
}

void PreviewWidget::addFrame(const QImage &frame) {
    if(!framesCount) {
        canvas = QImage(640*framesNo, 480, QImage::Format_RGB32);
    }
    if (framesCount < framesNo) {
        QPainter painter;
        painter.begin(&canvas);
        //frame.save("/tmp/test1.png");
        painter.drawImage(640*framesCount, 0, frame.scaled(QSize(640, 480)));
        painter.end();
        ++framesCount;
    }
}

void PreviewWidget::addLabel(const QString &words) {
    QPainter painter;

    painter.begin(&canvas);

    QFont font = painter.font();
    font.setPixelSize(24);
    painter.setFont(font);
    QPen pen = painter.pen();
    pen.setColor({255, 255, 255});
    painter.setPen(pen);

    painter.drawText(30, 30, words);
    painter.end();
}

void PreviewWidget::showPreview() {
    preview.setPixmap(QPixmap::fromImage(canvas));
}

void PreviewWidget::keyPressEvent(QKeyEvent *event) {
    auto c = event->key();
    if(event->key() == Qt::Key_Enter
        || event->key() == Qt::Key_Return)
    {
        emit doCapture();
    }
    else if(event->key() == Qt::Key_Escape)
    {
        emit doTerminate();
    }
    QWidget::keyPressEvent(event);
}

void PreviewWidget::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyReleaseEvent(event);
}
