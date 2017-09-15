#include "multithread.h"
#include <winscard.h>

multithread::multithread(QObject *parent) : QObject(parent)
{
    mStop = false;
}
multithread::~multithread()
{
    mStop = true;
    CardReleaseContext();
    CardFreeMemory();
}

void multithread::start()
{
    cardUID = "";
    mStop = false;
    returnCardReaderStatus = false;
    CardEstablishContext();
    for(;;)
    {
        if(mStop != true)
        {
            CardListReaders();
            if(returnCardReaderStatus == false)
            {
                CardEstablishContext();
                emit onNumber("");
            }else{
                if(cardUID==NULL)
                {
                    CardListReaders();
                    CardConnect(nfccard);
                    CardTransmit();
                    CardStatus();
                    emit onNumber(cardUID);
                }
                else
                {
                    qDebug()<<"Card Found "<<cardUID;
                    emit onNumber(cardUID);
                    CardStatus();
                }
            }
        }
        else
        {
            return;
        }
    }
}

void multithread::CardListReadersLoop()
{
    LPTSTR          pmszReaders = NULL;
    LONG            lReturn, lReturn2;
    DWORD           cch = SCARD_AUTOALLOCATE;

    mStop = false;
    cardUID = "";

    CardEstablishContext();
    // Retrieve the list the readers.
    // hSC was set by a previous call to SCardEstablishContext.
    lReturn = SCardListReaders(hSC,
                               NULL,
                               (LPTSTR)&pmszReaders,
                               &cch );
    switch( lReturn )
    {
        case SCARD_E_NO_READERS_AVAILABLE:
           // qDebug("Reader is not in groups.\n");
        qDebug()<<"Reader is not in groups.\n";
            // Take appropriate action.
            // ...
            break;

        case SCARD_S_SUCCESS:
            // Do something with the multi string of readers.
            // Output the values.
            // A double-null terminates the list of values.
            pReader = pmszReaders;
            while ( '\0' != *pReader )
            {
                // Display the value.
                //qDebug("Reader: %S\n", pReader );

                QString convert = QString::fromWCharArray(pReader);

                //qDebug()<<convert;
               qDebug()<<"Reader: %S"<<convert;
               emit cardReaderName(convert);
               nfccard = convert;

                // Advance to the next value.
               pReader = pReader + wcslen((wchar_t *)pReader) + 1;
//                QString convert = QString::fromchararray(pReader);
//                qDebug()<<convert;

            }
            // Free the memory.
            lReturn2 = SCardFreeMemory( hSC,
                                       pmszReaders );
            if ( SCARD_S_SUCCESS != lReturn2 )
               // qDebug("Failed SCardFreeMemory\n");
                qDebug()<<"Failed SCardFreeMemory\n";
            break;

    default:
            //qDebug("Failed SCardListReaders\n");
        qDebug()<<"Failed SCardListReaders\n";
            // Take appropriate action.
            // ...
            break;
    }
}

void multithread::stop()
{
    cardUID = "";
    mStop = true;
}

void multithread::pause()
{
    mStop = true;
}

void multithread::CardReadData(int blockNo)
{
    CardConnect(nfccard);
    CardAuthenticate(blockNo);
    BYTE pbRecv[258];
    BYTE  pbSend[] = {0xFF,0xB0,0x00,0x00,0x10};
    pbSend[3] = blockNo;
    DWORD dwSend, dwRecv;
    dwSend = sizeof(pbSend);
    dwRecv = sizeof(pbRecv);
    LONG lReturn;
    lReturn = SCardTransmit(hCardHandle,
                             SCARD_PCI_T1,
                             pbSend,
                             dwSend,
                             NULL,
                             pbRecv,
                             &dwRecv );
    if ( SCARD_S_SUCCESS != lReturn )
    {
        qDebug()<<"Failed SCardTransmit\n " << pbSend[3];
        returnStatus = false;
    }
    else if(SCARD_S_SUCCESS == lReturn)
    {
        QByteArray qdb = (const char*)(pbRecv);
        qDebug()<<"Success"<<qdb << " " << pbSend[3];
        readData = qdb.data();
        returnStatus = true;
     }
}

void multithread::CardWriteData(QString inputData, int blockNo)
{
    CardConnect(nfccard);
    CardAuthenticate(blockNo);
    BYTE pbRecv[258];
    BYTE  pbSend[] = {0xFF,0xD6,0x00,0x04,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    pbSend[3] = blockNo;;
    unsigned int j = 0;
    for(int i = 1; i <= 16; i++)
    {

        if(i > inputData.trimmed().size() || inputData.trimmed().isEmpty())
        {
            pbSend[i+4] = 0x00;
        }else{
            pbSend[i+4] = inputData.trimmed().at(j).toLatin1();
        }
        j++;
    }
    DWORD dwSend,dwRecv;
    dwSend = sizeof(pbSend);
    dwRecv = sizeof(pbRecv);
    LONG lReturn;
    lReturn = SCardTransmit(hCardHandle,
                             SCARD_PCI_T1,
                             pbSend,
                             dwSend,
                             NULL,
                             pbRecv,
                             &dwRecv );
    if ( SCARD_S_SUCCESS != lReturn )
    {
        qDebug()<<"Failed SCardTransmit\n " <<pbSend[5];
        returnStatus = false;
    }
    else if(SCARD_S_SUCCESS == lReturn)
    {
        returnStatus = true;
        QByteArray qdb = (const char*)(pbRecv);
        readData = qdb;
        qDebug()<<"Success " <<readData <<" "<<pbSend[5];
     }
}

void multithread::CardEstablishContext()
{
        LONG            lReturn;
        // Establish the context.
        lReturn = SCardEstablishContext(SCARD_SCOPE_USER,
                                        NULL,
                                        NULL,
                                        &hSC);
        if ( SCARD_S_SUCCESS == lReturn )
           // qDebug("Failed SCardEstablishContext\n");
            qDebug()<<"SUCCESS SCardEstablishContext\n";
        else
        {
            // Use the context as needed. When done,
            // free the context by calling SCardReleaseContext.
            // ...
            qDebug()<<"FAILED SCardEstablishContext\n";
        }
}

void multithread::CardListReaders()
{
    LPTSTR          pmszReaders = NULL;
    LONG            lReturn, lReturn2;
    DWORD           cch = SCARD_AUTOALLOCATE;

    // Retrieve the list the readers.
    // hSC was set by a previous call to SCardEstablishContext.
    lReturn = SCardListReaders(hSC,
                               NULL,
                               (LPTSTR)&pmszReaders,
                               &cch );
    switch( lReturn )
    {
        case SCARD_E_NO_READERS_AVAILABLE:
           // qDebug("Reader is not in groups.\n");
        qDebug()<<"Reader is not in groups.\n";
            // Take appropriate action.
            // ...
        returnCardReaderStatus = false;
            break;

        case SCARD_S_SUCCESS:
            // Do something with the multi string of readers.
            // Output the values.
            // A double-null terminates the list of values.
            pReader = pmszReaders;
            while ( '\0' != *pReader )
            {
                // Display the value.
                //qDebug("Reader: %S\n", pReader );

                QString convert = QString::fromWCharArray(pReader);

                //qDebug()<<convert;
               qDebug()<<"Reader: %S"<<convert;

               nfccard = convert;
               returnCardReaderStatus = true;

                // Advance to the next value.
               pReader = pReader + wcslen((wchar_t *)pReader) + 1;
//                QString convert = QString::fromchararray(pReader);
//                qDebug()<<convert;

            }
            // Free the memory.
            lReturn2 = SCardFreeMemory( hSC,
                                       pmszReaders );
            if ( SCARD_S_SUCCESS != lReturn2 )
               // qDebug("Failed SCardFreeMemory\n");
                qDebug()<<"Failed SCardFreeMemory\n";
            break;

    default:
            //qDebug("Failed SCardListReaders\n");
        qDebug()<<"Failed SCardListReaders\n";
            // Take appropriate action.
            // ...
        returnCardReaderStatus = false;
            break;
    }
}

void multithread:: CardConnect(QString s)
{

    LONG            lReturn;
    DWORD           dwAP;

    lReturn = SCardConnect( hSC,
                            (LPCTSTR)s.unicode(),
                            SCARD_SHARE_SHARED,
                            SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                            &hCardHandle,
                            &dwAP);

    if ( SCARD_S_SUCCESS != lReturn )
    {
       // qDebug("Failed SCardConnect\n");
        qDebug()<<"Failed SCardConnect\n";
        returnStatus = false;

       // exit(1);  // Or other appropriate action.
    }else{
        returnStatus = true;
        // Use the connection.
        // Display the active protocol.
        switch ( dwAP )
        {
            case SCARD_PROTOCOL_T0:
                //qDebug("Active protocol T0\n");
            qDebug()<<"Active protocol T0\n";
                break;

            case SCARD_PROTOCOL_T1:
              //  qDebug("Active protocol T1\n");
            qDebug()<<"Active protocol T1\n";
                break;

            case SCARD_PROTOCOL_UNDEFINED:

            default:
               // qDebug("Active protocol unnegotiated or unknown\n");
            qDebug()<<"Active protocol unnegotiated or unknown\n";
                break;

        }
    }
}

void multithread::CardListReaderGroups()
{
    LPTSTR          pmszReaderGroups = NULL;
    LPTSTR          pReaderGroup;
    LONG            lReturn;
    DWORD           cch = SCARD_AUTOALLOCATE;

    // Retrieve the list the reader groups.
    // hSC was set by a previous call to SCardEstablishContext.
    lReturn = SCardListReaderGroups(hSC,
                                    (LPTSTR)&pmszReaderGroups,
                                    &cch );
    if ( SCARD_S_SUCCESS != lReturn )
       // qDebug("Failed SCardListReaderGroups\n");
        qDebug()<<"Failed SCardListReaderGroups\n";
    else
    {
        // Do something with the multi string of reader groups.
        // Output the values.
        // A double-null terminates the list of values.
        pReaderGroup = pmszReaderGroups;
        while ( '\0' != *pReaderGroup )
        {
            // Display the value.
           // qDebug("%S\n", pReaderGroup );
            QString convert = QString::fromWCharArray(pReaderGroup);
            qDebug()<<"%S"<<convert;
            // Advance to the next value.
            pReaderGroup = pReaderGroup + wcslen((wchar_t *) pReaderGroup) + 1;
        }

        // Remember to free pmszReaderGroups by a call to SCardFreeMemory.
        // ...
    }
}

void multithread::CardListCards()
{
    LPTSTR pmszCards = NULL;

    LONG lReturn;
    DWORD cch = SCARD_AUTOALLOCATE;
    DWORD *null = NULL;
    SCARDCONTEXT *scard = NULL;
    // Retrieve the list of cards.
    lReturn = SCardListCards(*scard, NULL, NULL, *null, (LPTSTR)&pmszCards, &cch );
    if ( SCARD_S_SUCCESS != lReturn )
    {
//        qDebug("Failed SCardListCards\n");
        qDebug()<<"Failed SCardListCards\n";
        exit(1); // Or other appropriate error action
    }
    // Do something with the multi string of cards.
    // Output the values.
    // A double-null terminates the list of values.
    pCard = pmszCards;
    while ( '\0' != *pCard )
    {
        // Display the value.
//       qDebug("%S\n", pCard );
        qDebug()<<"%S"<<pCard << " value";
        // Advance to the next value.
        pCard = pCard + wcslen(pCard) + 1;
    }

    // Remember to free pmszCards (by calling SCardFreeMemory).
    // ...
}

void multithread::CardIntroduceCardType()
{
    GUID  MyGuid = { 0xABCDEF00,
                     0xABCD,
                     0xABCD,
                     0xAA, 0xBB, 0xCC, 0xDD,
                     0xAA, 0xBB, 0xCC, 0xDD };

    static const BYTE MyATR[] =     { 0xaa, 0xbb, 0xcc, 0x00, 0xdd };
    static const BYTE MyATRMask[] = { 0xff, 0xff, 0xff, 0x00, 0xff};

    LONG            lReturn;

    lReturn = SCardIntroduceCardType(hContext,
                                     L"ACS ACR122 0",
                                     &MyGuid,
                                     NULL,    // No interface array
                                     0,       // Interface count = 0
                                     MyATR,
                                     MyATRMask,
                                     sizeof(MyATR));
    if ( SCARD_S_SUCCESS != lReturn ){
       // printf("Failed SCardIntroduceCardType\n");
        qDebug()<<"Failed SCardIntroduceCardType\n";
    }
    else if(SCARD_S_SUCCESS == lReturn)
    {
        qDebug()<<"card introduced success";
    }
}

void multithread::CardReleaseContext()
{
    LONG lReturn = SCardReleaseContext(hSC);

    if ( SCARD_S_SUCCESS != lReturn ){
        qDebug("Failed SCardReleaseContext\n");
    }
    else{
        qDebug("Succcess SCardReleaseContext\n");
    }

}

void multithread::CardStatus()
{

    WCHAR           szReader[200];
    DWORD           cch = 200;
    BYTE            bAttr[32];
    DWORD           cByte = 32;
    DWORD           dwState, dwProtocol;
    LONG            lReturn;

    // Determine the status.
    // hCardHandle was set by an earlier call to SCardConnect.
    lReturn = SCardStatus(hCardHandle,
                          szReader,
                          &cch,
                          &dwState,
                          &dwProtocol,
                          (LPBYTE)&bAttr,
                          &cByte);

    if ( SCARD_S_SUCCESS != lReturn )
    {
//        printf("Failed SCardStatus\n");
        qDebug()<<"Failed SCardStatus\n";
//        exit(1);     // or other appropriate action
    }

//    Examine retrieved status elements.
//    Look at the reader name and card state.
//    printf("%S\n", szReader );

    qDebug()<<"%S\n"<<szReader;

    switch ( dwState )
    {
        case SCARD_ABSENT:
        qDebug()<<"Card absent.\n";
            cardUID = "";
            emit CardStatusName("Card absent.\n");
            break;
        case SCARD_PRESENT:
       qDebug()<<"Card present.\n";
       emit CardStatusName("Card present.\n");
            break;
        case SCARD_SWALLOWED:
        qDebug()<<"Card swallowed.\n";
        emit CardStatusName("Card swallowed.\n");
            break;
        case SCARD_POWERED:
        qDebug()<<"Card has power.\n";
        emit CardStatusName("Card has power.\n");
            break;
        case SCARD_NEGOTIABLE:
        qDebug()<<"Card reset and waiting PTS negotiation.\n";
        emit CardStatusName("Card reset and waiting PTS negotiation.\n");
            break;
        case SCARD_SPECIFIC:
        qDebug()<<"Card has specific communication protocols set.\n";
            emit CardStatusName("Card has specific communication protocols set.\n");
            break;
        default:
            qDebug()<<"Unknown or unexpected card state.\n";
            emit CardStatusName("Unknown or unexpected card state.");
            cardUID = "";
            break;
    }
}

void multithread::CardTransmit()
{
   BYTE pbRecv[258];
   BYTE  pbSend[] = {0xFF,0xCA,0x00,0x00,0x00};
   DWORD dwSend,dwRecv;
   dwSend = sizeof(pbSend);
   dwRecv = sizeof(pbRecv);

    LONG lReturn;
    lReturn = SCardTransmit(hCardHandle,
                            SCARD_PCI_T1,
                            pbSend,
                            dwSend,
                            NULL,
                            pbRecv,
                            &dwRecv );
    if ( SCARD_S_SUCCESS != lReturn )
    {
       // printf("Failed SCardTransmit\n");
        qDebug()<<"Failed SCardTransmit\n";
        //exit(1);   // or other appropriate error action
    }
    else if(SCARD_S_SUCCESS == lReturn)
    {

        QByteArray qdb = (const char*)(pbRecv);
        qdb.chop(1);
        cardUID = QString(qdb.toHex());
        qDebug()<<"Success "<< qdb;
    }
}

void multithread::CardDisconnect()
{
    LONG lReturn = SCardDisconnect(hCardHandle,
                              SCARD_UNPOWER_CARD);
    if ( SCARD_S_SUCCESS != lReturn )
    {
        qDebug("Failed SCardDisconnect");
        exit(1);  // Or other appropriate action.
    }else if(SCARD_S_SUCCESS == lReturn){
        qDebug("SUCCESS CARD DISCONNECT!");
    }
}

void multithread::CardFreeMemory()
{
    LONG lReturn = SCardFreeMemory(hSC,
                              pCard );
    if ( SCARD_S_SUCCESS == lReturn )
        qDebug("SUCCESS SCardFreeMemory");
    else{
        qDebug("FAIL SCardFreeMemory");
    }
}

void multithread::CardAuthenticate(int blockNo)
{
    BYTE pbRecv[258];
    BYTE  pbSend[] = {0xFF,0x88,0x00,0x04,0x60,0x00};
    pbSend[3] = blockNo;
    DWORD dwSend,dwRecv;
    dwSend = sizeof(pbSend);
    dwRecv = sizeof(pbRecv);

     LONG lReturn;
     lReturn = SCardTransmit(hCardHandle,
                             SCARD_PCI_T1,
                             pbSend,
                             dwSend,
                             NULL,
                             pbRecv,
                             &dwRecv );
     if ( SCARD_S_SUCCESS != lReturn )
     {
        // printf("Failed SCardTransmit\n");
         qDebug()<<"Failed SCardTransmit\n";
         //exit(1);   // or other appropriate error action
     }
     else if(SCARD_S_SUCCESS == lReturn)
     {

         QByteArray qdb = (const char*)(pbRecv);
         qDebug()<<"Success Authentication: "<< qdb;

     }
}
