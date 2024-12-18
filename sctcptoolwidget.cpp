#include "sctcptoolwidget.h"
#include "ui_sctcptoolwidget.h"
#include <QDateTime>
#include <QFileDialog>
#include <QHostInfo>
#include <QDesktopServices>
#include <QMessageBox>

#include <QPainter>
#include <QMouseEvent>

#include "CRC.h"
#include "QThread"
#include <iostream>
#include <QThreadPool>
#include "mycarconnnect.h"

#include <QTreeView>
#include <QFileSystemModel>


#include <QtMath>
#include <QMessageBox>
#include <QMenuBar>

#include "carmessage.h"
using namespace std;


SCTcpToolWidget::SCTcpToolWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SCTcpToolWidget)
{
    ui->setupUi(this);
    //设置
    ui->pushButton_clearInfo->setObjectName("pushButton_green");
    ui->pushButton_connect->setObjectName("pushButton_green");
    ui->pushButton_send->setObjectName("pushButton_green");
    ui->pushButton_zipFile->setObjectName("pushButton_green");


    QTreeView *treeView = new QTreeView(nullptr);
    QFileSystemModel *fileSystemModel = new QFileSystemModel();
    fileSystemModel->setRootPath(QDir::rootPath());

    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(QDir::homePath()));

    // 创建视图
    ui->tableView->resizeColumnToContents(0);
    ui->tableView->resizeColumnToContents(1);
    ui->tableView->resizeColumnToContents(2);
    ui->tableView->resizeColumnToContents(3);
    ui->tableView->resizeColumnToContents(4);
    ui->tableView->resizeColumnToContents(5);
    ui->tableView->resizeColumnToContents(6);
    ui->tableView->resizeColumnToContents(7);
    ui->tableView->resizeColumnToContents(8);
    ui->tableView->resizeColumnToContents(9);
    ui->tableView->resizeColumnToContents(10);
    ui->tableView->resizeColumnToContents(11);
    // 创建模型
    model = new QStandardItemModel(100, 12); // 4行2列
    model->setHeaderData(0, Qt::Horizontal, "序号");
    model->setHeaderData(1, Qt::Horizontal, "路径号");
    model->setHeaderData(2, Qt::Horizontal, "站点号");
    model->setHeaderData(3, Qt::Horizontal, "停车模式");
    model->setHeaderData(4, Qt::Horizontal, "重启方向");
    model->setHeaderData(5, Qt::Horizontal, "重启时长");
    model->setHeaderData(6, Qt::Horizontal, "动作指令");
    model->setHeaderData(7, Qt::Horizontal, "速度指令");
    model->setHeaderData(8, Qt::Horizontal, "岔道指令");
    model->setHeaderData(9, Qt::Horizontal, "障碍指令");
    model->setHeaderData(10, Qt::Horizontal, "跳转指令");
    model->setHeaderData(11, Qt::Horizontal, "执行方向");
    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->show();
    //仙工地图绘制时得放大比例初始化
    magnification = 20;


    //自动滚动.
    connect(ui->textEdit_info,SIGNAL(textChanged()),this,SLOT(slotAutomaticallyScroll()));
    //tcp
    _scStatusTcp = new SCStatusTcp(this);
    connect(_scStatusTcp,SIGNAL(sigPrintInfo(QString)),this,SLOT(slotPrintInfo(QString)));
    connect(_scStatusTcp,SIGNAL(sigChangedText(bool,int,QByteArray,QByteArray,int,int)),
            this,SLOT(slotChangedText(bool,int,QByteArray,QByteArray,int,int)));

    _CarStatesTcp = new SCStatusTcp(this);
    connect(_CarStatesTcp,SIGNAL(sigPrintInfo(QString)),this,SLOT(slotPrintInfo(QString)));
    connect(_CarStatesTcp,SIGNAL(sigChangedText(bool,int,QByteArray,QByteArray,int,int)),
            this,SLOT(slotChangedText(bool,int,QByteArray,QByteArray,int,int)));

    _CarControlTcp = new SCStatusTcp(this);
    connect(_CarControlTcp,SIGNAL(sigPrintInfo(QString)),this,SLOT(slotPrintInfo(QString)));
    connect(_CarControlTcp,SIGNAL(sigChangedText(bool,int,QByteArray,QByteArray,int,int)),
            this,SLOT(slotChangedText(bool,int,QByteArray,QByteArray,int,int)));

    _CarNavigationTcp = new SCStatusTcp(this);
    connect(_CarNavigationTcp,SIGNAL(sigPrintInfo(QString)),this,SLOT(slotPrintInfo(QString)));
    connect(_CarNavigationTcp,SIGNAL(sigChangedText(bool,int,QByteArray,QByteArray,int,int)),
            this,SLOT(slotChangedText(bool,int,QByteArray,QByteArray,int,int)));

    _CarConfigTcp = new SCStatusTcp(this);
    connect(_CarConfigTcp,SIGNAL(sigPrintInfo(QString)),this,SLOT(slotPrintInfo(QString)));
    connect(_CarConfigTcp,SIGNAL(sigChangedText(bool,int,QByteArray,QByteArray,int,int)),
            this,SLOT(slotChangedText(bool,int,QByteArray,QByteArray,int,int)));

    _CarOtherTcp = new SCStatusTcp(this);
    connect(_CarOtherTcp,SIGNAL(sigPrintInfo(QString)),this,SLOT(slotPrintInfo(QString)));
    connect(_CarOtherTcp,SIGNAL(sigChangedText(bool,int,QByteArray,QByteArray,int,int)),
            this,SLOT(slotChangedText(bool,int,QByteArray,QByteArray,int,int)));

    //ip正则.
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    QRegularExpression regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    auto ev = new QRegularExpressionValidator(regExp,ui->lineEdit_ip);
    ui->lineEdit_ip->setValidator(ev);
#else
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    QRegExpValidator *ev = new QRegExpValidator(regExp);
    ui->lineEdit_ip->setValidator(ev);

    QRegExp regExp1("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    QRegExpValidator *ev1 = new QRegExpValidator(regExp1);
    ui->lineEdit_ip_3->setValidator(ev1);
#endif
    //0-65535,已在.ui中约束
    // ui->spinBox_number
    ui->textEdit_info->document()->setMaximumBlockCount(1000);
    on_checkBox_timeOut_clicked(true);

    _carMessage = new CarMessage();
    connect( _carMessage,&CarMessage::SigRevOneMessage,this,&SCTcpToolWidget::SlotRevOneMessage);
    connect(_carMessage,SIGNAL(SigConnectStates(QString
                                                )//注释提示
                               ),this,SLOT(BtnConnectStateUpdata(
                                               QString
                                               )));
    IapSendData1 = new MyCarConnnect();
    IapSendData1->setMode(1);
    IapSendData1->start();
    connect(ui->chb_AutoRwParam,&QCheckBox::stateChanged,this,&SCTcpToolWidget::SlotCarAutoRwPara);
    connect(IapSendData1,SIGNAL(myThreadSignal(
                                    QString
                                    )//注释提示
                                ),this,SLOT(slotPrintThreadInfo(
                                                QString
                                                )));

    connect(IapSendData1,&MyCarConnnect::myThreadIapSend,
            //            SIGNAL(myThreadIapSend(uint8_t ,
            //                                               uint8_t ,
            //                                               QByteArray ,
            //                                               uint16_t
            //                                               )),
            this,
            &SCTcpToolWidget::slotIapThreadSend);
    //            SLOT(slotIapThreadSend(
    //                                               uint8_t ,
    //                                               uint8_t ,
    //                                               QByteArray ,
    //                                               uint16_t
    //                                               )));
    connect(IapSendData1,
            &MyCarConnnect::SigIapSendNum,
            this,
            &SCTcpToolWidget::slotPgb_IAPUp);



    connect(ui->actionNew, &QPushButton::clicked, this, &SCTcpToolWidget::slotActNew);  //新建图
    connect(ui->actionOpen, &QPushButton::clicked, this, &SCTcpToolWidget::slotActOpen);   //打开图
    connect(ui->actionSave, &QPushButton::clicked, this, &SCTcpToolWidget::slotActSave);   //保存图
    connect(ui->actionSaveAs, &QPushButton::clicked, this, &SCTcpToolWidget::slotActSaveAs);//保存为
    connect(ui->actionExImage, &QPushButton::clicked, this, &SCTcpToolWidget::slotActPrintImage); //导出图片
    connect(ui->actionExPDF, &QPushButton::clicked, this, &SCTcpToolWidget::slotActPrintPDF);  //导出pdf

    connect(ui->actionLight, &QPushButton::clicked, this, &SCTcpToolWidget::slotActLight); //明亮模式
    connect(ui->actionDark, &QPushButton::clicked, this, &SCTcpToolWidget::slotActDark);   //暗黑模式

    //调整栅格大小
    connect(ui->action1_pixel, &QPushButton::clicked, this, &SCTcpToolWidget::slotAct1Pixel);
    connect(ui->action2_pixel, &QPushButton::clicked, this, &SCTcpToolWidget::slotAct2Pixel);
    connect(ui->action5_pixel, &QPushButton::clicked, this, &SCTcpToolWidget::slotAct5Pixel);
    connect(ui->action10_pixel, &QPushButton::clicked, this, &SCTcpToolWidget::slotAct10Pixel);

    //创建菜单栏
        QMenuBar* menuBar = new QMenuBar(this);
    //创建菜单（用addMenu方法添加入菜单栏）
        QMenu* filename = menuBar->addMenu(QStringLiteral("文件(&F)"));
    //创建菜单项
        QAction* openfile = new QAction(QStringLiteral("打开文件(&O)"));
        QAction* opendlg = new QAction(QStringLiteral("打开对话框(&D)"));
    //给菜单项添入图标
        openfile->setIcon(QIcon(":/D:/image/Luffy.png"));
        opendlg->setIcon(QIcon(":/D:/image/LuffyQ.png"));
    //用addAction加入菜单项
        filename->addAction(opendlg);
        filename->addAction(openfile);

    itemDockWidgetInit();  //读取样式文件 并设置样式，连接按钮和槽函数
    tabWidgetInit();        //窗口选中被改变是    窗口点击关闭时

}

void SCTcpToolWidget::slotPgb_IAPUp(int cnt,int num)
{
    ui->Pgb_IAP->setMaximum(num);
    ui->Pgb_IAP->setValue(cnt);

}
void SCTcpToolWidget::SlotRevOneMessage(bool flag ,
                                        uint8_t command,  //命令
                                        QByteArray totaldat ,
                                        int length,
                                        int mtime)
{
    IapSendData1->SetOneMessage(flag,command,totaldat,length,mtime);

    ui->textEdit_2->append(QString(totaldat));
    if(totaldat.contains(0x18))
    {
        ui->textEdit_2->append("iap失败");
    }
    if(command == 0xb2)//读取小车得状态
    {
        ui->lab_carcurrentpos->setText(tr("当前卡号：%1").arg(totaldat.at(0 )));
        ui->lab_carspeed->setText(tr("当前速度%1").arg(totaldat.at(1 )));
        ui->lab_carvoltage->setText(tr("电压%1").arg(totaldat.at(2 )));
        ui->lab_carvoltage->setText(tr("电压%1").arg(totaldat.at(3 )));
        ui->lab_rundir->setText(tr("运行方向%1").arg(totaldat.at(4 )));
        switch (totaldat.at(5 )) {
        case 1:
            ui->lab_cartraffc->setText("交管状态：未交管");
            ui->lab_carstates->setText("运行状态：运行中");
            break;
        case 2:
            ui->lab_cartraffc->setText("交管状态：未交管");
            break;
        case 3:
            ui->lab_cartraffc->setText("交管状态：交管中");
            ui->lab_carstates->setText("运行状态：运行中");
            break;
        case 4:
            ui->lab_carstates->setText("运行状态：运行中");
            break;
        default:
            break;
        }

    }
}
void SCTcpToolWidget::BtnConnectStateUpdata(QString str)
{
    ui->btn_connectcarM->setText(str);
}
void SCTcpToolWidget::SlotCarAutoRwPara()
{
    if(ui->chb_AutoRwParam->isChecked() == true)
    {
        IapSendData1->setautoRwParam(1);
    }
    else
    {
        IapSendData1->setautoRwParam(0);
    }
}
SCTcpToolWidget::~SCTcpToolWidget()
{
    if(_queryTimer){
        _queryTimer->deleteLater();
    }
    //_scStatusTcp类不用手动释放，qt会自动释放SCTcpToolWidget的子类.
    delete ui;
}


/** socket连接/断开.
 * @brief SCTcpToolWidget::on_pushButton_connect_clicked
 * @param checked
 */
void SCTcpToolWidget::on_pushButton_connect_clicked()
{
    switch (_scStatusTcp->connectHost(ui->lineEdit_ip->text(),ui->comboBox_port->currentText().toInt())) {
    case 1:
        ui->pushButton_connect->setText(tr("Connect"));
        break;

    default:
        break;
    }
}


//TODO---------------tcp----------------------
/** tcp槽实时监测tcp状态.
 * @brief SCTcpToolWidget::stateChanged
 * @param state
 */
void SCTcpToolWidget::stateChanged1(QAbstractSocket::SocketState state)
{
    QString info;
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        info = "QAbstractSocket::UnconnectedState";
        ui->comboBox_port->setEnabled(true);
        ui->pushButton_connect->setText(tr("Connect"));
        ui->btn_connectcarM->setText(tr("连接小车"));
        break;
    case QAbstractSocket::HostLookupState:
        info = "QAbstractSocket::HostLookupState";
        break;

    case QAbstractSocket::ConnectingState:
        info = "QAbstractSocket::ConnectingState";
        ui->pushButton_connect->setText(tr("Connecting..."));
        ui->btn_connectcarM->setText(tr("正在连接"));
        ui->comboBox_port->setEnabled(false);
        break;
    case QAbstractSocket::ConnectedState:
    {
        info = "QAbstractSocket::ConnectedState \n";
        ui->pushButton_connect->setText(tr("Disconnect"));
        ui->btn_connectcarM->setText(tr("断开小车"));
    }
        break;
    case QAbstractSocket::BoundState:
        info = "QAbstractSocket::BoundState";
        break;
    case QAbstractSocket::ListeningState:
        info = "QAbstractSocket::ListeningState";
        break;
    case QAbstractSocket::ClosingState:
        info = "QAbstractSocket::ClosingState";
        ui->comboBox_port->setEnabled(true);
        ui->pushButton_connect->setText(tr("Connect"));
        ui->btn_connectcarM->setText(tr("连接小车"));
        break;
    }
    ui->textEdit_info->append(QString("%1 IP:%2:%3 %4")
                              .arg(_scStatusTcp->getCurrentDateTime())
                              .arg(ui->lineEdit_ip->text())
                              .arg(ui->comboBox_port->currentText())
                              .arg(info));
}
//TODO---------------tcp----------------------
/** tcp槽实时监测tcp状态.
 * @brief SCTcpToolWidget::stateChanged
 * @param state
 */
void SCTcpToolWidget::stateChanged2(QAbstractSocket::SocketState state)
{
    QString info;
    QString str1 ;

    switch (state) {
    case QAbstractSocket::UnconnectedState: // 连接小车
        info = "QAbstractSocket::UnconnectedState";
        str1 =  "连接小车";
        //emit SigCarConnectStates(str1);
        break;
    case QAbstractSocket::HostLookupState:
        info = "QAbstractSocket::HostLookupState";
        break;

    case QAbstractSocket::ConnectingState:  //正在连接
        info = "QAbstractSocket::ConnectingState";
        str1 =  "正在连接";
        //emit SigCarConnectStates(str1);
        break;
    case QAbstractSocket::ConnectedState://断开小车
    {
        info = "QAbstractSocket::ConnectedState \n";
    }
        break;
    case QAbstractSocket::BoundState:
        info = "QAbstractSocket::BoundState";
        break;
    case QAbstractSocket::ListeningState:
        info = "QAbstractSocket::ListeningState";
        break;
    case QAbstractSocket::ClosingState://连接小车
        str1 =  "连接小车";
        //emit SigCarConnectStates(str1);
        break;
    }
}


/** tcp槽 返回tcp错误.
 * @brief SCTcpToolWidget::receiveTcpError
 * @param error
 */
void SCTcpToolWidget::receiveTcpError(QAbstractSocket::SocketError error)
{
    ui->textEdit_info->append(QString("%1  connect error[%2]: IP:%3:%4")
                              .arg(_scStatusTcp->getCurrentDateTime())
                              .arg(error)
                              .arg(ui->lineEdit_ip->text())
                              .arg(ui->comboBox_port->currentText()));
    ui->comboBox_port->setEnabled(true);
    ui->pushButton_connect->setText(tr("Connect"));
}

/** 发送.
 * @brief SCTcpToolWidget::on_pushButton_send_clicked
 */
void SCTcpToolWidget::on_pushButton_send_clicked()
{
    if(_scStatusTcp->tcpSocket()
            && _scStatusTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        //报头数据类型.
        uint16_t sendCommand = ui->spinBox_sendCommand->value();
        //数据区数据.
        QString sendDataStr = ui->textEdit_sendData->toPlainText();
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_scStatusTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_scStatusTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_scStatusTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}
/** 发送后，响应.
 * @brief SCTcpToolWidget::slotChangedText
 * @param isOk 是否正常返回.
 * @param revCommand 返回的数据类型.
 * @param revData 返回的数据.
 * @param revHex 返回hex.
 * @param number 序号.
 * @param msTime 发送->返回时间 单位：ms.
 */
void SCTcpToolWidget::slotChangedText(bool isOk,
                                      int revCommand,
                                      QByteArray revData,
                                      QByteArray revHex,
                                      int number,
                                      int msTime)
{
    if(isOk){
        int dataSize = 0;
        dataSize = revData.size();
        if(ui->checkBox_revHex->isChecked()){//16进制显示.
            //            dataSize = revHex.size();
            ui->textEdit_revData->setText(_scStatusTcp->hexToQString(revHex));
        }else{//文本显示.
            //            dataSize = revData.size();
            ui->textEdit_revData->setText(QString(revData));
        }
        ui->label_revText->setText(QString("Receive type: %1 (0x%2) \t\n\n"
                                           "Number: %4 (0x%5)\t\n\n"
                                           "Waste time: %6 ms \t\n\n"
                                           "Data Size: %7")
                                   .arg(revCommand)
                                   .arg(QString::number(revCommand,16))
                                   .arg(number)
                                   .arg(QString::number(number,16))
                                   .arg(msTime)
                                   .arg(dataSize));
        //保存到SeerReceive.temp文件.
        if(ui->checkBox_saveFile->isChecked()){
            QFile file("./SeerReceive.temp");
            if(file.open(QIODevice::WriteOnly)){
                file.write(revData);
            }else{
                slotPrintInfo(tr(" <font color=\"red\">Open SeerReceive.temp failed</font> "));
            }
            file.close();
        }
    }else{

        slotPrintInfo(tr(" <font color=\"red\">"
                         "%1--------- Receive error----------\n"
                         "Type:%2  \n"
                         "Error: %3"
                         "</font> ")
                      .arg(_scStatusTcp->getCurrentDateTime())
                      .arg(revCommand)
                      .arg(_scStatusTcp->lastError()));

        ui->textEdit_revData->setText(QString(revData));
        ui->label_revText->setText(QString("Receive error: %1 \t\n")
                                   .arg(_scStatusTcp->lastError()));
    }
}
QByteArray iap_revdat;
/** 发送后，响应.
 * @brief SCTcpToolWidget::slotChangedText
 * @param isOk 是否正常返回.
 * @param revCommand 返回的数据类型.
 * @param revData 返回的数据.
 * @param revHex 返回hex.
 * @param number 序号.
 * @param msTime 发送->返回时间 单位：ms.
 */
void SCTcpToolWidget::slotCarMChangedText(bool isOk,
                                          uint8_t revCommand,
                                          QByteArray revData,
                                          uint16_t number,
                                          int msTime)
{
    if(isOk){
        //        ui->textEdit_2->append(QString("Receive cmd: %1 (0x%2) \t\n\n"
        //                                       "Number: %3 (0x%4)\t\n\n"
        //                                       "revData: (0x%5) \t\n\n"
        //                                       "Waste time: %6 ms \t\n\n")
        //                               .arg(revCommand)
        //                               .arg(QString::number(revCommand,16))
        //                               .arg(number)
        //                               .arg(QString::number(number,16))
        //                               .arg(QString::fromUtf8(revData))
        //                               .arg(msTime));
        //保存到SeerReceive.temp文件.
        if(ui->checkBox_saveFile->isChecked()){
            QFile file("./SeerReceive.temp");
            if(file.open(QIODevice::WriteOnly)){
                file.write(revData);
            }else{
                slotPrintInfo(tr(" <font color=\"red\">Open SeerReceive.temp failed</font> "));
            }
            file.close();
        }
    }else{

        iap_revdat = revData;
        slotPrintInfo(tr(" <font color=\"red\">"
                         "%1--------- Receive error----------\n"
                         "Type:%2  \n"
                         "Error: %3"
                         "</font> ")
                      .arg(_scStatusTcp->getCurrentDateTime())
                      .arg(revCommand)
                      .arg(_scStatusTcp->lastError()));

        ui->textEdit_revData->setText(QString(revData));
        ui->label_revText->setText(QString("Receive error: %1 \t\n")
                                   .arg(_scStatusTcp->lastError()));
    }
}
/** 打印信息.
 * @brief SCTcpToolWidget::slotPrintInfo
 * @param info
 */
void SCTcpToolWidget::slotPrintInfo(QString info)
{
    ui->textEdit_info->append(info);
}

/** 清空textEdit_info数据.
 * @brief SCTcpToolWidget::on_pushButton_clearInfo_clicked
 */
void SCTcpToolWidget::on_pushButton_clearInfo_clicked()
{
    //    if(ui->textEdit_info->document()){
    //        ui->textEdit_info->document()->clear();
    //    }
    ui->textEdit_info->clear();
}

/** 自动滚动.
 * @brief SCTcpToolWidget::slotAutomaticallyScroll
 */
void SCTcpToolWidget::slotAutomaticallyScroll()
{
    if(ui->checkBox_automatically->isChecked()){
        QTextEdit *textedit = (QTextEdit*)sender();
        if(textedit){
            QTextCursor cursor = textedit->textCursor();
            cursor.movePosition(QTextCursor::End);
            textedit->setTextCursor(cursor);
        }
    }
}

void SCTcpToolWidget::on_pushButton_zipFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("File"), ".", tr("File(*.*)"));
    if (filePath.isEmpty()){
        return;
    }
    ui->textEdit_sendData->setText(filePath);
}
//是否开启超时.
void SCTcpToolWidget::on_checkBox_timeOut_clicked(bool checked)
{
    if(checked){
        _scStatusTcp->setTimeOut(ui->spinBox_timeOut->value());
    }else{
        _scStatusTcp->setTimeOut(0);
    }
}

void SCTcpToolWidget::on_checkBox_queryTime_clicked(bool checked)
{
    if(checked){
        if(!_queryTimer){
            _queryTimer = new QTimer(this);
            connect(_queryTimer,&QTimer::timeout,this,[=](){
                on_pushButton_send_clicked();
            });
        }
        _queryTimer->stop();
        _queryTimer->start(ui->spinBox_queryTime->value());
    }else{
        if(_queryTimer && _queryTimer->isActive()){
            _queryTimer->stop();
        }
    }
}

void SCTcpToolWidget::on_pushButton_openFolder_clicked()
{
    QString filePath = QCoreApplication::applicationDirPath();
    QDesktopServices::openUrl(QString("file:///%1").arg(filePath));
}

void SCTcpToolWidget::on_toolButton_en_clicked()
{
    if(_translator){
        qApp->removeTranslator(_translator);
        delete _translator;
        _translator = Q_NULLPTR;
    }
    ui->retranslateUi(this);
}

void SCTcpToolWidget::on_toolButton_ch_clicked()
{
    if(!_translator){
        _translator = new QTranslator(this);
        if(_translator->load(QString(":/resource/Ch.qm")))
            qApp->installTranslator(_translator);
    }
    ui->retranslateUi(this);
}
//选点充电
void SCTcpToolWidget::on_pushButton_2_clicked()
{
    if(_CarNavigationTcp->tcpSocket()
            && _CarNavigationTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        //报头数据类型.
        uint16_t sendCommand = 3051;
        //数据区数据.
        QString sendDataStr = "{\"id\":\"CP3\"}";
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_CarNavigationTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_CarNavigationTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_CarNavigationTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}
//多点导航
void SCTcpToolWidget::on_pushButton_clicked()
{
    if(_CarNavigationTcp->tcpSocket()
            && _CarNavigationTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        QDateTime nowtime = QDateTime::currentDateTime();
        QString  str = nowtime.toString("yyyyMMddhhmmsszzz");
        //报头数据类型.
        uint16_t sendCommand = 3066;
        //数据区数据.
        QString sendDataStr;
        sendDataStr.append("{\"move_task_list\":[");
        if(ui->cbb_1->currentText() != "0")
        {
            sendDataStr.append("{\"source_id\":\"");
            sendDataStr.append(ui->comboBox->currentText());
            sendDataStr.append("\",");
            if(ui->checkBox->isChecked() == true)
            {
                sendDataStr.append("\"spin\":");
                sendDataStr.append("true");
                sendDataStr.append(",");

            }

            sendDataStr.append("\"id\":\"");
            sendDataStr.append(ui->cbb_1->currentText());
            sendDataStr.append("\",");
            sendDataStr.append("\"task_id\":\"");
            sendDataStr.append(str+"1");
            sendDataStr.append("\"}");

        }
        if(ui->cbb_2->currentText() != "0")
        {
            sendDataStr.append(",{\"source_id\":\"");
            sendDataStr.append(ui->cbb_1->currentText());
            sendDataStr.append("\",");



            if(ui->checkBox->isChecked() == true)
            {
                sendDataStr.append("\"spin\":");
                sendDataStr.append("true");
                sendDataStr.append(",");

            }
            sendDataStr.append("\"id\":\"");
            sendDataStr.append(ui->cbb_2->currentText());
            sendDataStr.append("\",");
            sendDataStr.append("\"task_id\":\"");
            sendDataStr.append(str+"2");
            sendDataStr.append("\"}");

        }
        if(ui->cbb_3->currentText() != "0")
        {
            sendDataStr.append(",{\"source_id\":\"");
            sendDataStr.append(ui->cbb_2->currentText());
            sendDataStr.append("\",");
            if(ui->checkBox->isChecked() == true)
            {
                sendDataStr.append("\"spin\":");
                sendDataStr.append("true");
                sendDataStr.append(",");
            }
            sendDataStr.append("\"id\":\"");
            sendDataStr.append(ui->cbb_3->currentText());
            sendDataStr.append("\",");
            sendDataStr.append("\"task_id\":\"");
            sendDataStr.append(str+"3");
            sendDataStr.append("\"}");
        }
        if(ui->cbb_4->currentText() != "0")
        {
            sendDataStr.append(",{\"source_id\":\"");
            sendDataStr.append(ui->cbb_3->currentText());
            sendDataStr.append("\",");
            if(ui->checkBox->isChecked() == true)
            {
                sendDataStr.append("\"spin\":");
                sendDataStr.append("true");
                sendDataStr.append(",");
            }
            sendDataStr.append("\"id\":\"");
            sendDataStr.append(ui->cbb_4->currentText());
            sendDataStr.append("\",");
            sendDataStr.append("\"task_id\":\"");
            sendDataStr.append(str+"4");
            sendDataStr.append("\"}");

        }
        if(ui->cbb_5->currentText() != "0")
        {
            sendDataStr.append(",{\"source_id\":\"");
            sendDataStr.append(ui->cbb_4->currentText());
            sendDataStr.append("\",");

            if(ui->checkBox->isChecked() == true)
            {
                sendDataStr.append("\"spin\":");
                sendDataStr.append("true");
                sendDataStr.append(",");
            }

            sendDataStr.append("\"id\":\"");
            sendDataStr.append(ui->cbb_5->currentText());
            sendDataStr.append("\",");
            sendDataStr.append("\"task_id\":\"");
            sendDataStr.append(str+"5");
            sendDataStr.append("\"}");
        }
        sendDataStr.append("]}");
        ui->textEdit->append(sendDataStr);
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_CarNavigationTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_CarNavigationTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_CarNavigationTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}
//选点导航
void SCTcpToolWidget::on_pushButton_3_clicked()
{
    if(_CarNavigationTcp->tcpSocket()
            && _CarNavigationTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        //报头数据类型.
        uint16_t sendCommand = 3051;
        //数据区数据.
        QString sendDataStr;
        if(ui->comboBox_2->currentText() != "0")
        {
            sendDataStr.append("{\"id\":\"");
            sendDataStr.append(ui->comboBox_2->currentText());
            sendDataStr.append("\"}");
        }

        ui->textEdit->append(sendDataStr);
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_CarNavigationTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_CarNavigationTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_CarNavigationTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}
//举升下降
void SCTcpToolWidget::on_btn_jackup_clicked()
{
    if(_CarOtherTcp->tcpSocket()
            && _CarOtherTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        //报头数据类型.
        uint16_t sendCommand = 6070;
        //数据区数据.
        QString sendDataStr = ui->textEdit_sendData->toPlainText();
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_CarOtherTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_CarOtherTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_CarOtherTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}
//举升下降
void SCTcpToolWidget::on_btn_jackdown_clicked()
{
    if(_CarOtherTcp->tcpSocket()
            && _CarOtherTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        //报头数据类型.
        uint16_t sendCommand = 6071;
        //数据区数据.
        QString sendDataStr = ui->textEdit_sendData->toPlainText();
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_CarOtherTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_CarOtherTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_CarOtherTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}
//确认定位
void SCTcpToolWidget::on_btn_confirmposition_clicked()
{
    if(_CarControlTcp->tcpSocket()
            && _CarControlTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        //报头数据类型.
        uint16_t sendCommand = 2003;
        //数据区数据.
        QString sendDataStr = ui->textEdit_sendData->toPlainText();
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_CarControlTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_CarControlTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_CarControlTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}


//仙工整车测试  tcp连接
void SCTcpToolWidget::on_btn_carconnect_clicked()
{
    _CarStatesTcp->connectHost(ui->lineEdit_ip->text(),19204);
    _CarControlTcp->connectHost(ui->lineEdit_ip->text(),19205);
    _CarNavigationTcp->connectHost(ui->lineEdit_ip->text(),19206);
    _CarConfigTcp->connectHost(ui->lineEdit_ip->text(),19207);
    _CarOtherTcp->connectHost(ui->lineEdit_ip->text(),19210);

}
//取消导航
void SCTcpToolWidget::on_btn_cancelNav_clicked()
{
    if(_CarNavigationTcp->tcpSocket()
            && _CarNavigationTcp->tcpSocket()->state()==QAbstractSocket::ConnectedState)
    {
        //报头数据类型.
        uint16_t sendCommand = 3003;
        //数据区数据.
        QString sendDataStr = ui->textEdit_sendData->toPlainText();
        QByteArray sendData = sendDataStr.toLatin1();
        //Json 区数据
        QByteArray jsonData = ui->textEdit_JSON->toPlainText().toLocal8Bit();
        //发送数据size.
        quint64 sendDataSize = sendData.size();
        //如果数据区是文件直接打开读取发送.
        if(!sendDataStr.isEmpty()){
            QFile file(sendDataStr);
            if(file.exists()){//如果数据区是文件.
                if(file.open(QIODevice::ReadOnly)){
                    sendData = file.readAll();
                    sendDataSize = sendData.size();
                    qDebug()<<"sendData(zip file): size"<<sendDataSize;
                }
                file.close();
            }
        }
        //序号.
        uint16_t number = ui->spinBox_number->value();
        uint8_t byte15 = ui->spinBox_byte15->value();
        //清理接收数据区域.
        ui->textEdit_revData->clear();
        //发送数据.
        if(!_CarNavigationTcp->writeTcpData(sendCommand,jsonData,sendData,number,byte15)){
            slotPrintInfo(tr("<font color=\"red\">"
                             "%1--------- Send error----------\n"
                             "Type:%2  \n"
                             "Error: %3"
                             "</font> ")
                          .arg(_CarNavigationTcp->getCurrentDateTime())
                          .arg(sendCommand)
                          .arg(_CarNavigationTcp->lastError()));
        }
    }else{
        //FIX </font> 后面的空格是一定要的，不然会串色.
        ui->textEdit_info->append(tr(" <font color=\"red\">Unconnected:%1</font> ").arg(ui->lineEdit_ip->text()));
    }
}




void SCTcpToolWidget::on_btn_mapupdate_clicked()
{
    update();
}
void SCTcpToolWidget::DrawMapByJson(int Magnification,QByteArray dat)
{
    // ui->textEdit->append(QString(allData));
    QGraphicsScene  *scene;
    //json文件转换成对象
    QJsonDocument jsonDoc(QJsonDocument::fromJson(dat));
    QJsonObject jsonObject = jsonDoc.object();

    //列出json里的所有key
    QStringList keys = jsonObject.keys();
    for(int i=0;i<keys.size();++i)
        qDebug()<<"key "<<i<<" is "<<keys.at(i);

    //放大倍率
    int mulriple = Magnification;

    //根据key进行相应操作
    if(keys.contains("header"))
    {
        //获取属性信息
        QJsonObject JOB_header = jsonObject["header"].toObject();
        QString mapType = JOB_header["mapType"].toString();
        QString mapName = JOB_header["mapName"].toString();
        QString mapVersion = JOB_header["version"].toString();
        double resolution = JOB_header["resolution"].toDouble();

        //打印map属性
        ui->mapType->setText(QString("mapType: ")+mapType);
        ui->mapName->setText(QString("mapName: ")+mapName);
        ui->mapVers->setText(QString("mapVersion: ")+mapVersion);
        ui->resolution->setText(QString("resolution: %1").arg(resolution));

        //获取场景大小信息
        QJsonObject JOB_minPos = JOB_header["minPos"].toObject();
        QJsonObject JOB_maxPos = JOB_header["maxPos"].toObject();

        qreal scenePosX = JOB_minPos["x"].toDouble()*mulriple;//最小点X值
        qreal scenePosY = -(JOB_minPos["y"].toDouble()*mulriple);   //最小点Y值
        qreal sceneEdgeX = JOB_maxPos["x"].toDouble()*mulriple;     //最大点X
        qreal sceneEdgeY = -(JOB_maxPos["y"].toDouble()*mulriple);  //最大点Y

        qreal sceneWidth = sceneEdgeX - scenePosX;                  //图幅宽度
        qreal sceneHight = sceneEdgeY - scenePosY;                  //图幅高度
        scene = new QGraphicsScene(scenePosX,scenePosY,sceneWidth,sceneHight);  //创建

        //添加场景，并显示矩形框
        ui->graphicsView->setScene(scene);
        scene->addRect(scenePosX,scenePosY,scene->width(),scene->height());
    }

    //显示普通点
    if(keys.contains("normalPosList"))
    {
        //取出所有普通点
        QJsonArray posArray = jsonObject["normalPosList"].toArray();

        //绘制点
        for(auto i : posArray){
            QJsonObject JOB_pos = i.toObject();
            //计算坐标
            qreal x = JOB_pos["x"].toDouble()*mulriple;
            qreal y = JOB_pos["y"].toDouble()*mulriple;
            //根据坐标绘制一个像素点得普通电
            scene->addEllipse(x,-y,1,1);
        }
    }
    //显示高级点
    if(keys.contains("advancedPointList"))
    {
        //去除所有高级点
        QJsonArray APosArray = jsonObject["advancedPointList"].toArray();
        //计算共计有多少个高级点
        int AposCount = APosArray.count();
        ui->LandM->setText(QString("LandMarks Num: %1").arg(AposCount));
        //绘制高级点
        for(auto i : APosArray){
            QJsonObject JOB_Apos = i.toObject();
            QJsonObject JOB_pos = JOB_Apos["pos"].toObject();
            QString Name_pos = JOB_Apos["instanceName"].toString();
            //获取高级点得坐标
            qreal x = JOB_pos["x"].toDouble()*mulriple;
            qreal y = JOB_pos["y"].toDouble()*mulriple;
            //绘制高级点（矩形）
            QGraphicsRectItem *item_Apos = scene->addRect(x,
                                                          -(y),
                                                          20,
                                                          20,
                                                          QPen(QColor(Qt::yellow)),
                                                          QBrush(QColor(Qt::blue)));
            QGraphicsSimpleTextItem *pItem = new QGraphicsSimpleTextItem();
            pItem->setText(Name_pos);
            // 字体
            QFont font = pItem->font();
            font.setPixelSize(10);  // 像素大小
            font.setItalic(false);  // 斜体
            font.setUnderline(false);  // 下划线

            pItem->setFont(font);
            pItem->setBrush(QBrush(QColor(0, 160, 230)));
            pItem->setX(x);
            pItem->setY(-y);
            // 将 item 添加至场景中
            // QGraphicsScene *pScene = new QGraphicsScene();
            scene->addItem(pItem);
            // 为视图设置场景
            //  scene->addSimpleText(Name_pos,"仿宋");

            //将他加入可编辑图元列表里
            //this->items_editable.append(item_Apos);

            //            //若标明方向，则绘制方向
            if(JOB_Apos.keys().contains("dir")){
                qreal angle = JOB_Apos["dir"].toDouble()*180/3.14;
                QVector<QPointF> Tri = {{x,-(y)},{x,-y},{x,-(y)}};
                QGraphicsItem* item_pos_dir = scene->addPolygon(QPolygonF(Tri),QPen(QColor(Qt::red)),QBrush(QColor(Qt::red)));
                //设置旋转中心
                item_pos_dir->setTransformOriginPoint(x,-y);
                item_pos_dir->setRotation(angle);
                item_pos_dir->setParentItem(item_Apos);
            }
            QString className = JOB_Apos["className"].toString();
            QString instanceName = JOB_Apos["instanceName"].toString();

            ui->comboBox_2->addItem(instanceName);
            ui->comboBox->addItem(instanceName);
            ui->cbb_1->addItem(instanceName);
            ui->cbb_2->addItem(instanceName);
            ui->cbb_3->addItem(instanceName);
            ui->cbb_4->addItem(instanceName);
            ui->cbb_5->addItem(instanceName);
        }
    }

    //显示曲线
    if(keys.contains("advancedCurveList"))
    {
        QJsonArray CurArray = jsonObject["advancedCurveList"].toArray();
        for(auto i : CurArray){
            QJsonObject JOB_Cur = i.toObject();

            QJsonObject JOB_Cur_startPos = JOB_Cur["startPos"].toObject();
            QJsonObject JOB_Cur_endPos = JOB_Cur["endPos"].toObject();

            QJsonObject startPos = JOB_Cur_startPos["pos"].toObject();
            qreal x1 = startPos["x"].toDouble()*mulriple;
            qreal y1 = startPos["y"].toDouble()*mulriple;

            QJsonObject endPos = JOB_Cur_endPos["pos"].toObject();
            qreal x2 = endPos["x"].toDouble()*mulriple;
            qreal y2 = endPos["y"].toDouble()*mulriple;

            QJsonObject controlPos1 = JOB_Cur["controlPos1"].toObject();
            qreal cx1 = controlPos1["x"].toDouble()*mulriple;
            qreal cy1 = controlPos1["y"].toDouble()*mulriple;

            QJsonObject controlPos2 = JOB_Cur["controlPos2"].toObject();
            qreal cx2 = controlPos2["x"].toDouble()*mulriple;
            qreal cy2 = controlPos2["y"].toDouble()*mulriple;

            //画贝塞斯曲线
            //绘制贝塞斯曲线路径，初始化输入起始点，然后设置曲线的两个控制点和结束点
            QPainterPath CurPath(QPointF(x1,-y1));
            CurPath.cubicTo(QPointF(cx1,-cy1),QPointF(cx2,-cy2),QPointF(x2,-y2));
            scene->addPath(CurPath,QPen(QColor(Qt::darkGreen)));
        }
    }
}
void SCTcpToolWidget::on_btn_openmap_clicked()
{
    //选择文件 获取文件路径
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Text Files (*.smap)"));
    if (fileName.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No file selected."));
        return;
    }
    //打开文件
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot open file for reading."));
        return;
    }
    //读取文件所有内容
    mapdat = file.readAll();
    file.close();
    DrawMapByJson(magnification,mapdat);

}

//查询路径设置
void SCTcpToolWidget::on_btn_ReadLinpath_clicked()
{
    // 填充数据
    for (int row = 0; row < 100; ++row) {
        for (int column = 0; column < 12; ++column) {
            QStandardItem *item = new QStandardItem(QString("%1,%2").arg(row).arg(column));
            model->setItem(row, column, item);
        }
    }

    // ui->tableView->setColumnWidth(1,100);
    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->show();
    //ui->tableView->resizeColumnToContents();
}

QString getFileNameAndSize(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName(); // 获取文件名
    qint64 fileSize = fileInfo.size(); // 获取文件大小（字节）

    // 返回包含文件名和大小的字符串
    return QString("%1 (%2 bytes)")
            .arg(fileName)
            .arg(fileSize);
}
QString BinPath ;//BIN文件目录
QString BinName ;//BIN文件目录
QString BinSize ;//BIN文件目录
qint64 binlength=0;
void SCTcpToolWidget::on_pushButton_9_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Text Files (*.bin)"));
    if (fileName.isEmpty()) {
        QMessageBox::information(this, tr("Info"), tr("No file selected."));
        return;
    }
    BinPath = fileName;
    ui->lineEdit->setText(fileName);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot open file for reading."));
        return;
    }
    QFileInfo fileInfo(fileName);
    BinName = fileInfo.fileName(); // 获取文件名
    qint64 filesize = fileInfo.size(); // 获取文件大小（字节
    binlength = filesize;
    BinSize= QString::number(filesize);
    ui->lab_binname->setText("文件名称："+BinName);
    ui->lab_binsize->setText(QString("文件大小： %1 bytes")
                             .arg(filesize));
    QByteArray allData = file.readAll();
    file.close();
}
//向小车发送程序升级指令   进入刷写模式
void SCTcpToolWidget::on_btn_SendCmdIap_clicked()
{
    //报头数据类型.
    QFileInfo fileInfo(ui->lineEdit->text());
    QString fileName = fileInfo.fileName(); // 获取文件名
    qint64 fileSize = fileInfo.size(); // 获取文件大小（字节）
    fileSize = 0;
    //报头数据类型.
    int number = 7;
    int sendCommand = 0x79;
    QByteArray sendData(number,0) ;
    sendData[0] = 'I';
    sendData[1] = 'A';
    sendData[2] = 'P';
    sendData[3]= (uint8_t)((fileSize &0xff000000 )>>24);
    sendData[4]= (uint8_t)((fileSize &0x00ff0000 )>>16);
    sendData[5]= (uint8_t)((fileSize &0x0000ff00 )>>8);
    sendData[6]= (uint8_t)(fileSize &0x000000ff);
    _carMessage->SendOneMessage(1,sendCommand, sendData,number);
}


//发送bin文件数据
void SCTcpToolWidget::on_btn_SendDataiap_clicked()
{
    //启动线程
    IapSendData1->setFilePath(BinPath);//传入文件路径
    IapSendData1->setMode(2);
    qDebug()<<"IapSendData is begin";

}

void SCTcpToolWidget::on_btn_cmdcarstop_clicked()
{
    //报头数据类型.
    int number = 2;
    int sendCommand = 0x32;
    QByteArray sendData(number,0) ;
    sendData[0] = 0x01;
    sendData[1] = 0x05;
    _carMessage->SendOneMessage(1,sendCommand, sendData,number);
}

void SCTcpToolWidget::on_btn_cmdcarstartfront_clicked()
{
    //报头数据类型.
    int number = 2;
    int sendCommand = 0x32;
    QByteArray sendData(number,0) ;
    sendData[0] = 0x01;
    sendData[1] = 0x01;

    _carMessage->SendOneMessage(1,sendCommand, sendData,number);
}

void SCTcpToolWidget::on_btn_cmdcarstartback_clicked()
{
    //报头数据类型.

    int number = 2;
    int sendCommand = 0x32;
    QByteArray sendData(number,0) ;

    sendData[0] = 0x01;
    sendData[1] = 0x02;

    _carMessage->SendOneMessage(1,sendCommand, sendData,number);
}
//磁导航连接小车
void SCTcpToolWidget::on_btn_connectcarM_clicked()
{
    _carMessage->ConnectToServer(ui->lineEdit_ip_3->text(),ui->comboBox_port_2->currentText());
}

//发送菜单选项1 开始下载流程
void SCTcpToolWidget::on_btn_SendCmdIAPBegin_clicked()
{
    int number = 1;
    QByteArray sendData(number,0) ;
    sendData[0] = '1';
    _carMessage->SendOneMessage(0,0, sendData,1);
}
//开始运行新程序
void SCTcpToolWidget::on_btn_agvStart_clicked()
{
    int number = 1;
    QByteArray sendData(number,0) ;
    sendData[0] = '3';
    _carMessage->SendOneMessage(0,0, sendData,number);
}

//终止更新
void SCTcpToolWidget::on_btn_about_clicked()
{
    int number = 1;
    QByteArray sendData(number,0) ;
    sendData[0] = 'a';
    _carMessage->SendOneMessage(0,0, sendData,number);
    IapSendData1->setMode(1);
}

void SCTcpToolWidget::on_btn_setES_clicked()
{
    //报头数据类型.
    int number = 1;
    int sendCommand = 0x32;
    QByteArray sendData(number,0) ;
    sendData[0] = 0x03;
    _carMessage->SendOneMessage(1,sendCommand, sendData,number);
}

void SCTcpToolWidget::on_btn_clearError_clicked()
{
    //报头数据类型.
    int number = 1;
    int sendCommand = 0x32;
    QByteArray sendData(number,0) ;
    sendData[0] = 0x04;
    _carMessage->SendOneMessage(1,sendCommand, sendData,number);
}

void SCTcpToolWidget::on_btn_setActionNum_clicked()
{
    //报头数据类型.
    int number = 1;
    int sendCommand = 0x32;
    QByteArray sendData(number,0) ;
    sendData[0] = 0x03;
    _carMessage->SendOneMessage(1,sendCommand,sendData,number);
}

/** 打印信息.
 * @brief SCTcpToolWidget::slotPrintInfo
 * @param info
 */
void SCTcpToolWidget::slotPrintThreadInfo(
        QString str1)
{
    ui->txe_carlog->append(str1);
}

/** 打印信息.
 * @brief SCTcpToolWidget::slotPrintInfo
 * @param info
 */
void SCTcpToolWidget::slotIapThreadSend(int type,
                                        int command,
                                        QByteArray str,
                                        int length)
{
    uint8_t m_type = type;
    uint8_t m_command = command;
    uint16_t m_length = length;
    _carMessage->SendOneMessage(m_type,m_command, str,m_length);

}


void SCTcpToolWidget::on_btn_mapAmplify_clicked()
{
    magnification+=5;
    DrawMapByJson(magnification,mapdat);
}

void SCTcpToolWidget::on_btn_mapReduce_clicked()
{
    magnification-=5;
    DrawMapByJson(magnification,mapdat);
}




//样式初始化
void SCTcpToolWidget::itemDockWidgetInit()
{
//    //读取样式文件
//    QFile file(":/Resources/qss/style.qss");
//    if(!file.open(QFile::ReadOnly)) return;
//    QTextStream textStream(&file);
//    QString stylesheet = textStream.readAll();
//    //设置样式
//    this->setStyleSheet(stylesheet);
//    file.close();

    connect(ui->btnText, &QPushButton::clicked, this, &SCTcpToolWidget::slotBtnText);//文本按钮按下时
    connect(ui->btnLine, &QPushButton::clicked, this, &SCTcpToolWidget::slotBtnLine);//画线按钮按下时
    connect(ui->btnRect, &QPushButton::clicked, this, &SCTcpToolWidget::slotBtnRect);//矩形按钮按下时
    connect(ui->btnEllipse, &QPushButton::clicked, this, &SCTcpToolWidget::slotBtnEllipse);//椭圆按钮按下时
}
//窗口初始化
void SCTcpToolWidget::tabWidgetInit()
{
    ui->tbW_map->setTabsClosable(true);

    connect(ui->tbW_map, &QTabWidget::currentChanged, this, [=](int index)   //选中窗口变化时
    {
        Q_UNUSED(index);
        curView = dynamic_cast<MBaseView *>(ui->tbW_map->currentWidget());//获取当前选中窗口
    });

    connect(ui->tbW_map, &QTabWidget::tabCloseRequested, this, [=](int index){  //关闭窗口时提示
        QMessageBox msgBox;
        msgBox.setText("The document has changed. Whether to save the changes?");//提示信息
        //创建三个按钮 保存 不保存  取消
        QPushButton *save = msgBox.addButton("Save", QMessageBox::ActionRole);
        QPushButton *unsave = msgBox.addButton("Unsave", QMessageBox::ActionRole);
        QPushButton *cancel = msgBox.addButton("Cancel", QMessageBox::ActionRole);
        Q_UNUSED(cancel);
        //弹出窗口
        msgBox.exec();
        //获取被按下按钮
        if(msgBox.clickedButton() == save)
        {
            slotActSave();
            ui->tbW_map->removeTab(index);
        }
        else if (msgBox.clickedButton() == unsave) {
            ui->tbW_map->removeTab(index);
        }
    });


}
//保存时文件得初始化  xml
void SCTcpToolWidget::xmlFileInit(QDomDocument *doc)
{
    /* 创建并添加文档声明 */
    QDomProcessingInstruction  docStatement =
    doc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc->appendChild(docStatement);

    QDomElement root;   /* 根节点 */
    QDomElement header; /* 标题 */
    QDomElement _class; /* 根节点下的class属性 */
    QDomComment comment;/* 注释 */

    root = doc->createElement("root");
    doc->appendChild(root);

    header = doc->createElement("header"); /* 添加窗口标题 */
    header.appendChild(doc->createTextNode(ui->tabWidget->tabText(ui->tabWidget->currentIndex())));
    root.appendChild(header);

    comment = doc->createComment("The following are rect primitives");
    root.appendChild(comment);
    _class = doc->createElementNS("rect", "class");
    root.appendChild(_class);

    comment = doc->createComment("The following are line primitives");
    root.appendChild(comment);
    _class = doc->createElementNS("line", "class");
    root.appendChild(_class);

    comment = doc->createComment("The following are text primitives");
    root.appendChild(comment);
    _class = doc->createElementNS("text", "class");
    root.appendChild(_class);

    comment = doc->createComment("The following are ellipse primitives");
    root.appendChild(comment);
    _class = doc->createElementNS("ellipse", "class");
    root.appendChild(_class);


}

bool SCTcpToolWidget::appendObj2Xml(QDomDocument &doc, const QString &cls, const QDomElement &new_node)
{
    QString nsURL = cls;
    QDomNodeList nodelist = doc.elementsByTagNameNS(nsURL.toLower(), "class");
    QDomNode node = nodelist.at(0);

    QDomElement classElement;
    if(node.isElement())
        classElement = node.toElement();

    if(classElement.appendChild(new_node).isNull())
        return false;
    return true;
}

bool SCTcpToolWidget::xmlIsExist(const QString &name)
{
    QDir dir(defaultXmlPath);
    QStringList nameFilters;
    nameFilters << "*.xml";
    QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Time);
    return files.contains(name);
}
//关闭软件时 提示
void SCTcpToolWidget::closeEvent(QCloseEvent *event)
{
    if(ui->tabWidget->count() == 0)  //没有画图窗口时  直接关闭
    {
        qApp->quit();
        return;
    }

    QMessageBox msgBox;
    msgBox.setIconPixmap(QPixmap(":/Resources/images/close.png"));
    QPushButton *sBtn = msgBox.addButton("Save and Close(S)",QMessageBox::ActionRole);
    QPushButton *yBtn = msgBox.addButton("Close(Y)",QMessageBox::ActionRole);
    QPushButton *nBtn = msgBox.addButton("Cancel(F)",QMessageBox::ActionRole);
    Q_UNUSED(nBtn);
    msgBox.setText("Whether you want to close the current tag or all tags？");

    msgBox.exec();
    if(msgBox.clickedButton() == sBtn)
    {
        slotActSave();
        qApp->quit();
    }
    if(msgBox.clickedButton() == yBtn)
    {
        qApp->quit();
    }
    else {
        event->ignore();
    }

}

void SCTcpToolWidget::savePage(const QString &filename)
{
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "Save failed";
        return;
    }
    QDomDocument domDoc;
    xmlFileInit(&domDoc);

    /* 图元的保存
     * 获取当前Scene中所有的items
     */
    QList<QGraphicsItem *> items = curView->scene()->items();

    int size = items.size();
    MBaseObject *obj = nullptr;

    for (int i = 0; i < size; i++) {
        obj = qgraphicsitem_cast<MBaseObject*>(items[i]);
        QPointF pos = obj->scenePos();

        QString scale = QString::number(obj->scale());
        QString rotation = QString::number(obj->rotation());
        QString penwidth = QString::number(obj->gPen.widthF());

        QDomElement newNode = domDoc.createElement("object");

        QDomElement scaleNode = domDoc.createElement("scale");
        QDomText scaleText = domDoc.createTextNode(scale);
        scaleNode.appendChild(scaleText);

        QDomElement xNode = domDoc.createElement("x");
        QDomText xText = domDoc.createTextNode(QString::number(pos.x()));
        xNode.appendChild(xText);

        QDomElement yNode = domDoc.createElement("y");
        QDomText yText = domDoc.createTextNode(QString::number(pos.y()));
        yNode.appendChild(yText);

        QDomElement rotationNode = domDoc.createElement("rotation");
        QDomText rotationText = domDoc.createTextNode(rotation);
        rotationNode.appendChild(rotationText);

        QDomElement penwidthNode = domDoc.createElement("penwidth");
        QDomText penwidthText = domDoc.createTextNode(penwidth);
        penwidthNode.appendChild(penwidthText);

        newNode.appendChild(xNode);
        newNode.appendChild(yNode);
        newNode.appendChild(scaleNode);
        newNode.appendChild(rotationNode);
        newNode.appendChild(penwidthNode);

        switch (obj->objType()) {
        case MBaseObject::Text:
        {
            MTextObject *textObj = qgraphicsitem_cast<MTextObject*>(obj);
            QDomElement textNode = domDoc.createElement("text");
            QDomText textText = domDoc.createTextNode(textObj->text());
            textNode.appendChild(textText);
            newNode.appendChild(textNode);
        }break;
        case MBaseObject::Line:
        {
            MLineObject *lineObj = qgraphicsitem_cast<MLineObject*>(obj);

            QDomElement sxNode = domDoc.createElement("sx");
            QDomText sxText = domDoc.createTextNode(QString::number(lineObj->startPoint().x()));
            sxNode.appendChild(sxText);

            QDomElement syNode = domDoc.createElement("sy");
            QDomText syText = domDoc.createTextNode(QString::number(lineObj->startPoint().y()));
            syNode.appendChild(syText);

            QDomElement exNode = domDoc.createElement("ex");
            QDomText exText = domDoc.createTextNode(QString::number(lineObj->endPoint().x()));
            exNode.appendChild(exText);

            QDomElement eyNode = domDoc.createElement("ey");
            QDomText eyText = domDoc.createTextNode(QString::number(lineObj->endPoint().y()));
            eyNode.appendChild(eyText);

            newNode.appendChild(sxNode);
            newNode.appendChild(syNode);
            newNode.appendChild(exNode);
            newNode.appendChild(eyNode);
        }break;
        default:break;
        }

        appendObj2Xml(domDoc, obj->nameString(), newNode);
    }

    QTextStream stream(&file);
    domDoc.save(stream, 4);
    file.close();
}
//打开文件时 执行
void SCTcpToolWidget::openPage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open A XML File"
                                                        , "./xml", "Xml(*.xml)");

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Open failed";
        return;
    }

    QXmlStreamReader reader(&file);
    QXmlStreamReader::TokenType token;
    QString element, nsUrl, textStr;
    /* 以下是xml文件中的属性 */
    qreal x, y, penwidth, scale, rotation;
    qreal sx,sy,ex,ey;
    QString text;
    x = 0;
    y = 0;
    penwidth = 0;
    scale = 0;
    rotation = 0;
    sx = 0;
    sy = 0;
    ex = 0;
    ey = 0;

    MBaseObject::OBJTYPE type = MBaseObject::Base; /* class类型 */
    bool bStartDrawObjFlag = false; /* 开始绘制一个object标志 */

    while (!reader.atEnd()) {
        token = reader.readNext();
        if(token == QXmlStreamReader::StartElement)
        {
            element = reader.name().toString();
            if(QString::compare(element, "header") == 0) /* 创建一个View窗口 */
            {
                textStr = reader.readElementText();
                MBaseScene *pScene = new MBaseScene(this);
                pScene->setSceneRect(-297*3/2, -210*3/2, 297*3, 210*3);
                MBaseView *view = new MBaseView(pScene);
                ui->tabWidget->addTab(view, textStr);
                curView = view;
            }
            else if(QString::compare(element, "class") == 0) /* 获取类 */
            {
                nsUrl = reader.namespaceUri().toString();
                if(nsUrl == "rect")
                    type = MBaseObject::Rect;
                else if (nsUrl == "line")
                    type = MBaseObject::Line;
                else if (nsUrl == "text")
                    type = MBaseObject::Text;
                else if (nsUrl == "ellipse")
                    type = MBaseObject::Ellipse;
            }
            else if(QString::compare(element, "object") == 0) /* 获取对象 */
            {
                bStartDrawObjFlag = true;
            }
            else if(QString::compare(element, "x") == 0) /* 对象的x属性 */
            {
                textStr = reader.readElementText();
                x = textStr.toDouble();
            }
            else if(QString::compare(element, "y") == 0) /* 对象的y属性 */
            {
                textStr = reader.readElementText();
                y = textStr.toDouble();
            }
            else if(QString::compare(element, "scale") == 0) /* 对象的scale属性 */
            {
                textStr = reader.readElementText();
                scale = textStr.toDouble();
            }
            else if(QString::compare(element, "rotation") == 0) /* 对象的rotation属性 */
            {
                textStr = reader.readElementText();
                rotation = textStr.toDouble();
            }
            else if(QString::compare(element, "penwidth") == 0) /* 对象的penwidth属性 */
            {
                textStr = reader.readElementText();
                penwidth = textStr.toDouble();
            }
            else if(QString::compare(element, "text") == 0) /* 对象的text属性 */
            {
                textStr = reader.readElementText();
                text = textStr;
            }
            else if(QString::compare(element, "sx") == 0) /* 对象的sx属性 */
            {
                textStr = reader.readElementText();
                sx = textStr.toDouble();
            }
            else if(QString::compare(element, "sy") == 0) /* 对象的sy属性 */
            {
                textStr = reader.readElementText();
                sy = textStr.toDouble();
            }
            else if(QString::compare(element, "ex") == 0) /* 对象的ex属性 */
            {
                textStr = reader.readElementText();
                ex = textStr.toDouble();
            }
            else if(QString::compare(element, "ey") == 0) /* 对象的ey属性 */
            {
                textStr = reader.readElementText();
                ey = textStr.toDouble();
            }
        }
        else if(token == QXmlStreamReader::EndElement)
        {
            if(bStartDrawObjFlag)
            {
                bStartDrawObjFlag = false;
                switch (type) {
                case MBaseObject::Rect:
                {
                    MRectObject *rectObj = new MRectObject();
                    rectObj->setScale(scale);
                    rectObj->setRotation(rotation);
                    rectObj->gPen.setWidthF(penwidth);
                    rectObj->setPos(x, y);
                    curView->scene()->addItem(rectObj);
                }break;
                case MBaseObject::Line:
                {
                    MLineObject *lineObj = new MLineObject();
                    lineObj->setScale(scale);
                    lineObj->setRotation(rotation);
                    lineObj->gPen.setWidthF(penwidth);
                    lineObj->setStartPoint(QPointF(sx, sy));
                    lineObj->setEndPoint(QPointF(ex, ey));
                    lineObj->setPos(x, y);
                    curView->scene()->addItem(lineObj);
                }break;
                case MBaseObject::Text:
                {
                    MTextObject *textObj = new MTextObject();
                    textObj->setScale(scale);
                    textObj->setRotation(rotation);
                    textObj->gPen.setWidthF(penwidth);
                    textObj->setText(text);
                    textObj->setPos(x, y);
                    curView->scene()->addItem(textObj);
                }break;
                case MBaseObject::Ellipse:
                {
                    MEllipseObject *ellipseObj = new MEllipseObject();
                    ellipseObj->setScale(scale);
                    ellipseObj->setRotation(rotation);
                    ellipseObj->gPen.setWidthF(penwidth);
                    ellipseObj->setPos(x, y);
                    curView->scene()->addItem(ellipseObj);
                }break;
                default:break;
                }
            }
        }
    }
}
//添加文本按钮槽函数
void SCTcpToolWidget::slotBtnText()
{
    if(ui->tbW_map->count() == 0) return;
    MTextObject *item = new MTextObject();
    item->setPos(curView->gEndPoint);
    curView->scene()->addItem(item);
}
//添加直线按钮槽函数
void SCTcpToolWidget::slotBtnLine()
{
    if(ui->tbW_map->count() == 0) return;
    curView->enbaleDrawLine(true);
}
//添加矩形按钮槽函数
void SCTcpToolWidget::slotBtnRect()
{
    if(ui->tbW_map->count() == 0) return;
    MBaseObject *item = new MRectObject();
    item->setPos(curView->gEndPoint);
    curView->scene()->addItem(item);
}
//添加椭圆按钮槽函数
void SCTcpToolWidget::slotBtnEllipse()
{
    if(ui->tbW_map->count() == 0) return;
    MBaseObject *item = new MEllipseObject();
    item->setPos(curView->gEndPoint);
    curView->scene()->addItem(item);
}
//新建窗口
void SCTcpToolWidget::slotActNew()
{
    static int count = 0;
    QString text = "untitled_" + QString::number(count);//加后缀序号命名
    count ++;

    MBaseScene *pScene = new MBaseScene(this);   //继承graph得类
    //pScene->setSceneRect(-297*3/2, -210*3/2, 297*3, 210*3);
    pScene->setSceneRect(-(ui->tbW_map->width()/2), -(ui->tbW_map->height()/2), ui->tbW_map->width(), ui->tbW_map->height());
    MBaseView *view = new MBaseView(pScene);
    ui->tbW_map->addTab(view, text);
    ui->tbW_map->setCurrentWidget(view);
    curView = view;//标记当前选中窗口
}
//打开文件
void SCTcpToolWidget::slotActOpen()
{
    openPage();
}
//保存文件
void SCTcpToolWidget::slotActSave()
{
    if(ui->tbW_map->count() == 0) return;
    int index = ui->tbW_map->currentIndex();
    QString name = ui->tbW_map->tabText(index) + ".xml";
    QString pathname = defaultXmlPath + name;
    QString fileName;
    if(!xmlIsExist(name))
    {
        fileName = QFileDialog::getSaveFileName(this, "Save File",
                                                        pathname,"Xml(*.xml)");
        qDebug() << fileName;
        QString title = fileName.split('/').last().split('.').first();
        ui->tbW_map->setTabText(index, title);
    }
    else {
        fileName = pathname;
    }

    savePage(fileName);
}
//保存成其他类型
void SCTcpToolWidget::slotActSaveAs()
{
    if(ui->tbW_map->count() == 0) return;
    int index = ui->tbW_map->currentIndex();
    QString name = ui->tbW_map->tabText(index) + ".xml";
    QString pathname = defaultXmlPath + name;
    QString fileName;

    fileName = QFileDialog::getSaveFileName(this, "Save File",
                                                        pathname,"Xml(*.xml)");

    QString title = fileName.split('/').last().split('.').first();
    ui->tbW_map->setTabText(index, title);

    savePage(fileName);
}
//保存成图片
void SCTcpToolWidget::slotActPrintImage()
{
    if(ui->tbW_map->count() == 0) return;
    int index = ui->tbW_map->currentIndex();
    QString name = ui->tbW_map->tabText(index) + ".png";
    QString pathname = defaultXmlPath + name;
    QString fileName;

    fileName = QFileDialog::getSaveFileName(this,
                                            "Save File",
                                             pathname,
                                            "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;All files (*.*)");



    QPixmap pixmap(qCeil(curView->scene()->sceneRect().width()),
                       qCeil(curView->scene()->sceneRect().height()));
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing
                         | QPainter::HighQualityAntialiasing );//| QPainter::LosslessImageRendering
    curView->scene()->render(&painter);
    painter.end();

    if (fileName == "" && pixmap.isNull())
    {
        qDebug() << "Error!";
        return;
    }

    pixmap.save(fileName);
    QMessageBox::information(this, nullptr, "Export image success");
}
//保存为pdf
void SCTcpToolWidget::slotActPrintPDF()
{
    if(ui->tbW_map->count() == 0) return;
    int index = ui->tbW_map->currentIndex();
    QString name = ui->tbW_map->tabText(index) + ".pdf";
    QString pathname = defaultXmlPath + name;
    QString fileName;

    fileName = QFileDialog::getSaveFileName(this,
                                            "Save File",
                                             pathname,
                                            "PDF (*.pdf);");
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter;
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing
                           | QPainter::HighQualityAntialiasing);// | QPainter::LosslessImageRendering
    if(!painter.begin(&printer))
    {
        qDebug() << "Error!";
        return;
    }
    curView->scene()->render(&painter);
    painter.end();

    QMessageBox::information(this, nullptr, "Export PDF success");

}
//配置明亮背景
void SCTcpToolWidget::slotActLight()
{
     /* Scene的背景色 */
    MBaseScene::gBackgroundColor = QColor("#f2f4f6");
    MBaseScene::gForegroundColorDark = QColor("#ceefe4");
    MBaseScene::gForegroundColorLight = QColor("#f4f0e6");
    /* 图元绘制的颜色 */
    MBaseObject::gObjectColor = Qt::black;
    MBaseObject::gBorderColor = QColor("#260033");

    MBaseView::gXYLineColor = QColor("#FE8402");

}
//设置暗黑背景
void SCTcpToolWidget::slotActDark()
{
    /* Scene的背景色 */
    MBaseScene::gBackgroundColor = QColor("#393939");
    MBaseScene::gForegroundColorDark = QColor("#292929");
    MBaseScene::gForegroundColorLight = QColor("#2f2f2f");
    /* 图元绘制的颜色 */
    MBaseObject::gObjectColor = Qt::white;
    MBaseObject::gBorderColor = QColor(8,255,200);

    MBaseView::gXYLineColor = QColor(0,255,255);
}

/* Grid */
void SCTcpToolWidget::slotAct1Pixel()
{
    MBaseScene::gGridSize = 20;
    MBaseScene::gGridSquares = 5;
    MBaseObject::gMoveOffset = 20;
}

void SCTcpToolWidget::slotAct2Pixel()
{
    MBaseScene::gGridSize = 2;
    MBaseScene::gGridSquares = 5;
    MBaseObject::gMoveOffset = 2;
}

void SCTcpToolWidget::slotAct5Pixel()
{
    MBaseScene::gGridSize = 5;
    MBaseScene::gGridSquares = 5;
    MBaseObject::gMoveOffset = 5;
}

void SCTcpToolWidget::slotAct10Pixel()
{
    MBaseScene::gGridSize = 10;
    MBaseScene::gGridSquares = 5;
    MBaseObject::gMoveOffset = 10;
}


