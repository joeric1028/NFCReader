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
    mJob->CardReleaseContext();
    mJob->CardFreeMemory();
    delete ui;
}

void MainWindow::newNumber(QString cardUid)
{
    cardUID = cardUid;
    if(ui->pushButton_polling->text() != "Start Polling")
    {
        if(mJob->returnCardReaderStatus == true)
        {
            if(cardUID != NULL)ui->label->setText("Card Detection: Card Found "+cardUID);
            else ui->label->setText("Card Detection: No Card Within Range");
        }else{
            ui->label->setText("Card Detection: No Card Reader Found!");

        }
    }
    else ui->label->setText("Card Detection: Stop Detecting");
}

void MainWindow::cardStatus(QString status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::enablebutton(int i)
{
    if(i <= 3)ui->pushButton_writeData->setDisabled(true);
    else ui->pushButton_writeData->setEnabled(true);
}

void MainWindow::scanScard()
{
    connect(mJob,SIGNAL(onNumber(QString)),this,SLOT(newNumber(QString)));
    connect(mJob,SIGNAL(CardStatusName(QString)),this,SLOT(cardStatus(QString)));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(enablebutton(int)));
    test = QtConcurrent::run(mJob,multithread::start);

    ui->pushButton_readData->hide();
    ui->pushButton_writeData->hide();
    ui->lineEdit_valueData->hide();
    ui->spinBox->hide();
    if(ui->spinBox->text().toFloat() <= 3)ui->pushButton_writeData->setDisabled(true);
    else ui->pushButton_writeData->setEnabled(true);
}

void MainWindow::on_pushButton_polling_clicked()
{
    if(test.isRunning())
    {
        mJob->pause();
        ui->pushButton_polling->setText("Start Polling");
        ui->pushButton_readData->show();
        ui->pushButton_writeData->show();
        ui->lineEdit_valueData->show();
        ui->spinBox->show();
    }
    else
    {
        scanScard();
        ui->pushButton_polling->setText("Stop  Polling");
    }
}

void MainWindow::on_pushButton_readData_clicked()
{
    QString data;

    if(ui->pushButton_polling->text() == "Start Polling")
    {

        mJob->CardReadData(ui->spinBox->value());
        data = mJob->readData;
        ui->lineEdit_valueData->setText(data);
        if(mJob->returnStatus == true)
        {
            ui->label->setText("Read Data Success : " + mJob->readData);
        }else{
            ui->label->setText("Read Data Failed");
        }
    }
}

void MainWindow::on_pushButton_writeData_clicked()
{
    if(ui->pushButton_polling->text() == "Start Polling")
    {

        mJob->CardWriteData(ui->lineEdit_valueData->text(), ui->spinBox->value());
        ui->lineEdit_valueData->clear();
        mJob->CardReadData(ui->spinBox->value());
        if(mJob->returnStatus == true)
        {
            ui->label->setText("Write Data Success : " + mJob->readData);
        }else{
            ui->label->setText("Write Data Failed");
        }
    }
}
