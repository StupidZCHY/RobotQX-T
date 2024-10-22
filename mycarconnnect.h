#ifndef MYCARCONNNECT_H
#define MYCARCONNNECT_H

#include <QThread>
#include <QDebug>
#include "SCStatusTcp.h"
#include "CRC.h"
#include "carmessage.h"

class MyCarConnnect : public QThread
{
    Q_OBJECT

public:
    void RevOneDatMessage(uint8_t dat);
    bool WaitOneCharData(char dat,int time);
    MyCarConnnect();
    // MyThread(QObject *parent = nullptr) : QThread(parent), m_param(0) {}
    void setParam(int param) {
        m_param = param;
    }
    void setautoRwParam(int flag) {
        m_autoRwParam = flag;
    }
    void setMode(int mode) {
        m_mode = mode;
    }
    void setFilePath(QString FilePath) {
        m_FilePath = FilePath;
    }
    void SetOneMessage(bool flag ,
                       uint8_t command,  //命令
                       QByteArray totaldat ,
                       uint16_t length,
                       int mtime){
        m_flag = flag;
        m_command = command; //命令
         m_totaldat =totaldat;
         m_length =length;
         m_time =mtime ;
    }

    void run() override;
    CarMessage *_carMessageThread {Q_NULLPTR};

signals:

    void myThreadIapSend(int ,
                         int ,
                         QByteArray,
                         int
                         );  // 自定义发送的信号
    void myThreadSignal(
            QString
            );  // 自定义发送的信号
    void SigIapSendNum(
            int,//当前第多少帧
            int //总帧数
            );  // 自定义发送的信号


public slots:
//    void SlotIapRevData(bool ,  //是否成功
//                        uint8_t ,//命令
//                        QByteArray  ,//数据
//                        int ,       //数据长度
//                        int );

private:
    void ClearMessageFlag();
    QByteArray m_datarry;
    int m_autoRwParam = 0;
    int m_param = 0;
    uint8_t m_mode =1 ;
    bool m_flag ;
    int m_command; //命令
    QByteArray m_totaldat ;
    int m_length;
    int m_time;
    QString m_FilePath;


};

#endif // MYCARCONNNECT_H
