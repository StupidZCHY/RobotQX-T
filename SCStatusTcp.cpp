#include "SCStatusTcp.h"
#include "SCHeadData.h"
#include <QDateTime>
#include <QNetworkProxy>

#include "CRC.h"
SCStatusTcp::SCStatusTcp(QObject *parent) : QObject(parent),
    _tcpSocket(Q_NULLPTR)
{

}
SCStatusTcp::~SCStatusTcp()
{
    releaseTcpSocket();
    if(_tcpSocket){
        delete _tcpSocket;
    }
}
/** 释放tcpSocket
 * @brief SCStatusTcp::releaseTcpSocket
 */
void SCStatusTcp::releaseTcpSocket()
{
    if(_tcpSocket){

        if(_tcpSocket->isOpen()){
            _tcpSocket->close();
        }
        _tcpSocket->abort();
    }
}
/** 连接
 * @brief SCStatusTcp::connectHost
 * @param ip
 * @param port
 * @return
 */
int SCStatusTcp::connectHost(const QString&ip,quint16 port)
{
    int ret = 0;
    if(!_tcpSocket){
        _tcpSocket = new QTcpSocket(this);
        //无代理，不加这行会导致开启 vpn 无法连接局域网设备.
        _tcpSocket->setProxy(QNetworkProxy::NoProxy);
        //连接接收数据信号
        connect(_tcpSocket,
                SIGNAL(readyRead()),
                this,
                SLOT(receiveTcpReadyRead()));
        //传输连接状态1
        connect(_tcpSocket,
                SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this->parent(),
                SLOT(stateChanged1(QAbstractSocket::SocketState)));
        //传输连接状态2
        connect(_tcpSocket,
                SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this->parent(),
                SLOT(stateChanged2(QAbstractSocket::SocketState)));
        //传输错误状态
        connect(_tcpSocket,
                SIGNAL(error(QAbstractSocket::SocketError)),
                this->parent(),
                SLOT(receiveTcpError(QAbstractSocket::SocketError)));
    }
    if(_tcpSocket->isOpen()//连接时连接状态时断开连接
            &&(_tcpSocket->state()==QAbstractSocket::ConnectedState
               ||_tcpSocket->state()==QAbstractSocket::ConnectingState)){
        _tcpSocket->close();
        _tcpSocket->abort();
        qDebug()<<"----close _tcpSocket----\n";
        ret = 1;
    }else{
        _tcpSocket->connectToHost(ip,port,QTcpSocket::ReadWrite,QTcpSocket::IPv4Protocol);
        _ip = ip;
        _port = port;
        ret = 0;
    }
    //清空缓冲区
    if(_tcpSocket->isOpen())
        _tcpSocket->readAll();
    return ret;
}

/** TCP请求
 * @brief SCStatusTcp::writeTcpData
 * @param sendCommand 报文类型.
 * @param sendData 数据区数据.
 * @param jsonData Json区数据.
 * @param number 序号.
 * @return
 */
bool SCStatusTcp::writeTcpData(uint16_t sendCommand,
                               const QByteArray &jsonData,
                               const QByteArray &sendData,
                               uint16_t &number,
                               uint8_t byte15)
{
    //已发送.
    _oldSendCommand = sendCommand;
    _oldNumber = number;
    //数据区长度.
    int size = 0;
    //报文头部数据.
    uint8_t* headBuf = Q_NULLPTR;
    int headSize = 0;
    //发送的全部数据.
    SeerData* seerData = Q_NULLPTR;
    //开始计时.
    _time.start();

    //json数据
    uint16_t jsonSize = 0;
    if(!jsonData.isEmpty()){
        jsonSize = jsonData.toStdString().length();
    }
    QByteArray totalData = jsonData + sendData;


    //根据数据区数据进行数据转换.
    if(totalData.isEmpty()){
        headSize = sizeof(SeerHeader);
        headBuf = new uint8_t[headSize];
        seerData = (SeerData*)headBuf;
        size = seerData->setData(sendCommand,Q_NULLPTR,0,0,number,byte15);
    }else{
        std::string totalDataStr = totalData.toStdString();
        headSize = sizeof(SeerHeader) + totalDataStr.length();
        headBuf = new uint8_t[headSize];
        seerData = (SeerData*)headBuf;
        size = seerData->setData(sendCommand,
                                 (uint8_t*)totalDataStr.data(),
                                 totalDataStr.length(),
                                 jsonSize,
                                 number,byte15);
    }
    //---------------------------------------
    //发送的所有数据.
    QByteArray tempA = QByteArray::fromRawData((char*)seerData,size);
    qDebug()<<"send:"<<QString(tempA)<<"  Hex:"<<tempA.toHex()<<"seerData:size:"<<size;
    QString dataHex = "";
    if(size<=2048){
        dataHex = hexToQString(sendData.toHex());
    }else{
        dataHex = tr("If the data region length is larger than 2048 bytes, it will not be printed");
    }
    //打印信息.
    QString info = tr("\n%1---------[Request] ---------\n"
                      "Type:%2 (0x%3) \n"
                      "Port: %4\n"
                      "Number: %5 (0x%6)\n"
                      "Head hex: %7 \n"
                      "Data[size:%8 (0x%9)]: %10 \n"
                      "Data hex: %11 \n"
                      "JSON[size:%12]: %13")
            .arg(getCurrentDateTime())
            .arg(sendCommand)
            .arg(QString::number(sendCommand,16))
            .arg(_port)
            .arg(number)
            .arg(QString::number(number,16))
            .arg(hexToQString(tempA.left(16).toHex()))
            .arg(sendData.size())
            .arg(QString::number(sendData.size(),16))
            .arg(QString(sendData))
            .arg(dataHex)
            .arg(jsonSize)
            .arg(QString(jsonData));

    emit sigPrintInfo(info);
    //---------------------------------------
    _tcpSocket->write((char*)seerData, size);
    delete[] headBuf;

    //-------------
    qDebug()<<"TCP:_timeOut:"<<_timeOut;
    //如果_timeOut = 0表示不监听超时.
    if(0 == _timeOut){
        return true;
    }

    //等待写入.
    if(!_tcpSocket->waitForBytesWritten(_timeOut)){
        _lastError = tr("waitForBytesWritten: time out!");
        return false;
    }
    //等待读取.
    if(!_tcpSocket->waitForReadyRead(_timeOut)){
        _lastError = tr("waitForReadyRead: time out!");
        return false;
    }
    return true;
}

/** TCP请求
 * @brief SCStatusTcp::writeTcpData
 * @param sendCommand 报文类型.
 * @param sendData 数据区数据.
 * @param jsonData Json区数据.
 * @param number 序号.
 * @return
 */
bool SCStatusTcp::writeTcpSimpleData(uint8_t sendCommand,
                                     const uint8_t* sendData,
                                     uint16_t &number
                                     )
{
    uint16_t crcsum=0;
    int length1=0;
    length1 = number+7;

    QByteArray senddatByte(length1,0);

    uint8_t senddat[20];
    senddatByte[0] = 0x6b;
    senddatByte[1] = 0x78;
    senddatByte[2] = (length1>>8)&0xff;
    senddatByte[3] = (length1)&0xff;
    senddatByte[4] = sendCommand;
    for(int i=0;i<number;i++)
    {
        senddatByte[5+i] = sendData[i];
    }

    crcsum = CRC16_cal(senddatByte,2,length1-4);
    senddatByte[length1-2] = (crcsum)&0xff ;
    senddatByte[length1-1] = (crcsum>>8)&0xff;
    for(int i=0;i<length1;i++)
    {
        senddat[i] = senddatByte[i];
    }

    _tcpSocket->write((char*)senddat, length1);

    //-------------
    qDebug()<<"TCP:_timeOut:"<<_timeOut;
    //如果_timeOut = 0表示不监听超时.
    if(0 == _timeOut){
        return true;
    }

    //等待写入.
    if(!_tcpSocket->waitForBytesWritten(_timeOut)){
        _lastError = tr("waitForBytesWritten: time out!");
        return false;
    }
    //等待读取.
    if(!_tcpSocket->waitForReadyRead(_timeOut)){
        _lastError = tr("waitForReadyRead: time out!");
        return false;
    }
    return true;
}


/** TCP请求
 * @brief SCStatusTcp::writeTcpData
 * @param sendCommand 报文类型.
 * @param sendData 数据区数据.
 * @param jsonData Json区数据.
 * @param number 序号.
 * @return
 */
bool SCStatusTcp::writeTcpOneData(
        const uint8_t* sendData,
        uint16_t &number
        )
{

    _tcpSocket->write((char*)sendData, number);

    //-------------
    qDebug()<<"TCP:_timeOut:"<<_timeOut;
    //如果_timeOut = 0表示不监听超时.
    if(0 == _timeOut){
        return true;
    }

    //等待写入.
    if(!_tcpSocket->waitForBytesWritten(_timeOut)){
        _lastError = tr("waitForBytesWritten: time out!");
        return false;
    }
    //等待读取.
    if(!_tcpSocket->waitForReadyRead(_timeOut)){
        _lastError = tr("waitForReadyRead: time out!");
        return false;
    }
    return true;
}



void SCStatusTcp::receiveTcpReadyRead()
{
    //读取所有数据.
    //返回的数据大小不定,需要使用_lastMessage成员变量存放多次触发槽读取的数据.
    QByteArray message = _tcpSocket->readAll();
    message = _lastMessage + message;
    int size = message.size();

    while(size > 0){
        char a0 = message.at(0);
        if (uint8_t(a0) == 0x5A){//检测第一位是否为0x5A.
            if (size >= 16) {//返回的数据最小长度为16位,如果大小小于16则数据不完整等待再次读取.
                SeerHeader* header = new SeerHeader;
                memcpy(header, message.data(), 16);

                uint32_t data_size;//返回所有数据总长值.
                uint16_t revCommand;//返回报文数据类型.
                uint16_t number;//返回序号.
                qToBigEndian(header->m_length,(uint8_t*)&(data_size));
                qToBigEndian(header->m_type, (uint8_t*)&(revCommand));
                qToBigEndian(header->m_number, (uint8_t*)&(number));

                //json区长度.
                uint16_t jsonSize = 0;
                {
                    uint8_t u2 = header->m_reserved[2];
                    uint8_t u3 = header->m_reserved[3];
                    uint16_t tempJsonSize = u2;
                    tempJsonSize = tempJsonSize << 8;
                    tempJsonSize = tempJsonSize | u3;
                    if(tempJsonSize <0 || tempJsonSize > 65535){
                        tempJsonSize = 0;
                    }
                    jsonSize = tempJsonSize;
                }

                delete header;

                uint32_t remaining_size = size - 16;//所有数据总长度 - 头部总长度16 = 数据区长度.
                //如果返回数据长度值 大于 已读取数据长度 表示数据还未读取完整，跳出循环等待再次读取..
                if (data_size > remaining_size) {
                    _lastMessage = message;

                    break;
                }else{//返回数据长度值 大于等于 已读取数据，开始解析.
                    QByteArray tempMessage;
                    if(_lastMessage.isEmpty())
                    {
                        tempMessage = message;
                    }
                    else
                    {
                        tempMessage = _lastMessage;
                    }
                    QByteArray headB = message.left(16);
                    QByteArray jsonData;
                    QByteArray byteData;


                    //截取报头16位后面的数据区数据.
                    {
                        QByteArray totalData = message.mid(16,data_size);
                        if(jsonSize > 0){
                            jsonData =  totalData.mid(0,jsonSize);
                            qDebug()<<">>>>>>>>>>>>>>>>>>>>>>jsonSize:"<<jsonSize
                                   <<"tempJsonData:"<<QString("[%1]").arg(QString(jsonData));
                        }
                        byteData =  totalData.mid(jsonSize,data_size - jsonSize);
                        qDebug()<<"jsonData:"<<QString(jsonData)<<"revByteData:"<<QString(byteData)<<"  Hex:"<<byteData.toHex();
                    }


                    //--------------------------------------
                    QString dataHex = "";
                    if(size<=2048){
                        dataHex = hexToQString(byteData.toHex());
                    }else{
                        dataHex = tr("If the data region length is larger than 2048 bytes, it will not be printed");
                    }
                    //输出打印信息.
                    QString info = QString("%1---------[Response]---------\n"
                                           "Type:%2 (%3) \n"
                                           "Number: %4 (0x%5)\n"
                                           "Head hex: %6\n"
                                           "Data[size:%7 (0x%8)]: %9 \n"
                                           "Data hex: %10 \n"
                                           "JSON[size:%11]: %12\n")
                            .arg(getCurrentDateTime())
                            .arg(revCommand)
                            .arg(QString::number(revCommand,16))
                            .arg(number)
                            .arg(QString::number(number,16))
                            .arg(hexToQString(headB.toHex()))
                            .arg(byteData.size())
                            .arg(QString::number(byteData.size(),16))
                            .arg(QString(byteData))
                            .arg(dataHex)
                            .arg(jsonData.size())
                            .arg(QString(jsonData));

                    emit sigPrintInfo(info);
                    int msTime = _time.elapsed();
                    //----------------------------------------
                    //输出返回信息.
                    emit sigChangedText(true,revCommand,
                                        byteData,byteData.toHex(),
                                        number,msTime);
                    //截断message,清空_lastMessage.
                    message = message.right(remaining_size - data_size);
                    size = message.size();
                    _lastMessage.clear();
                }

            } else{
                _lastMessage = message;
                break;
            }
        }else if(uint8_t(a0) == 0x6b  ){//检测第一位是否为0x6b.
            uint32_t remaining_size = size - 7;//所有数据总长度 - 头部总长度16 = 数据区长度.
            if (size >= 4) {//返回的数据最小长度为16位,如果大小小于16则数据不完整等待再次读取.
                int length=message[2]<<8|message[3];
                if(size < length)//长度符合接收长度
                {
                    _lastMessage = message;
                    break;
                }
                else
                {
                    int revCommand;
                    QByteArray tempMessage;
                    if(_lastMessage.isEmpty()){
                        tempMessage = message;
                    }else{
                        tempMessage = _lastMessage;
                    }
                    QByteArray headB = message.left(4);
                    QByteArray messagedata;
                    QByteArray byteData;
                    int number;
                    number = length-7; // 数据长度
                    revCommand = message.at(4);
                    QByteArray totalData = message.mid(5,number);
                    //截取报头16位后面的数据区数据.
                    {


                        qDebug()<<"totalData:"<<QString(totalData)<<"  Hex:"<<totalData.toHex();
                    }


                    //--------------------------------------
                    QString dataHex = "";
                    if(size<=2048){
                        dataHex = hexToQString(byteData.toHex());
                    }else{
                        dataHex = tr("If the data region length is larger than 2048 bytes, it will not be printed");
                    }
                    //输出打印信息.
                    QString info = QString("%1---------[Response]---------\n"
                                           "Type:%2 (%3) \n"
                                           "Number: %4 (0x%5)\n"
                                           "Head hex: %6\n"
                                           "Data[size:%7 (0x%8)]: %9 \n"
                                           "Data hex: %10 \n")
                            .arg(getCurrentDateTime())
                            .arg(revCommand)
                            .arg(QString::number(revCommand,16))
                            .arg(length)
                            .arg(QString::number(length,16))
                            .arg(hexToQString(headB.toHex()))
                            .arg(byteData.size())
                            .arg(QString::number(byteData.size(),16))
                            .arg(QString(byteData))
                            .arg(dataHex);

                    emit sigPrintInfo(info);
                    int msTime = _time.elapsed();
                    //----------------------------------------
                    //输出返回信息.
                    emit sigcarMChangedText(true,   //接收成功
                                            revCommand, //命令码
                                            totalData,  //数据
                                            number,     //数据长度
                                            msTime);
                    //截断message,清空_lastMessage.
                    message = message.right(remaining_size - length);
                    size = message.size();
                    _lastMessage.clear();
                }

            } else{
                _lastMessage = message;
                break;
            }
        }
        else{
             int msTime = _time.elapsed();
            uint8_t revCommand = 0xff;
            QByteArray totalData  =message;
            emit sigcarMChangedText(false,   //接收成功
                                    revCommand, //命令码
                                    totalData,  //数据
                                    totalData.length(),     //数据长度
                                    msTime);
            //报头数据错误.
            setLastError("Header Error !!!");
        message = message.right(size - 1);
        size = message.size();

        emit sigChangedText(false,_oldSendCommand,
                            _lastMessage,_lastMessage.toHex(),
                            0,msTime);

        }
    }
}

int SCStatusTcp::getTimeOut() const
{
    return _timeOut;
}

void SCStatusTcp::setTimeOut(int timeOut)
{
    _timeOut = timeOut;
}


QTcpSocket *SCStatusTcp::tcpSocket() const
{
    return _tcpSocket;
}

QElapsedTimer SCStatusTcp::time() const
{
    return _time;
}

void SCStatusTcp::setLastError(const QString &lastError)
{
    _lastError = lastError;
}

QString SCStatusTcp::lastError() const
{
    return _lastError;
}
/** 获取当前时间
 * @brief SCStatusTcp::getCurrentDateTime
 * @return
 */
QString SCStatusTcp::getCurrentDateTime()const
{
    return QDateTime::currentDateTime().toString("[yyyyMMdd|hh:mm:ss:zzz]:");
}
/** 16进制全部显示大写
 * @brief SCStatusTcp::hexToQString
 * @param b
 * @return
 */
QString SCStatusTcp::hexToQString(const QByteArray &b)
{
    QString str;
    for(int i=0;i<b.size();++i){
        ////        //每2位加入 空格0x
        ////        if((!(i%2)&&i/2)||0==i){
        ////            str+= QString(" 0x");
        ////        }
        str +=QString("%1").arg(b.at(i));
    }
    str = str.toUpper();
    return str;
}

