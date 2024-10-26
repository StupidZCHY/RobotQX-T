#include "mycarconnnect.h"
#include <QFileDialog>
#include <QMessageBox>

MyCarConnnect::MyCarConnnect()
{

    //connect();
}
QList<QByteArray> splitByteArray(const QByteArray &byteArray, int size) {
    QList<QByteArray> result;
    for (int i = 0; i < byteArray.size(); i += size) {
        result.append(byteArray.mid(i, qMin(size, byteArray.size() - i)));
    }
    return result;
}

void MyCarConnnect::run()
{
    //bin文件相关
    QByteArray datStream1 ;//从bin文件中读出得所有数据
    QString binName;    //bin文件得名称  字符串
    QString binSize;    //bin文件得大小 字符串格式
    int binlength=0;
    //发送文件进度条显示
    // uint64_t Pgb_iap_max=0;
    int i = 0;
    // _carMessageThread = new CarMessage();
    //_carMessageThread->CarMessageinit();
    //模式中执行步骤
    int step =0;
    int number=0;
    int framecount = 0;
    QList<QByteArray> splitData;
    uint16_t checksum=0;
    QByteArray SOHArray(134,0);
    QByteArray STXArray(1029,0);
    QByteArray senddatatemp(1024,0);//读取文件缓存

    uint8_t type;
    uint8_t command;
    uint16_t length;
    QByteArray car_state_read;  //工作模式
    /*
    0   无 不执行任何操作
    1   参数读取模式
    2   iap模式
    */
    while(true)
    {
        if(m_mode == 1)
        {
            while(true)
            {
                if(m_autoRwParam == 1)
                {
                    step = 0;
                    emit myThreadSignal(tr("当前步骤%1  发送查询指令").arg(step));
                    type=1;
                    command = 0x31;
                    length = 1;
                    car_state_read[0] = 1;
                    ClearMessageFlag();
                    emit myThreadIapSend(
                                type,
                                command,
                                car_state_read,
                                length);
                    msleep(100);
                   // WaitOneCharData(0X06,10000);

                }

                if(m_mode != 1)
                {
                    break;
                }
                //msleep(1000);
            }
        }
        else if(m_mode == 2)
        {
            while(true)
            {

                if(step == 0)   //第一步
                {
                    emit myThreadSignal(tr("当前步骤%1  准备打开文件").arg(step));
                    //打开文件
                    QFile file(m_FilePath);
                    if (!file.open(QIODevice::ReadOnly)) {
                        emit myThreadSignal(tr("当前步骤%1  打开文件失败").arg(step));
                        //打开失败// QMessageBox::critical(this, tr("Error"), tr("Cannot open file for reading."));
                        return;
                    }

                    QDataStream in(&file);
                    in.setByteOrder(QDataStream::LittleEndian);

                    while (!in.atEnd()) {
                            qint8 byte;
                            in >> byte;
                            datStream1.append(byte);
                        }
                    QFileInfo fileInfo(m_FilePath);
                    binName = fileInfo.fileName(); // 获取文件名
                    qint64 filesize = fileInfo.size(); // 获取文件大小（字节
                    binlength = filesize;
                    binSize= QString::number(filesize);

                    emit myThreadSignal(tr("当前步骤%1  打开文件成功!文件名称：%2；文件大小：%3").arg(step).arg(binName).arg(filesize));

                    file.close();
                    //splitData  = splitByteArray(allData,binlength);
                    step = 1;
                }
                else if( step == 1)//开始发送
                {
                    emit myThreadSignal(tr("当前步骤%1  准备发送文件名称及长度").arg(step));

                    if(WaitOneCharData(0X43,10000))
                    {
                        emit myThreadSignal(tr("当前步骤%1  接收到回复0x43信息").arg(step));
                    }
                    else
                    {
                        emit myThreadSignal(tr("当前步骤%1  iap升级超时失败").arg(step));
                        m_mode = 1;
                        continue;
                    }
                    QByteArray filenameArray = binName.toUtf8();
                    QByteArray filelengthArray = binSize.toUtf8() ;
                    //发送文件名称
                    SOHArray[0] = 0x01;
                    SOHArray[1] = framecount;
                    SOHArray[2] = (uchar)(0xff - framecount);
                    for (i = 0; i < filenameArray.length(); i++)
                    {
                        SOHArray[3 + i] = filenameArray[i];
                    }
                    SOHArray[3 + i] = 0x00;
                    int fc = 4 + i;
                    for (i = 0; i < filelengthArray.length(); i++)
                    {
                        SOHArray[fc + i] = filelengthArray[i];
                    }
                    SOHArray[fc + i] = 0x00;
                    checksum = IapCRC16_cal(SOHArray, 3,  128);
                    SOHArray[132 ] = (uint8_t)(checksum & 0x00ff);
                    SOHArray[131] = (uint8_t)((checksum & 0xff00)>>8);
                    number = 133;
                    ClearMessageFlag();
                    emit myThreadIapSend(
                                0,0, SOHArray,number);
                    emit myThreadSignal(tr("当前步骤%1  文件名称及长度发送完成").arg(step));
                    if(WaitOneCharData(0X06,10000))
                    {
                        step = 2;
                        emit myThreadSignal(tr("当前步骤%1  接收到回复0x06信息").arg(step));
                    }
                    else
                    {
                        emit myThreadSignal(tr("当前步骤%1  iap升级超时失败").arg(step));
                        m_mode = 1;
                    }
                    emit SigIapSendNum(framecount,binlength / 1024);


                }
                else if(step == 2)
                {
                    emit myThreadSignal(tr("当前步骤%1  开始发送文件").arg(step));

                    //开始发送文件内容

                    if(WaitOneCharData(0X43,10000))
                    {
                        emit myThreadSignal(tr("当前步骤%1  接收到请求数据信号0x43").arg(step));
                    }
                    else
                    {
                        emit myThreadSignal(tr("当前步骤%1  iap升级超时失败").arg(step));
                        step = 0;
                        m_mode=1;
                        continue;
                    }
                    for (i = 0; i < binlength / 1024; i++)
                    {
                        senddatatemp.clear();
                        senddatatemp = datStream1.mid(1024*framecount,1024);
                        // QByteArray senddatatemp= splitData.at(framecount);
                        ++framecount;
                        STXArray.clear();
                        STXArray[0] = 0x02;
                        STXArray[1] = framecount;
                        STXArray[2] = (0xff - framecount);
                        for (int j = 0; j < 1024; j++)
                        {
                            STXArray[3 + j] = senddatatemp[j];
                        }
                        checksum = IapCRC16_cal(STXArray,3,  1024);

                        STXArray[1028] = (uint8_t)(checksum & 0x00ff);
                        STXArray[1027 ] = (uint8_t)((checksum & 0xff00)>>8);
                        number = 1029;
                        ClearMessageFlag();
                        emit myThreadIapSend(
                                    0,
                                    0,
                                    STXArray,
                                    number);

                        emit SigIapSendNum(framecount,binlength / 1024);
                        emit myThreadSignal(tr("当前步骤%1  发送第%2帧数据").arg(step).arg(framecount));
                        if(WaitOneCharData(0X06,10000))
                        {
                            emit myThreadSignal(tr("当前步骤%1  接收到请求数据信号0x06").arg(step));
                        }
                        else
                        {
                            emit myThreadSignal(tr("当前步骤%1  iap升级超时失败").arg(step));
                            step = 0;
                            m_mode=1;
                            break;
                        }
                    }
                    emit myThreadSignal(tr("当前步骤%1  发送文件剩余一帧").arg(step));

                    step = 3;
                }
                else if(step == 3  )
                {
                    emit SigIapSendNum(framecount,binlength / 1024);
                    //剩余部分先判断大小
                    if (binlength % 1024 <= 128 && binlength % 1024 != 0)
                    {
                        emit myThreadSignal(tr("当前步骤%1  开始发送最后一帧文件128").arg(step));

                        senddatatemp.clear();
                        senddatatemp = datStream1.right(binlength-1024*framecount);

                        ++framecount;
                        //ui->Pgb_IAP->setValue(framecount);
                        SOHArray[0] = 0x01;
                        SOHArray[1] = framecount;
                        SOHArray[2] = (0xff - framecount);

                        for (i = 0; i < 128; i++)
                        {
                            SOHArray[3 + i] = 0x1a;
                        }
                        for (i = 0; i < binlength % 1024; i++)
                        {
                            SOHArray[3 + i] = senddatatemp[i];
                        }

                        checksum = IapCRC16_cal(SOHArray,  3, 128);
                        SOHArray[132 ] = (uint8_t)(checksum & 0x00ff);
                        SOHArray[131] = (uint8_t)((checksum & 0xff00)>>8);
                        number = 133;
                        ClearMessageFlag();
                        emit myThreadIapSend(
                                    0,0, SOHArray,number);
                    }
                    else if (binlength % 1024 > 128 && binlength % 1024 != 0)
                    {
                        emit myThreadSignal(tr("当前步骤%1  开始发送最后一帧文件1024").arg(step));

                        senddatatemp.clear();
                        senddatatemp = datStream1.right(binlength-1024*framecount);

                        STXArray.clear();
                        ++framecount;
                        // ui->Pgb_IAP->setValue(framecount);
                        STXArray[0] = 0x02;
                        STXArray[1] = framecount;
                        STXArray[2] = (0xff - framecount);
                        for (i = 0; i < 1024; i++)
                        {
                            STXArray[3 + i] = 0x1a;
                        }
                        for (i = 0; i < binlength % 1024; i++)
                        {
                            STXArray[3 + i] = senddatatemp[i];
                        }
                        checksum = IapCRC16_cal(STXArray,3,   1024);
                        STXArray[1028 ] = (uint8_t)(checksum & 0x00ff);
                        STXArray[1027] = (uint8_t)((checksum & 0xff00)>>8);
                        number = 1029;
                        ClearMessageFlag();
                        emit myThreadIapSend(
                                    0,0, STXArray,number);

                    }
                    if(WaitOneCharData(0X06,10000))
                    {
                        emit myThreadSignal(tr("当前步骤%1  接收到请求数据信号0x06").arg(step));
                    }
                    else
                    {

                        emit myThreadSignal(tr("当前步骤%1  iap升级超时失败").arg(step));
                        step = 0;
                        m_mode=1;
                        break;
                    }
                    //等待0x06
                    //等待0x43
                    emit myThreadSignal(tr("当前步骤%1  跳过等待").arg(step));

                    number = 1;
                    QByteArray sendbyte(1,number);

                    //发送0x04
                    sendbyte[0] = 0x04;
                    ClearMessageFlag();
                    emit myThreadIapSend(
                                0,0, sendbyte,number);
                    if(WaitOneCharData(0X43,10000))
                    {
                        emit myThreadSignal(tr("当前步骤%1  接收到请求数据信号0x43").arg(step));
                    }
                    else
                    {
                        emit myThreadSignal(tr("当前步骤%1  iap升级超时失败").arg(step));

                        step = 0;
                        m_mode=1;
                        break;
                    }
                    step = 4;
                }
                else if(step == 4)
                {
                    emit myThreadSignal(tr("当前步骤%1  发送结尾").arg(step));

                    STXArray[0] = 0x02;
                    STXArray[1] = 0;
                    STXArray[2] = 0xff;
                    for (int j = 0; j < 1024; j++)
                    {
                        STXArray[3 + j] = 0;
                    }

                    STXArray[1027] = 0;
                    STXArray[1028] = 0;
                    number = 1029;
                    ClearMessageFlag();
                    emit myThreadIapSend(
                                0,0, STXArray,number);

                    emit myThreadSignal(tr("当前步骤%1  结尾发送完成").arg(step));

                    m_mode = 1;
                }
                if(m_mode != 2)
                {
                    framecount=0;
                    emit SigIapSendNum(framecount,binlength / 1024);
                    step=0;
                    datStream1.clear();
                    msleep(1000);
                    break;
                }
                //msleep(10);
            }
        }
    }

}

void MyCarConnnect::ClearMessageFlag()
{
    m_flag = false;
    m_command = 0;
    m_totaldat.clear();
    m_length = 0;
    m_time = 0;
}
bool MyCarConnnect::WaitOneCharData(char dat,int time)
{
    bool ret =false;
    while(time--)
    {
        if(m_totaldat.contains(dat))
        {
            ret = true;
            return ret;
        }
        msleep(1);
    }
    return ret;
}



