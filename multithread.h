#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QRunnable>
#include <QWaitCondition>
#include <winscard.h>

class multithread : public QObject
{
    Q_OBJECT
public:
    explicit multithread(QObject *parent = 0);
    ~multithread();
    void start();
    void CardListReadersLoop();
    QString cardUID = NULL;
    QString readData = NULL;
    bool mStop;
    bool returnStatus;
    bool returnCardReaderStatus;
private:
    QString         nfccard = NULL;
    SCARDCONTEXT    hSC;
    SCARDHANDLE     hCardHandle;
    SCARDCONTEXT    hContext;
    LPTSTR          pCard = NULL;
    LPTSTR          pReader = NULL;

signals:
    void onNumber(QString cardName);
    void CardStatusName(QString status);
    void cardReaderName(QString name);


public slots:
    void stop();
    void pause();
    void CardReadData(int blockNo);
    void CardWriteData(QString inputData, int blockNo);
    void CardEstablishContext();
    void CardListReaders();
    void CardConnect(QString s);
    void CardListReaderGroups();
    void CardListCards();
    void CardIntroduceCardType();
    void CardReleaseContext();
    void CardStatus();
    void CardTransmit();
    void CardDisconnect();
    void CardFreeMemory();
    void CardAuthenticate(int blockNo);
};

#endif // MULTITHREAD_H
