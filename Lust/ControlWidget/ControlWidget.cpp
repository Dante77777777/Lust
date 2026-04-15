#include "ControlWidget.h"

ControlWidget::ControlWidget(QWidget* parent)
    : QWidget(parent)
    , Listener()
    , lastCameraIndex(-1)
{
    ui = new ControlWidgetUI();
    ui->setupUI(this);

    connect(ui->enumerationButton,&QPushButton::clicked,this,&ControlWidget::onEnumerationClicked);
    connect(ui->connectionButton,&QPushButton::clicked,this,&ControlWidget::onConnectionClicked);
    connect(ui->saveConfigButton,&QPushButton::clicked,this,&ControlWidget::onSaveConfigClicked);
    connect(ui->loadConfigButton,&QPushButton::clicked,this,&ControlWidget::onLoadConfigClicked);
    connect(ui->enumerationCameras,&QListWidget::currentRowChanged,this,&ControlWidget::onCamerasListCurrentRowChanged);

    //ui->connectionButton->setEnabled(false);

    ListenerManager::getInstance()->registerMessage(MESSAGE::CAMERA_CAMERASWICH
                                                    | MESSAGE::CAMERA_CONNECT
                                                    | MESSAGE::CAMERA_DISCONNECT
                                                    | MESSAGE::CAMERA_ENUMRTION
                                                    | MESSAGE::CAMERA_STARTGRAB
                                                    | MESSAGE::CAMERA_STOPTGRAB, this
                                                    );

}

ControlWidget::~ControlWidget()
{
    delete ui;
}

void ControlWidget::onEnumerationClicked()
{
    LoadingDialog::Loading();
    qDebug("onEnumerationClicked");
    QVector<CMCameraMetaInfo> cameraInfos;
    CameraContext::getInstance()->enumerationCamera(cameraInfos);
    ui->enumerationCameras->clear();
    cameraMetaInfos.clear();
    cameraMetaInfos = cameraInfos;
    for(const auto& info : cameraMetaInfos)
    {
        QString name = QString(info.UserDefineID.data());
        QString serial = QString(info.Serial.data());
        ui->enumerationCameras->addItem(name + "(" + serial + ")");
    }
    ListenerManager::getInstance()->notify(MESSAGE::CAMERA_ENUMRTION);
    LoadingDialog::HideLoading();
}

void ControlWidget::onConnectionClicked()
{
    qDebug("onConnectionClicked");
    CMCameraMetaInfo currentCamera = getCurrentCameraInfo();
    QString serial = currentCamera.Serial;
    if(serial.isNull())
    {
        ui->connectionButton->setText("连接相机");
        return ;
    }

    bool state = false;
    CameraContext::getInstance()->isConnect(serial,state);
    if(state)
    {
        CHECK_RETURN(CameraContext::getInstance()->stopGrabbing(serial));
        CHECK_RETURN(CameraContext::getInstance()->disconnect(serial));
        ui->connectionButton->setText("连接相机");
        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_DISCONNECT);
    }
    else
    {
        CHECK_RETURN(CameraContext::getInstance()->connect(serial));
        ui->connectionButton->setText("断开连接");
        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_CONNECT);
    }
}

void ControlWidget::onSaveConfigClicked()
{
    qDebug("onSaveConfigClicked");
    QString serial = getCurrentCameraInfo().Serial;
    QString format = CameraContext::getInstance()->getConfigFormat(serial);
    QString filter = tr("Config files(*.%1)").arg(format);
    QString filepath = QFileDialog::getSaveFileName(this,"Save Config","",filter);

    if(filepath.isEmpty())
    {
        QMessageBox::critical(this,"Error","Filepath is empty!");
        return ;
    }

    CHECK_RETURN(CameraContext::getInstance()->saveConfig(serial,filepath));
}

void ControlWidget::onLoadConfigClicked()
{
    qDebug("onLoadConfigClicked");
    QString serial = getCurrentCameraInfo().Serial;
    QString format = CameraContext::getInstance()->getConfigFormat(serial);
    QString filter = tr("Config files(*.%1)").arg(format);
    QString filepath = QFileDialog::getOpenFileName(this,"Load Config","",filter);
    if(filepath.isEmpty())
    {
        QMessageBox::critical(this,"Error","Filepath is empty!");
        return;
    }
    CHECK_RETURN(CameraContext::getInstance()->loadConfig(serial,filepath));
}

void ControlWidget::onCamerasListCurrentRowChanged(int currentRow)
{
    if(currentRow == -1)
    {
        return ;
    }
    CMCameraMetaInfo lastCamera = getCameraInfo(lastCameraIndex);
    CameraContext::getInstance()->stopGrabbing(lastCamera.Serial);
    CameraContext::getInstance()->disconnect(lastCamera.Serial);

    lastCameraIndex = currentRow;
    CMCameraMetaInfo currentCamera = getCurrentCameraInfo();
    bool state = false;
    CameraContext::getInstance()->isConnect(currentCamera.Serial,state);
    if(state)
    {
        ui->connectionButton->setText("断开连接");
        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_CONNECT);
    }
    else
    {
        ui->connectionButton->setText("连接相机");
        ListenerManager::getInstance()->notify(MESSAGE::CAMERA_DISCONNECT);
    }
}

void ControlWidget::onRespondMessage(int message)
{
    qDebug("onRespondMessage");
}

CMCameraMetaInfo ControlWidget::getCurrentCameraInfo()
{
    int index = ui->enumerationCameras->currentRow();
    if(index == -1 || index >= cameraMetaInfos.size())
    {
        return CMCameraMetaInfo();
    }
    return cameraMetaInfos.at(index);
}

CMCameraMetaInfo ControlWidget::getCameraInfo(int index)
{
    if(index == -1 || index >= cameraMetaInfos.size())
    {
        return CMCameraMetaInfo();
    }
    return cameraMetaInfos.at(index);
}
