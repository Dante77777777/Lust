#include "mainwindow.h"

#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , controlWidget(new ControlWidget(this))
    , viewWidget(new ViewWidget(controlWidget,this))
    , paramWidget(new ParamWidget(controlWidget,this))
    , loadingDialog(new LoadingDialog(this))
    , tcpClient(new TcpClient(this))
{
    setupUI();
    setupNetworkConnections();
}

MainWindow::~MainWindow()
{
    viewWidget->setPacketPublishingEnabled(false);
    tcpClient->disconnectFromServer();
}

void MainWindow::setupUI()
{
    setWindowTitle(tr("Lust Vision"));
    setWindowIcon(QIcon(QStringLiteral(":/resource/camara.png")));
    resize(1650, 900);
    setMinimumSize(1180, 680);

    auto* centralWidget = new QWidget(this);
    centralWidget->setObjectName("CentralWidget");

    auto* centralLayout = new QHBoxLayout(centralWidget);
    centralLayout->setContentsMargins(10, 10, 10, 10);
    centralLayout->setSpacing(0);

    auto* splitter = new QSplitter(Qt::Horizontal, centralWidget);
    splitter->setObjectName("MainSplitter");
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(8);
    splitter->addWidget(controlWidget);
    splitter->addWidget(viewWidget);
    splitter->addWidget(paramWidget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);
    splitter->setSizes({280, 1040, 300});

    controlWidget->setMinimumWidth(240);
    controlWidget->setMaximumWidth(360);
    paramWidget->setMinimumWidth(270);
    paramWidget->setMaximumWidth(400);

    centralLayout->addWidget(splitter);
    setCentralWidget(centralWidget);
    statusBar()->showMessage(tr("就绪"));
}

void MainWindow::setupNetworkConnections()
{
    connect(
        controlWidget,
        &ControlWidget::detectionServerConnectRequested,
        this,
        &MainWindow::connectDetectionServer
    );

    connect(
        controlWidget,
        &ControlWidget::detectionServerDisconnectRequested,
        this,
        &MainWindow::disconnectDetectionServer
    );

    /*
     * 数据包先通过 QueuedConnection 到达主线程的
     * ViewWidget，再在主线程调用 TcpClient。
     */
    connect(viewWidget,&ViewWidget::sigPacketReady,tcpClient,[this](const QByteArray& packet){
        tcpClient->sendPacket(packet);
    });

    connect(viewWidget,&ViewWidget::sigPacketError,this,[this](const QString& errorMessage){
        statusBar()->showMessage(tr("图像编码失败：%1").arg(errorMessage),5000);
    });

    connect(tcpClient,&TcpClient::connected,this,[this](){
        controlWidget->setDetectionServerConnected(true);
        viewWidget->setPacketPublishingEnabled(true);
        statusBar()->showMessage(tr("检测服务器已连接"));
    });

    connect(tcpClient,&TcpClient::disconnected,this,[this](){
        controlWidget->setDetectionServerConnected(false);
        viewWidget->setPacketPublishingEnabled(false);
        statusBar()->showMessage(tr("检测服务器已断开"));
    });

    connect(tcpClient,&TcpClient::connectionError,this,[this](const QString& errorMessage){
        controlWidget->setDetectionServerConnected(false);
        viewWidget->setPacketPublishingEnabled(false);
        statusBar()->showMessage(tr("检测服务器连接失败：%1").arg(errorMessage),5000);
    });

    connect(tcpClient,&TcpClient::sendError,this,[this](const QString& errorMessage){
        statusBar()->showMessage(tr("图像发送失败：%1").arg(errorMessage),5000);
    });
}

void MainWindow::testModel()
{

}


void MainWindow::connectDetectionServer(const QString& host, quint16 port)
{
    viewWidget->setPacketPublishingEnabled(false);
    controlWidget->setDetectionServerBusy(
        true,
        tr("正在连接...")
    );
    statusBar()->showMessage(tr("正在连接检测服务器 %1:%2...").arg(host).arg(port));

    tcpClient->connectToServer(host,port);
}


void MainWindow::disconnectDetectionServer()
{
    viewWidget->setPacketPublishingEnabled(false);
    controlWidget->setDetectionServerBusy(
        true,
        tr("正在断开...")
    );
    tcpClient->disconnectFromServer();
}
