#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scanScard();
}

MainWindow::~MainWindow()
{
    mJob.stop();
    delete ui;
}

void MainWindow::newNumber(QString cardName)
{
    cardUID = cardName;

    if(cardUID != NULL)
    {
        ui->label->setText("Card detected: "+cardUID);
    }else
    {
        ui->label->setText("Card not yet detected");
    }
}

void MainWindow::cardStatus(QString status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::scanScard()
{
    connect(&mJob,&multithread::on_number,this,&MainWindow::newNumber);
    connect(&mJob,&multithread::CardStatusName,this,&MainWindow::cardStatus);
    QFuture<void> test = QtConcurrent::run(&this->mJob,&multithread::start,QString("Start Read"));
}
