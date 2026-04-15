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
    connect(imageProcessThread,&AcquireImageProcess::sigUpdateImage,ui->imageView,&GraphicsView::setImage);

    ListenerManager::getInstance()->registerMessage(MESSAGE::CAMERA_CONNECT
                                                    | MESSAGE::CAMERA_DISCONNECT
                                                    | MESSAGE::CAMERA_ENUMRTION
                                                    | MESSAGE::CAMERA_STARTGRAB
                                                    | MESSAGE::CAMERA_STOPTGRAB,
                                                this);

}

ViewWidget::~ViewWidget()
{
    delete ui;
}

void ViewWidget::onGrabbingButtonClicked()
{
    qDebug("onGrabbingButtonClicked");
    QString serial = controlWidget_->getCurrentCameraInfo().Serial;
    bool state = false;
    CameraContext::getInstance()->isGrabbing(serial,state);
    if(!state)
    {
        connect(imageProcessThread,&AcquireImageProcess::sigUpdateImage,ui->imageView,&GraphicsView::setImage);
        CHECK_RETURN(CameraContext::getInstance()->startGrabbing(serial));
        imageProcessThread->setSrial(serial);
        imageProcessThread->start();
        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_STARTGRAB);
    }
    else
    {
        disconnect(imageProcessThread,&AcquireImageProcess::sigUpdateImage,ui->imageView,&GraphicsView::setImage);
        CHECK_RETURN(CameraContext::getInstance()->stopGrabbing(serial));
        imageProcessThread->quit();
        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_STOPTGRAB);
    }
}

void ViewWidget::onRespondMessage(int message)
{
    if((message & MESSAGE::CAMERA_DISCONNECT) == MESSAGE::CAMERA_DISCONNECT)
    {
        ui->imageView->clear();
        setStartGrabbingButtonState(true);
    }
    else if((message & MESSAGE::CAMERA_ENUMRTION) == MESSAGE::CAMERA_ENUMRTION)
    {
        ui->imageView->clear();
        setStartGrabbingButtonState(true);
    }
    else if((message & MESSAGE::CAMERA_STARTGRAB) == MESSAGE::CAMERA_STARTGRAB)
    {
        setStartGrabbingButtonState(false);
    }
    else if((message & MESSAGE::CAMERA_STOPTGRAB) == MESSAGE::CAMERA_STOPTGRAB)
    {
        setStartGrabbingButtonState(true);
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
