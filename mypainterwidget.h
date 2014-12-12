#ifndef MYPAINTERWIDGET_H
#define MYPAINTERWIDGET_H

#include "mainwindow.h"
#include <QWidget>
#include <QPaintEvent>
#include <QImage>

class MyPainterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyPainterWidget(QWidget *parent = 0);
    ~MyPainterWidget();
    const QImage *image() const;
    void setMainWindow(MainWindow *);
    void setActionType(ActionType);

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent  *event);
public slots:
    void setImage(const QImage &img);
    void clearImage();

private:
    QImage m_image;
    MainWindow * main_window;
    ActionType action_type;
    int zoom,o_x=0,o_y=0,r_x,r_y;
    float percentage;
    QRect rect,img_rect;
    bool is_mouse_press=false;
};

#endif // MYPAINTERWIDGET_H
