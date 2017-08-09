#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
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

public slots:
    void newNumber(QString cardName);
    void cardStatus(QString status);
private:
    Ui::MainWindow *ui;
    void scanScard();
    QString cardUID, code;
    multithread mJob;
};

#endif // MAINWINDOW_H
