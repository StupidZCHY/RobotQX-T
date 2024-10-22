#ifndef CARMESSAGE_H
#define CARMESSAGE_H

#include "SCStatusTcp.h"
#include <QWidget>
#include <QTranslator>
class CarMessage  : public QObject
{

    Q_OBJECT
public:
    CarMessage();
    void ConnectToServer(QString ipAddress,
                         QString Port);
    uint8_t SendOneMessage(uint8_t dataType,
                           uint8_t command,
                           QByteArray senddata,
                           uint16_t length);
signals:
    void SigRevOneMessage(bool ,
                       uint8_t ,  //命令
                       QByteArray ,
                       uint16_t ,
                       uint16_t
                       );
    void SigCarConnectStates(QString);
public slots:
    void SlotCarMessagePrintf(bool,
                              uint8_t,  //命令
                              QByteArray,
                              uint16_t,
                              uint16_t);
private:
    //磁导航小车连接
    SCStatusTcp *_Carmagnetic {Q_NULLPTR};
};

#endif // CARMESSAGE_H
