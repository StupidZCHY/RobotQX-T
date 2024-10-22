#include "carmessage.h"
#include <QTcpSocket>
#include <QHostInfo>

using namespace std;
CarMessage::CarMessage()
{
    _Carmagnetic = new SCStatusTcp();

    connect(_Carmagnetic,
            &SCStatusTcp::sigcarMChangedText,
            this,
            &CarMessage::SlotCarMessagePrintf);
}


//连接服务端
void CarMessage::ConnectToServer(QString ipAddress,QString Port)
{
    _Carmagnetic->connectHost(ipAddress,Port.toInt()) ;

}
uint8_t CarMessage::SendOneMessage( uint8_t dataType,uint8_t command, QByteArray senddata,uint16_t length)
{
    uint8_t ret=0;
    uint8_t data[length];
   // data[0] = 100;
    for(int i=0;i<length;i++)
    {
        data[i] = (uint8_t)senddata[i];
    }
    switch (dataType) {
    case 0:         //发送简单字符
        if(_Carmagnetic->tcpSocket()
                && _Carmagnetic->tcpSocket()->state()==QAbstractSocket::ConnectedState)
        {

            //发送数据.
            if(!_Carmagnetic->writeTcpOneData(data,
                                              length )){
                ;
                //            slotPrintInfo(tr("<font color=\"red\">"
                //                             "%1--------- Send error----------\n"
                //                             "Error: %2"
                //                             "</font> ")
                //                          .arg(_Carmagnetic->getCurrentDateTime())
                //                          .arg(_Carmagnetic->lastError()));
            }
            ret = 1;
        }else{

            ;
            //FIX </font> 后面的空格是一定要的，不然会串色.
            //ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
        }
        return 0;
        break;
    case 1:
        if(_Carmagnetic->tcpSocket()
                && _Carmagnetic->tcpSocket()->state()==QAbstractSocket::ConnectedState)
        {

            //发送数据.
            if(!_Carmagnetic->writeTcpSimpleData(command,
                                                 data,
                                                 length
                                                 )){
                ;
                //                slotPrintInfo(tr("<font color=\"red\">"
                //                                 "%1--------- Send error----------\n"
                //                                 "Type:%2  \n"
                //                                 "Error: %3"
                //                                 "</font> ")
                //                              .arg(_Carmagnetic->getCurrentDateTime())
                //                              .arg(sendCommand)
                //                              .arg(_Carmagnetic->lastError()));
            }
            ret = 1;
        }else{
            ;
            //FIX </font> 后面的空格是一定要的，不然会串色.
            // ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
        }
        break;
    default:
        break;
    }
    return ret;
}


void CarMessage::SlotCarMessagePrintf(bool flag,
                          uint8_t comand,  //命令
                          QByteArray totaldata,
                          uint16_t length,
                          uint16_t mtimeout)
{
    emit SigRevOneMessage(flag,comand,totaldata,length,mtimeout);
    if(flag == false)
    {

        comand = 0;
    }
}

