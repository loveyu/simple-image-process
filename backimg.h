#ifndef BACKIMG_H
#define BACKIMG_H

#include <QWidget>
#include <QImage>
#include "imageaction.h"
#include <QPainter>

class BackImg : public QWidget
{
    Q_OBJECT
public:
    explicit BackImg(QWidget *parent = 0);
    ~BackImg();

protected:
    void paintEvent(QPaintEvent *);
signals:
public slots:
    void setImage(const QImage &img);

private:
    QImage m_image;
};

#endif // BACKIMG_H
