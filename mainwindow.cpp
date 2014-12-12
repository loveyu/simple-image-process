#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageaction.h"
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QImage>
#include <QMessageBox>
#include <QInputDialog>
#include <QMimeData>

MainWindow::MainWindow(QWidget * parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->widget->setMainWindow(this);
    status_action = new QLabel();
    status_size = new QLabel();
    status_size_now = new QLabel();
    status_mouse = new QLabel();
    status_value = new QLabel();
    setAcceptDrops(true);
    statusBar()->addPermanentWidget(status_action, 1);
    statusBar()->addPermanentWidget(status_value);
    statusBar()->addPermanentWidget(status_size_now);
    statusBar()->addPermanentWidget(status_size);
    statusBar()->addPermanentWidget(status_mouse);
    statusBar()->setStyleSheet(QString("QStatusBar { border-top: 1px solid #aaa;}"));
}

void MainWindow::show() {
    QMainWindow::show();
    status_action->setText(tr("Normal"));
    status_size->setText(tr("No file."));
    menuDisable(true);
}

MainWindow::~MainWindow() {
    delete status_action;
    delete status_size;
    delete status_size_now;
    delete status_mouse;
    delete status_value;
    delete ui;
}
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
void MainWindow::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) { return ; }
    QString path = urls.first().toLocalFile();
    QImage img = QImage(path);
    if(!img.isNull()){
        this->on_actionClose_triggered();
        img_file=path;
        ui->widget->setImage(img);
        if(preview){
            ui->widget_file->setImage(img);
        }
        image_status = true;
        menuDisable(false);
    }
}

void MainWindow::on_actionOpen_triggered() {
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), " ", tr("Image(*.bmp *.png *.jpg *.jpeg)"));
    qDebug() << "Image path:" << file;
    if (!file.isEmpty()) {
        this->on_actionClose_triggered();
        ui->widget->setImage(QImage(file));
        if(preview){
            ui->widget_file->setImage(QImage(file));
        }
        img_file = file;
    } else {
        return;
    }
    image_status = true;
    menuDisable(false);
}

void MainWindow::menuDisable(bool status) {
    ui->menu_convert->setDisabled(status);
    ui->menu_process->setDisabled(status);
    ui->menu_adjustment->setDisabled(status);
    ui->menu_edit->setDisabled(status);
    ui->actionClose->setDisabled(!image_status);
    ui->actionReset->setDisabled(img_file.isEmpty());
}

void MainWindow::on_actionReset_triggered() {
    this->on_actionClose_triggered();
    if (img_file.isEmpty()) {
        return;
    }
    ui->widget->setImage(QImage(img_file));
    if(preview){
        ui->widget_file->setImage(QImage(img_file));
    }
    image_status = true;
    menuDisable(false);
}

void MainWindow::on_actionClose_triggered() {
    image_status = false;
    menuDisable(true);
    ui->widget->setCursor(QCursor(Qt::ArrowCursor));
    ui->widget->clearImage();
    ui->widget->setActionType(Normal);
}

void MainWindow::on_actionExit_triggered() {
    if (QMessageBox::question(NULL, tr("Close Application"), tr("Are you sure close application?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
        this->close();
    }
}

void MainWindow::resizeEvent(QResizeEvent * event) {
    QSize size = event->size();
    if(preview){
        ui->widget_file->setGeometry(QRect(1, 1, size.width()/2-2 - 2, size.height() - ui->menuBar->height() - ui->statusBar->height() - 2));
        ui->widget->setGeometry(QRect(size.width()/2 - 2, 1, size.width()/2 - 2, size.height() - ui->menuBar->height() - ui->statusBar->height() - 2));
    }else{
        ui->widget->setGeometry(QRect(1, 1, size.width() - 1, size.height() - ui->menuBar->height() - ui->statusBar->height() - 2));
    }
}

void MainWindow::on_actionZoom_triggered() {
    if (image_status) {
        status_action->setText(QString("Zoom"));
        ui->widget->setCursor(QCursor(Qt::OpenHandCursor));
        ui->widget->setActionType(Zoom);
    } else {
        QMessageBox msgBox;
        msgBox.setText(tr("Please open the image first!"));
        msgBox.exec();
    }
}

void MainWindow::on_actionNegative_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Negative);
}

void MainWindow::on_actionSoften_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Soften);
}

void MainWindow::on_actionEmboss_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Emboss);
}

void MainWindow::on_actionBinarize_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Binarize);
}

void MainWindow::on_actionSharpen_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Sharpen);
}

void MainWindow::on_actionGray_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Gray);
}

bool MainWindow::imageCheck() {
    if (image_status) {
        return true;
    } else {
        QMessageBox::warning(this, tr("Open error"), tr("No image is open!"));
        return false;
    }
}

void MainWindow::on_actionTo_jpeg_triggered() {
    if (!imageCheck()) {
        return;
    }
    QString file = QFileDialog::getSaveFileName(this, tr("Save File"), " ", tr("Image(*.jpg)"));
    if (!file.isEmpty() && replaceQuestion(file)) {
        ui->widget->image()->save(file, "JPEG");

    }
}

void MainWindow::on_actionTo_PNG_triggered() {
    if (!imageCheck()) {
        return;
    }
    QString file = QFileDialog::getSaveFileName(this, tr("Save File"), " ", tr("Image(*.png)"));
    if (!file.isEmpty() && replaceQuestion(file)) {
        ui->widget->image()->save(file, "PNG");
    }
}

void MainWindow::on_actionTo_BMP_triggered() {
    if (!imageCheck()) {
        return;
    }
    QString file = QFileDialog::getSaveFileName(this, tr("Save File"), " ", tr("Image(*.bmp)"));
    if (!file.isEmpty() && replaceQuestion(file)) {
        ui->widget->image()->save(file, "BMP");
    }
}

bool MainWindow::replaceQuestion(QString str) {
    if (!QFile::exists(str)) {
        return true;
    }
    //replace
    return true;
}

void MainWindow::on_actionInfo_triggered() {
    QMessageBox::information(this, tr("About This Programe"), tr("This is a simple image zoom and process program.\n http://www.loveyu.org/3776.html"), QMessageBox::Ok);
}

void MainWindow::on_actionAuthor_triggered() {
    QMessageBox::about(this, tr("About the author"), tr("loveyu\nhttp://www.loveyu.org/"));
}

void MainWindow::on_actionResize_triggered() {
    if (!imageCheck()) {
        return;
    }
    bool isOK = true;
    while (true) {
        if (!isOK) {
            QMessageBox::warning(this, tr("Input error"), tr("Input error, please try right value!"));
        }
        QString str = QInputDialog::getText(NULL, tr("Change the size"),
                                            QString(tr("Now size: %1x%2\nFormate:width x height (zero will be auto)")).arg(ui->widget->image()->width()).
                                            arg(ui->widget->image()->height()),
                                            QLineEdit::Normal,
                                            (QString("%1x%2").arg(ui->widget->image()->width()).
                                             arg(ui->widget->image()->height())),
                                            &isOK);
        if (isOK) {
            QStringList list = str.split("x");
            if (list.length() != 2) {
                isOK = false;
                continue;
            }
            int width = list.at(0).toInt(&isOK);
            if (!isOK)continue;
            int height = list.at(1).toInt(&isOK);
            if (!isOK)continue;
            if (width < 0)width *= -1;
            if (height < 0)height *= -1;
            if (width == 0 && height == 0) {
                isOK = false;
                continue;
            }
            QImage img = ui->widget->image()->copy();
            isOK = ImageAction::Resize(img, width, height);
            if (isOK) {
                ui->widget->setImage(img);
                ui->widget->update();
            } else {
                QMessageBox::warning(this, tr("Resize error!"), tr("Please try again!"));
            }
            break;
        }
        break;
    }
}

void MainWindow::on_actionQuality_triggered() {
    if (!imageCheck()) {
        return;
    }
    bool isOK = true;
    while (true) {
        if (!isOK) {
            QMessageBox::warning(this, tr("Input error"), tr("Input error, please try right value!"));
        }
        QString str = QInputDialog::getText(NULL, tr("Change the quality"),
                                            QString(tr("The percentage, if use 80.5%, input 80.5!")),
                                            QLineEdit::Normal, "100", &isOK);
        float percentage = str.toFloat(&isOK);
        if (!isOK)continue;
        if (percentage < 0)percentage *= -1;
        if (percentage == 0) {
            isOK = false;
            continue;
        }
        if (percentage > 100) {
            break;
        }
        QImage img = ui->widget->image()->copy();
        isOK = ImageAction::Quality(img, percentage);
        if (isOK) {
            ui->widget->setImage(img);
            ui->widget->update();
        } else {
            QMessageBox::warning(this, tr("Resize error!"), tr("Please try again!"));
        }
        break;
    }

}

void MainWindow::on_actionTransparent_triggered() {
    if (!imageCheck()) {
        return;
    }
    bool isOK = true;
    while (true) {
        if (!isOK) {
            QMessageBox::warning(this, tr("Input error"), tr("Input error, please try right value!"));
        }
        QString str = QInputDialog::getText(NULL, tr("Change the transparent"),
                                            QString(tr("The value in 0-255, please input.")),
                                            QLineEdit::Normal, "255", &isOK);
        int alpha = str.toInt(&isOK);
        if (!isOK)continue;
        if (alpha < 0)alpha *= -1;
        if (alpha == 0 || alpha > 255) {
            isOK = false;
            continue;
        }
        QImage img = ui->widget->image()->copy();
        isOK = ImageAction::Transparent(img, alpha);
        if (isOK) {
            ui->widget->setImage(img);
            ui->widget->update();
        } else {
            QMessageBox::warning(this, tr("Change transparent error!"), tr("Please try again!"));
        }
        break;
    }
}

void MainWindow::on_actionAngle_triggered() {
    if (!imageCheck()) {
        return;
    }
    bool isOK = true;
    while (true) {
        if (!isOK) {
            QMessageBox::warning(this, tr("Input error"), tr("Input error, please try right value!"));
        }
        QString str = QInputDialog::getText(NULL, tr("Rotation the image"),
                                            QString(tr("The value in -360-+360, please input.")),
                                            QLineEdit::Normal, "180", &isOK);
        float angle = str.toFloat(&isOK);
        if (!isOK)continue;
        if (angle > 360 || angle < -360) {
            isOK = false;
            continue;
        }
        QImage img = ui->widget->image()->copy();
        isOK = ImageAction::Rotation(img, angle);
        if (isOK) {
            ui->widget->setImage(img);
            ui->widget->update();
        } else {
            QMessageBox::warning(this, tr("Rotation error!"), tr("Please try again!"));
        }
        break;
    }
}

void MainWindow::on_actionCut_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Cut);
}

void MainWindow::on_actionCopy_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Copy);
}

void MainWindow::on_actionDelete_triggered() {
    if (!imageCheck()) {
        return;
    }
    ui->widget->setActionType(Delete);
}

void MainWindow::on_actionPreview_triggered()
{
    preview =!preview;
    if(preview){
        ui->actionPreview->setText(tr("Close Preview"));
        ui->widget_file->show();
        ui->widget_file->setImage(QImage(img_file));
    }else{
        ui->actionPreview->setText(tr("Preview"));
        ui->widget_file->hide();
    }
    QSize size(this->width(),this->height());
    QResizeEvent sz(size,size);
    QApplication::sendEvent(this,&sz);
}
