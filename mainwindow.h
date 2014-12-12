#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ActionType.h"
#include <QMainWindow>
#include <QLabel>

namespace Ui {
class MainWindow;
class MyPainterWidget;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QLabel *status_size;
    QLabel *status_size_now;
    QLabel *status_mouse;
    QLabel *status_action;
    QLabel *status_value;

private:
    Ui::MainWindow *ui;
    bool image_status;
    bool preview = false;
    QString img_file;
    bool replaceQuestion(QString);
    bool imageCheck();
    void menuDisable(bool);

protected:
    void resizeEvent(QResizeEvent *event);

public Q_SLOTS:
     void show();
private slots:
    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    void on_actionExit_triggered();
    void on_actionZoom_triggered();
    void on_actionNegative_triggered();
    void on_actionSoften_triggered();
    void on_actionEmboss_triggered();
    void on_actionBinarize_triggered();
    void on_actionSharpen_triggered();
    void on_actionGray_triggered();
    void on_actionReset_triggered();
    void on_actionTo_jpeg_triggered();
    void on_actionTo_PNG_triggered();
    void on_actionTo_BMP_triggered();
    void on_actionInfo_triggered();
    void on_actionAuthor_triggered();
    void on_actionResize_triggered();
    void on_actionQuality_triggered();
    void on_actionTransparent_triggered();
    void on_actionAngle_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionDelete_triggered();
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void on_actionPreview_triggered();
};

#endif // MAINWINDOW_H
