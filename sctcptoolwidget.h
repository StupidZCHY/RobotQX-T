#ifndef SCTCPTOOLWIDGET_H
#define SCTCPTOOLWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTranslator>
#include "SCStatusTcp.h"
#include "carmessage.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QPainter>
#include "mycarconnnect.h"
#include <QStandardItemModel>

#include <QDockWidget>
#include <QPushButton>
#include <QTextStream>
#include <QInputDialog>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDomNode>
#include <QPrinter>

#include "mbaseview.h"
#include "mbasescene.h"

#include "mtextobject.h"
#include "mlineobject.h"
#include "mrectobject.h"
#include "mellipseobject.h"


//json解析
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
namespace Ui {
class SCTcpToolWidget;
}

class SCTcpToolWidget : public QWidget
{
    Q_OBJECT

public:


    void itemDockWidgetInit(void);
    void tabWidgetInit(void);
    void xmlFileInit(QDomDocument *doc);
    bool appendObj2Xml(QDomDocument &doc, const QString &cls, const QDomElement &new_node);
    bool xmlIsExist(const QString &name);
    explicit SCTcpToolWidget(QWidget *parent = 0);
    ~SCTcpToolWidget();

    //explicit MyPainterWidget(QWidget *parent = nullptr);

    void endDraw();     //结束绘制
    void clearPath();   //清除路径

    //void paintEvent(QPaintEvent *event);

public slots:
    void BtnConnectStateUpdata(QString str);
    void slotPrintThreadInfo(
            QString );
    void slotIapThreadSend(int ,
                           int ,
                           QByteArray ,
                           int );

    void stateChanged1(QAbstractSocket::SocketState state);
    void stateChanged2(QAbstractSocket::SocketState state);
    void receiveTcpError(QAbstractSocket::SocketError error);
    void slotPrintInfo(QString info );

    void slotChangedText(bool isOk, int revCommand, QByteArray revData, QByteArray revHex, int number, int msTime);
    void slotCarMChangedText(bool isOk,
                             uint8_t revCommand,
                             QByteArray revData,
                             uint16_t number,
                             int msTime);
    void slotAutomaticallyScroll();
    void SlotCarAutoRwPara();
private slots:
    void slotPgb_IAPUp(int cnt,int num);
    void SlotRevOneMessage(bool flag ,
                           uint8_t command,  //命令
                           QByteArray totaldat ,
                           int length,
                           int mtime);
    void DrawMapByJson(int Magnification,QByteArray dat);
    void on_pushButton_connect_clicked();
    void on_pushButton_send_clicked();
    void on_pushButton_clearInfo_clicked();
    void on_pushButton_zipFile_clicked();
    void on_checkBox_timeOut_clicked(bool checked);
    void on_checkBox_queryTime_clicked(bool checked);
    void on_pushButton_openFolder_clicked();
    void on_toolButton_en_clicked();
    void on_toolButton_ch_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_btn_jackup_clicked();
    void on_btn_jackdown_clicked();
    void on_btn_confirmposition_clicked();
    void on_btn_carconnect_clicked();
    void on_btn_cancelNav_clicked();
    void on_btn_mapupdate_clicked();
    void on_btn_openmap_clicked();
    void on_btn_ReadLinpath_clicked();
    void on_pushButton_9_clicked();
    void on_btn_SendCmdIap_clicked();
    void on_btn_SendDataiap_clicked();
    void on_btn_cmdcarstop_clicked();
    void on_btn_cmdcarstartfront_clicked();
    void on_btn_cmdcarstartback_clicked();
    void on_btn_connectcarM_clicked();
    void on_btn_SendCmdIAPBegin_clicked();
    void on_btn_agvStart_clicked();
    void on_btn_setES_clicked();
    void on_btn_clearError_clicked();
    void on_btn_setActionNum_clicked();

    void on_btn_about_clicked();
    void on_btn_mapAmplify_clicked();

    void on_btn_mapReduce_clicked();


    void slotBtnText(void);
    void slotBtnLine(void);
    void slotBtnRect(void);
    void slotBtnEllipse(void);

    void slotActNew(void);
    void slotActOpen(void);
    void slotActSave(void);
    void slotActSaveAs(void);
    void slotActPrintImage(void);
    void slotActPrintPDF(void);

    void slotActLight(void);
    void slotActDark(void);

    /* Grid */
    void slotAct1Pixel(void);
    void slotAct2Pixel(void);
    void slotAct5Pixel(void);
    void slotAct10Pixel(void);




protected:

void closeEvent(QCloseEvent *event);
private:
    Ui::SCTcpToolWidget *ui;


    //仙工单独端口测试
    SCStatusTcp *_scStatusTcp {Q_NULLPTR};
    //仙工整车端口测试
    SCStatusTcp *_CarStatesTcp {Q_NULLPTR};
    SCStatusTcp *_CarControlTcp {Q_NULLPTR};
    SCStatusTcp *_CarNavigationTcp {Q_NULLPTR};
    SCStatusTcp *_CarConfigTcp {Q_NULLPTR};
    SCStatusTcp *_CarOtherTcp {Q_NULLPTR};

    MyCarConnnect *IapSendData1 {Q_NULLPTR};
    CarMessage *_carMessage {Q_NULLPTR};
    CarMessage *_carMessageTemp {Q_NULLPTR};

    QTimer *_queryTimer {Q_NULLPTR};
    QTranslator * _translator {Q_NULLPTR};

    QStandardItemModel *model;
    QGraphicsScene  *CarMapscene;

    QByteArray mapdat;
    int magnification;

    MBaseView *curView;

    const QString defaultXmlPath = "./xml/";

    void savePage(const QString &file);
    void openPage();


};

#endif // SCTCPTOOLWIDGET_H
