#include "backimg.h"

BackImg::BackImg(QWidget *parent) : QWidget(parent)
{

}

BackImg::~BackImg()
{

}
void BackImg::setImage(const QImage &img)
{
    m_image = img;
    this->update();
}
void BackImg::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if (m_image.isNull())
        return;
    ImageAction ia;
    QImage img = ia.GetImageZoom(m_image,0,this->width(),this->height(),0,0);
    QPainter painter(this);
    painter.drawImage(ia.rect,img);
}
