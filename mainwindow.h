#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include "multithread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void onStop();

public slots:
    void newNumber(QString cardUid);
    void cardStatus(QString status);
private slots:
    void on_pushButton_polling_clicked();

private:
    Ui::MainWindow *ui;
    void scanScard();
    QString cardUID, code;
    multithread *mJob;
    QFuture<void> test;
};

#endif // MAINWINDOW_H
