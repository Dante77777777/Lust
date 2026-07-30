#include "viewwidget.h"

ViewWidget::ViewWidget(ControlWidget* controlWidget, QWidget* parent)
    : QWidget(parent)
    , Listener()
    , imageProcessThread(new AcquireImageProcess())
    , controlWidget_(controlWidget)
{
    ui = new ViewWidgetUI();
    ui->setupUI(this);
    connect(ui->grabbingButton,&QPushButton::clicked,this,&ViewWidget::onGrabbingButtonClicked);
    connect(imageProcessThread,&AcquireImageProcess::sigUpdateImage,ui->imageView,&GraphicsView::setImage,Qt::QueuedConnection);
    connect(imageProcessThread,&AcquireImageProcess::sigPacketReady,this,&ViewWidget::sigPacketReady,Qt::QueuedConnection);
    connect(imageProcessThread,&AcquireImageProcess::sigPacketError,this,&ViewWidget::sigPacketError,Qt::QueuedConnection);

    ListenerManager::getInstance()->registerMessage(MESSAGE::CAMERA_CONNECT
                                                    | MESSAGE::CAMERA_DISCONNECT
                                                    | MESSAGE::CAMERA_ENUMRTION
                                                    | MESSAGE::CAMERA_STARTGRAB
                                                    | MESSAGE::CAMERA_STOPTGRAB,
                                                this);

}

ViewWidget::~ViewWidget()
{
    stopImageProcess();
    delete imageProcessThread;
    imageProcessThread = nullptr;
    delete ui;
    ui = nullptr;
}

void ViewWidget::onGrabbingButtonClicked()
{
    qDebug("onGrabbingButtonClicked");

    if(streamActive_)
    {
        stopAcquisition();
    }
    else
    {
        startAcquisition();
    }
}

void ViewWidget::stopAcquisitionFromServer()
{
    stopAcquisition();
}

bool ViewWidget::startAcquisition()
{
    const QString serial =
        controlWidget_->getCurrentCameraInfo().Serial;
    const uint32_t result =
        CameraContext::getInstance()->startGrabbing(serial);
    if(result != CHONGMING_OK)
    {
        emit SigUpdateErrorInfo(getErrorInfoEn(result));
        return false;
    }

    imageProcessThread->setSerial(serial);
    imageProcessThread->start();
    setStreamActive(true);
    ListenerManager::getInstance()->notify(
        MESSAGE::CAMERA_STARTGRAB);
    return true;
}

bool ViewWidget::stopAcquisition()
{
    const QString serial =
        controlWidget_->getCurrentCameraInfo().Serial;

    bool isGrabbing = false;
    const uint32_t queryResult =
        CameraContext::getInstance()->isGrabbing(
            serial,
            isGrabbing);
    if(queryResult != CHONGMING_OK)
    {
        emit SigUpdateErrorInfo(
            getErrorInfoEn(queryResult));
        return false;
    }

    stopImageProcess();
    if(isGrabbing)
    {
        const uint32_t stopResult =
            CameraContext::getInstance()->stopGrabbing(serial);
        if(stopResult != CHONGMING_OK)
        {
            emit SigUpdateErrorInfo(
                getErrorInfoEn(stopResult));
            return false;
        }
    }

    setStreamActive(false);
    ListenerManager::getInstance()->notify(
        MESSAGE::CAMERA_STOPTGRAB);
    return true;
}

void ViewWidget::onRespondMessage(int message)
{
    if((message & MESSAGE::CAMERA_DISCONNECT) == MESSAGE::CAMERA_DISCONNECT)
    {
        stopImageProcess();
        setStreamActive(false);
        ui->imageView->clear();
        setStartGrabbingButtonState(true);
    }
    else if((message & MESSAGE::CAMERA_ENUMRTION) == MESSAGE::CAMERA_ENUMRTION)
    {
        stopImageProcess();
        setStreamActive(false);
        ui->imageView->clear();
        setStartGrabbingButtonState(true);
    }
    else if((message & MESSAGE::CAMERA_STARTGRAB) == MESSAGE::CAMERA_STARTGRAB)
    {
        setStartGrabbingButtonState(false);
    }
    else if((message & MESSAGE::CAMERA_STOPTGRAB) == MESSAGE::CAMERA_STOPTGRAB)
    {
        stopImageProcess();
        setStartGrabbingButtonState(true);
    }
}

void ViewWidget::setPacketPublishingEnabled(bool enabled)
{
    imageProcessThread->setPublishingEnabled(enabled);
}

bool ViewWidget::isStreamActive() const noexcept
{
    return streamActive_;
}

void ViewWidget::setStreamActive(bool active)
{
    if(streamActive_ == active)
    {
        return;
    }

    streamActive_ = active;
    if(active)
    {
        emit sigStreamStarted();
    }
    else
    {
        /*
         * stopImageProcess() 等待采集线程退出时，主线程事件
         * 队列中可能仍有最后几帧 sigPacketReady。把 StreamStop
         * 排在当前已入队帧之后，保证协议中的批次边界不越过
         * 最后一帧。
         */
        QMetaObject::invokeMethod(
            this,
            [this]()
            {
                emit sigStreamStopped();
            },
            Qt::QueuedConnection);
    }
}

void ViewWidget::setStartGrabbingButtonState(bool checked)
{
    if(checked)
    {
        ui->grabbingButton->setText("开始拉流");
    }
    else
    {
        ui->grabbingButton->setText("停止拉流");
    }
}


void ViewWidget::stopImageProcess()
{
    if(!imageProcessThread->isRunning())
    {
        return ;
    }

    imageProcessThread->requestStop();
    imageProcessThread->wait();
}
