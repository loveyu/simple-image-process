#include "mypainterwidget.h"
#include "imageaction.h"
#include <QPainter>
#include <QImage>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>

MyPainterWidget::MyPainterWidget(QWidget *parent) :
    QWidget(parent)
{
    action_type = Normal;
    setMouseTracking(true);
    zoom = 100;
    setFocusPolicy(Qt::StrongFocus);
}
MyPainterWidget::~MyPainterWidget(){
}

void MyPainterWidget::paintEvent(QPaintEvent *event)
{
    QPen pen;
    QVector<qreal> dashes;
    qreal space = 3;

    QWidget::paintEvent(event);
    if (m_image.isNull())
        return;
    ImageAction ia;
    QImage img = ia.GetImageZoom(m_image,action_type==Zoom?zoom:0,this->width(),this->height(),o_x,o_y);
    main_window->status_value->setText(QString("%1%").arg(ia.nowZoom));
    QPainter painter(this);
    img_rect = ia.rect;
    main_window->status_size_now->setText(QString("Now:%1x%2").arg(ia.rect.width()).arg(ia.rect.height()));
    painter.drawImage(ia.rect,img);
    percentage = (float)m_image.width()/img_rect.width();
    switch(action_type){
    case Cut:
    case Copy:
    case Delete:
        if(rect.isEmpty())break;
        pen.setBrush(QBrush(qRgb(100,100,100)));
        dashes << 5 << space << 5 <<space;
        pen.setDashPattern(dashes);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawRect(rect);
        break;
    default:

        break;
    }
    main_window->status_size->setText(QString("%1x%2").arg(m_image.width()).arg(m_image.height()));
}
void MyPainterWidget::keyPressEvent(QKeyEvent *event){
    if(action_type==Zoom){
        int bz = zoom;
        int bz_x = o_x;
        int bz_y = o_y;
        int key = event->key();
        if(key==Qt::Key_Minus)
        {
            if(zoom>5){
                zoom-=5;
            }
            if(zoom<5)zoom = 5;
        }else if(key==Qt::Key_Equal){
            if(zoom<500){
                zoom+=5;
            }
            if(zoom>500)zoom = 500;
        }else if(key==Qt::Key_Left){
            o_x -=10;
        }else if(key==Qt::Key_Right){
            o_x+=10;
        }else if(key==Qt::Key_Up){
            o_y-=10;
        }else if(key==Qt::Key_Down){
            o_y+=10;
        }
        if(bz!=zoom){
            r_x =  r_x*zoom/bz;
            r_y = r_y*zoom/bz;
            this->update();
        }else if(bz_x!=o_x || bz_y!=o_y){
            if(o_x<0)o_x = 0;
            if(o_y<0)o_y = 0;
            this->update();
        }
    }
}
const QImage* MyPainterWidget::image() const
{
    return &m_image;
}

void MyPainterWidget::wheelEvent(QWheelEvent *event)
{
    int numSteps = event->delta() / 8 / 15;
    event->accept();
    if(action_type==Zoom){
        int bz = zoom;
        if(numSteps>0){
            //--
            if(zoom>5){
                zoom-=5;
            }
            if(zoom<5)zoom = 5;
        }else{
            //++
            if(zoom<500){
                zoom+=5;
            }
            if(zoom>500)zoom = 500;
        }
        r_x =  r_x*zoom/bz;
        r_y = r_y*zoom/bz;
        this->update();
    }
}

void MyPainterWidget::setImage(const QImage &img)
{
    m_image = img;
    main_window->status_size_now->setText(QString(""));
    main_window->status_size->setText(QString("Size:%1x%2").arg(m_image.width()).arg(m_image.height()));
    this->update();
}

void MyPainterWidget::clearImage(){
    m_image= QImage();
    main_window->status_size->setText(tr("No file."));
    this->update();
}
void MyPainterWidget::setMainWindow(MainWindow *win){
    main_window = win;
}
void MyPainterWidget::mousePressEvent(QMouseEvent *e)
{
    int l,t,w,h;
    r_x = e->x();
    r_y = e->y();
    is_mouse_press = true;
    switch(action_type){
    case Zoom:
        this->setCursor(QCursor(Qt::ClosedHandCursor));
        break;
    case Cut:
    case Copy:
    case Delete:
        rect = QRect();
        l = img_rect.left(),t = img_rect.top(),w=img_rect.width(),h = img_rect.height();
        if(r_x<l){
            r_x = l;
        }else if(r_x>l+w){
            r_x = l+w;
        }
        if(r_y<t){
            r_y = t;
        }else if(r_y>t+h){
            r_y = t+h;
        }
        break;
    default:
        break;
    }
}
void MyPainterWidget::mouseReleaseEvent(QMouseEvent *e){
    int x = e->x();
    int y = e->y();
    this->setCursor(QCursor(Qt::ArrowCursor));
    is_mouse_press = false;
    switch(action_type){
    case Zoom:
        this->setCursor(QCursor(Qt::OpenHandCursor));
        o_x +=r_x-x;
        o_y +=r_y-y;
        if(o_x<0)o_x = 0;
        if(o_y<0)o_y = 0;
        this->update();
        break;
    case Cut:
        if(rect.width()<1 || rect.height()<1)break;
        ImageAction::Cut(m_image,QRect(rect.left()-img_rect.left(),rect.top()-img_rect.top(),rect.width(),rect.height()),percentage);
        this->update();
        break;
    case Copy:
        if(rect.width()<1 || rect.height()<1)break;
        if(ImageAction::Copy(m_image,QRect(rect.left()-img_rect.left(),rect.top()-img_rect.top(),rect.width(),rect.height()),percentage)){
            QMessageBox::information(main_window,tr("Copy success."),tr("Copy success."));
        }else{
            QMessageBox::warning(main_window,tr("Copy error!"),tr("Copy error!"));
        }
        break;
    case Delete:
        if(rect.width()<1 || rect.height()<1)break;
        ImageAction::Delete(m_image,QRect(rect.left()-img_rect.left(),rect.top()-img_rect.top(),rect.width(),rect.height()),percentage);
        this->update();
        break;
    default:
        break;
    }
    rect = QRect();
}
void MyPainterWidget::setActionType(ActionType at){
    action_type = at;
    rect = QRect();
    main_window->status_action->setText(tr("Loading........"));
    switch(at){
    case Normal:
        zoom = 100;
        o_x = o_y = 0;
        main_window->status_action->setText(tr("Normal"));
        break;
    case Zoom:
        main_window->status_action->setText(tr("Zoom"));
        break;
    case Negative:
        ImageAction::Negative(m_image);//负片处理
        main_window->status_action->setText(tr("Negative"));
        break;
    case Soften:
        ImageAction::Soften(m_image);//柔化
        main_window->status_action->setText(tr("Soften"));
        break;
    case Gray:
        ImageAction::Gray(m_image);//灰度
        main_window->status_action->setText(tr("Gray"));
        break;
    case Binarize:
        ImageAction::Binarize(m_image);//黑白
        main_window->status_action->setText(tr("Binarize"));
        break;
    case Emboss:
        ImageAction::Emboss(m_image);//浮雕
        main_window->status_action->setText(tr("Emboss"));
        break;
    case Sharpen:
        ImageAction::Sharpen(m_image);//锐化
        main_window->status_action->setText(tr("Sharpen"));
        break;
    case Cut:
        main_window->status_action->setText(tr("Cut"));
        break;
    case Copy:
        main_window->status_action->setText(tr("Copy"));
        break;
    case Delete:
        main_window->status_action->setText(tr("Delete"));
        break;
    default:
        break;
    }
    this->update();
}
void MyPainterWidget::mouseMoveEvent(QMouseEvent *e){
    int x = e->x();
    int y = e->y();
    main_window->status_mouse->setText(QString("%1:%2").arg(x).arg(y));
    switch(action_type){
    case Cut:
    case Copy:
    case Delete:
        if(is_mouse_press){
            int l = img_rect.left(),t = img_rect.top(),w=img_rect.width(),h = img_rect.height();
            if(x<l){
                x = l;
            }else if(x>l+w){
                x = l+w;
            }
            if(y<t){
                y = t;
            }else if(y>t+h){
                y = t+h;
            }
            l = r_x;
            t = r_y;
            w = x-r_x;h = y-r_y;
            if(w<0){
                l = x;
                w *= -1;
            }
            if(h<0){
                t = y;
                h *=-1;
            }
            rect = QRect(l,t,w,h);
            this->update();
        }
        break;
    default:
        break;
    }
}
