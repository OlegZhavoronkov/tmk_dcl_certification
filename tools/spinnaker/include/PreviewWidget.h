//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 09.06.2021.
//

#ifndef TMK_CAPTURE_OBSOLETE_PREVIEWWIDGET_H
#define TMK_CAPTURE_OBSOLETE_PREVIEWWIDGET_H

#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class PreviewWidget; }
QT_END_NAMESPACE

class PreviewWidget : public QWidget {
    Q_OBJECT

public:
    explicit PreviewWidget(int framesNo, QWidget *parent = nullptr);

    ~PreviewWidget() override;

    void dropPreview(bool refresh = true);
    void addFrame(const QImage &frame);
    void addLabel(const QString &words);
    void showPreview();

signals:
    void doCapture();
    void doTerminate();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    Ui::PreviewWidget *ui;

    QVBoxLayout *mainLayout;

    QImage canvas;

    QLabel preview;

    int framesNo;
    int framesCount;


};


#endif //TMK_CAPTURE_OBSOLETE_PREVIEWWIDGET_H
