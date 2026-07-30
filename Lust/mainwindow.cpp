#include "mainwindow.h"

#include <QDateTime>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , controlWidget(new ControlWidget(this))
    , viewWidget(new ViewWidget(controlWidget,this))
    , paramWidget(new ParamWidget(controlWidget,this))
    , loadingDialog(new LoadingDialog(this))
    , tcpClient(new TcpClient(this))
    , responseStreamDecoder(
          new ResponseStreamDecoder(this)
      )
    , responseDispatcher(
          new InferenceResponseDispatcher(this)
      )
    , requestTracker(
          new InferenceRequestTracker(
              InferenceRequestTracker::
                  DEFAULT_TIMEOUT_MS,
              this
          )
      )
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
    /*
     * TCP 没有消息边界。收到的任意字节先交给流解码器，
     * 再把完整协议包交给业务响应分发器。
     */
    connect(
        tcpClient,
        &TcpClient::bytesReceived,
        responseStreamDecoder,
        &ResponseStreamDecoder::appendBytes
    );

    connect(
        responseStreamDecoder,
        &ResponseStreamDecoder::sigResponseDecoded,
        responseDispatcher,
        &InferenceResponseDispatcher::dispatch
    );

    connect(
        responseStreamDecoder,
        &ResponseStreamDecoder::sigProtocolError,
        this,
        [this](const QString& errorMessage)
        {
            statusBar()->showMessage(
                tr("检测服务器协议错误：%1")
                    .arg(errorMessage),
                5000
            );

            /*
             * 协议流已经不可信，主动断开连接，避免继续
             * 按照错误边界解析后续字节。
             */
            tcpClient->disconnectFromServer();
        }
    );

    connect(
        responseStreamDecoder,
        &ResponseStreamDecoder::sigBufferOverflow,
        this,
        [this](qsizetype bufferedBytes)
        {
            statusBar()->showMessage(
                tr("检测响应缓存溢出：%1 bytes")
                    .arg(bufferedBytes),
                5000
            );

            tcpClient->disconnectFromServer();
        }
    );

    connect(
        responseDispatcher,
        &InferenceResponseDispatcher::
            sigInferenceResultReceived,
        this,
        [this](
            quint64 sequence,
            const InferenceResponseCodec::
                InferenceResultPayload& result)
        {
            if(!requestTracker->
                completeRequest(sequence))
            {
                return;
            }

            statusBar()->showMessage(
                tr(
                    "检测结果 #%1：%2，"
                    "Score=%3，耗时=%4 ms，"
                    "缺陷面积=%5 px"
                )
                    .arg(sequence)
                    .arg(result.detectionLevel)
                    .arg(result.score, 0, 'f', 3)
                    .arg(result.inferenceMs)
                    .arg(result.defectArea)
            );
        }
    );

    connect(
        responseDispatcher,
        &InferenceResponseDispatcher::
            sigInferenceErrorReceived,
        this,
        [this](
            quint64 sequence,
            const InferenceResponseCodec::
                InferenceErrorPayload& error)
        {
            /*
             * InferenceError 也是服务端对该 sequence
             * 的正式响应，不能继续等待超时。
             */
            if(!requestTracker->
                completeRequest(sequence))
            {
                return;
            }

            statusBar()->showMessage(
                tr("检测失败 #%1：[%2] %3")
                    .arg(sequence)
                    .arg(error.errorCode)
                    .arg(error.message),
                5000
            );
        }
    );

    connect(
        responseDispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected,
        this,
        [this](
            quint64 sequence,
            const QString& errorMessage)
        {
            /*
             * 业务响应虽然无效，但服务端已经返回了这个
             * sequence，不能继续占用待处理集合。
             */
            if(!requestTracker->
                completeRequest(sequence))
            {
                return;
            }

            statusBar()->showMessage(
                tr("无效检测响应 #%1：%2")
                    .arg(sequence)
                    .arg(errorMessage),
                5000
            );
        }
    );

    connect(
        responseDispatcher,
        &InferenceResponseDispatcher::
            sigStopAcquisitionRequested,
        this,
        [this](quint64 requestSequence)
        {
            pendingStopRequestSequence_ = requestSequence;
            if(viewWidget->isStreamActive())
            {
                statusBar()->showMessage(
                    tr("检测服务器请求停止相机采集"));
                viewWidget->stopAcquisitionFromServer();
                return;
            }

            announceStreamStop();
        }
    );

    connect(
        requestTracker,
        &InferenceRequestTracker::
            sigRequestTimedOut,
        this,
        [this](
            quint64 sequence,
            qint64 elapsedMs)
        {
            statusBar()->showMessage(
                tr("检测请求 #%1 超时（%2 ms）")
                    .arg(sequence)
                    .arg(elapsedMs),
                5000
            );
        }
    );

    connect(
        requestTracker,
        &InferenceRequestTracker::
            sigUnknownResponse,
        this,
        [this](quint64 sequence)
        {
            statusBar()->showMessage(
                tr("收到未知、重复或迟到响应 #%1")
                    .arg(sequence),
                5000
            );
        }
    );

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
    connect(
        viewWidget,
        &ViewWidget::sigPacketReady,
        tcpClient,
        [this](
            quint64 sequence,
            const QByteArray& packet)
        {
            if(requestTracker->contains(sequence))
            {
                statusBar()->showMessage(
                    tr("拒绝重复检测请求 #%1")
                        .arg(sequence),
                    5000
                );
                return;
            }

            if(!requestTracker->canAcceptRequest())
            {
                statusBar()->showMessage(
                    tr(
                        "待处理检测请求已达到上限，"
                        "丢弃帧 #%1"
                    ).arg(sequence),
                    5000
                );
                return;
            }

            if(!tcpClient->sendPacket(packet))
            {
                return;
            }

            /*
             * 只有完整数据包成功进入 Qt 的 TCP
             * 发送缓冲区后才登记为待响应请求。
             */
            if(!requestTracker->
                recordRequest(sequence))
            {
                statusBar()->showMessage(
                    tr("检测请求 #%1 登记失败")
                        .arg(sequence),
                    5000
                );

                /*
                 * 数据已经发送但无法跟踪，继续使用连接
                 * 会导致响应归属不明确。
                 */
                tcpClient->disconnectFromServer();
            }
        }
    );

    connect(viewWidget,&ViewWidget::sigPacketError,this,[this](const QString& errorMessage){
        statusBar()->showMessage(tr("图像编码失败：%1").arg(errorMessage),5000);
    });

    connect(
        viewWidget,
        &ViewWidget::sigStreamStarted,
        this,
        &MainWindow::announceStreamStart
    );

    connect(
        viewWidget,
        &ViewWidget::sigStreamStopped,
        this,
        &MainWindow::announceStreamStop
    );

    connect(tcpClient,&TcpClient::connected,this,[this](){
        controlWidget->setDetectionServerConnected(true);
        if(viewWidget->isStreamActive())
        {
            announceStreamStart();
        }
        viewWidget->setPacketPublishingEnabled(true);
        statusBar()->showMessage(tr("检测服务器已连接"));
    });

    connect(
        tcpClient,
        &TcpClient::disconnected,
        this,
        [this]()
        {
            activeStreamSequence_ = 0;
            pendingStopRequestSequence_ = 0;
            responseStreamDecoder->reset();
            requestTracker->clear();
            controlWidget->
                setDetectionServerConnected(false);
            viewWidget->
                setPacketPublishingEnabled(false);
            statusBar()->showMessage(
                tr("检测服务器已断开")
            );
        }
    );

    connect(
        tcpClient,
        &TcpClient::connectionError,
        this,
        [this](const QString& errorMessage)
        {
            activeStreamSequence_ = 0;
            pendingStopRequestSequence_ = 0;
            responseStreamDecoder->reset();
            requestTracker->clear();
            controlWidget->
                setDetectionServerConnected(false);
            viewWidget->
                setPacketPublishingEnabled(false);
            statusBar()->showMessage(
                tr("检测服务器连接失败：%1")
                    .arg(errorMessage),
                5000
            );
        }
    );

    connect(tcpClient,&TcpClient::sendError,this,[this](const QString& errorMessage){
        statusBar()->showMessage(tr("图像发送失败：%1").arg(errorMessage),5000);
    });
}

bool MainWindow::sendStreamControlPacket(
    FrameProtocol::MessageType messageType,
    quint64 sequence)
{
    if(!tcpClient->isConnected() || sequence == 0)
    {
        return false;
    }

    FrameProtocol::FrameHeader header;
    header.messageType = messageType;
    header.pixelFormat = FrameProtocol::PixelFormat::Unknown;
    header.sequence = sequence;
    header.timestampMs = static_cast<quint64>(
        QDateTime::currentMSecsSinceEpoch());

    const QByteArray packet =
        FrameProtocol::encodePacket(header, {});
    if(packet.isEmpty())
    {
        statusBar()->showMessage(
            tr("流控制消息编码失败"),
            5000);
        return false;
    }
    return tcpClient->sendPacket(packet);
}

void MainWindow::announceStreamStart()
{
    if(!tcpClient->isConnected())
    {
        activeStreamSequence_ = 0;
        return;
    }

    quint64 sequence = nextStreamSequence_++;
    if(sequence == 0)
    {
        sequence = nextStreamSequence_++;
    }

    if(sendStreamControlPacket(
           FrameProtocol::MessageType::StreamStart,
           sequence))
    {
        activeStreamSequence_ = sequence;
        pendingStopRequestSequence_ = 0;
    }
}

void MainWindow::announceStreamStop()
{
    const quint64 sequence =
        activeStreamSequence_ != 0
            ? activeStreamSequence_
            : pendingStopRequestSequence_;

    sendStreamControlPacket(
        FrameProtocol::MessageType::StreamStop,
        sequence);
    activeStreamSequence_ = 0;
    pendingStopRequestSequence_ = 0;
}

void MainWindow::testModel()
{

}


void MainWindow::connectDetectionServer(const QString& host, quint16 port)
{
    responseStreamDecoder->reset();
    requestTracker->clear();
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
