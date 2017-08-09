#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QWaitCondition>
#include <winscard.h>

class multithread : public QObject
{
    Q_OBJECT
public:
    explicit multithread(QObject *parent = 0);
    void start(QString name);
    QString cardUID = NULL;
    bool mStop;
private:
    QString         nfccard = NULL;
    SCARDCONTEXT    hSC;
    SCARDHANDLE     hCardHandle;
    SCARDCONTEXT    hContext;
    LPTSTR          pCard = NULL;
    LPTSTR          pReader = NULL;


signals:

    void on_number(QString cardName);
    void CardStatusName(QString status);

public slots:
    void stop();
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
};

#endif // MULTITHREAD_H
