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

    QString serial = controlWidget_->getCurrentCameraInfo().Serial;

    bool isGrabbing = false;
    CameraContext::getInstance()->isGrabbing(serial,isGrabbing);

    if(!isGrabbing)
    {
        //connect(imageProcessThread,&AcquireImageProcess::sigUpdateImage,ui->imageView,&GraphicsView::setImage);
        CHECK_RETURN(CameraContext::getInstance()->startGrabbing(serial));
        imageProcessThread->setSerial(serial);
        imageProcessThread->start();
        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_STARTGRAB);
    }
    else
    {
        //disconnect(imageProcessThread,&AcquireImageProcess::sigUpdateImage,ui->imageView,&GraphicsView::setImage);
        stopImageProcess();
        CHECK_RETURN(CameraContext::getInstance()->stopGrabbing(serial));

        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_STOPTGRAB);
    }
}

void ViewWidget::onRespondMessage(int message)
{
    if((message & MESSAGE::CAMERA_DISCONNECT) == MESSAGE::CAMERA_DISCONNECT)
    {
        stopImageProcess();
        ui->imageView->clear();
        setStartGrabbingButtonState(true);
    }
    else if((message & MESSAGE::CAMERA_ENUMRTION) == MESSAGE::CAMERA_ENUMRTION)
    {
        stopImageProcess();
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
