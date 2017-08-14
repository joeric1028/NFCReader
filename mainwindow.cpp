#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mJob = new multithread(this);
    scanScard();
}

MainWindow::~MainWindow()
{
    mJob->stop();
    delete ui;
}

void MainWindow::newNumber(QString cardUid)
{
    cardUID = cardUid;

    if(cardUID != NULL)
    {
        ui->label->setText("Card Detection: "+cardUID);
    }else
    {
        ui->label->setText("Card Detection: no card within range");
    }
}

void MainWindow::cardStatus(QString status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::scanScard()
{
    connect(mJob,multithread::onNumber,this,MainWindow::newNumber);
    connect(mJob,multithread::CardStatusName,this,MainWindow::cardStatus);
    test = QtConcurrent::run(this->mJob,multithread::start);
}

void MainWindow::on_pushButton_polling_clicked()
{
    if(test.isRunning())
    {
        mJob->pause();
        ui->pushButton_polling->setText("Start Polling");
    }
    else
    {
        scanScard();
        ui->pushButton_polling->setText("Stop  Polling");
    }
}
