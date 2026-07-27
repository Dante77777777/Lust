#include "ParamWidget.h"


ParamWidget::ParamWidget(ControlWidget* controlWidget,QWidget* parent)
    : QWidget(parent)
    , Listener()
    , controlWidget_(controlWidget)
    , ui(new ParamWidgetUI())
{
    setupUI();
    connect(selectionModel,&QItemSelectionModel::selectionChanged,this,&ParamWidget::onUpdateSelecton);
    connect(paramModel,&CameraParamModel::sigValueChanged,this,&ParamWidget::writeCameraParam);
    ListenerManager::getInstance()->registerMessage(MESSAGE::CAMERA_CONNECT
                                                    | MESSAGE::CAMERA_DISCONNECT
                                                    | MESSAGE::CAMERA_ENUMRTION
                                                    | MESSAGE::CAMERA_STARTGRAB
                                                    | MESSAGE::CAMERA_STOPTGRAB
                                                    | MESSAGE::CAMERA_CAMERASWICH,
                                                this);
}

ParamWidget::~ParamWidget()
{
    delete ui;
}

void ParamWidget::setupUI()
{
    ui->setupUI(this);
    QStringList headers = {"Params","Value"};
    paramModel = new CameraParamModel(headers);


    ui->paramTreeView->setModel(paramModel);
    ui->paramTreeView->expandAll();

    selectionModel = ui->paramTreeView->selectionModel();

    paramDelegate = new CameraParamDelegate();
    ui->paramTreeView->setItemDelegate(paramDelegate);

}

void ParamWidget::onRefreshButtonClicked()
{
    qDebug("onRefreshButtonClicked");
    ui->paramTreeView->reset();
    ui->paramTreeView->expandAll();
}

void ParamWidget::onUpdateSelecton(const QItemSelection& selected, const QItemSelection& deselected)
{
    ui->tipTextBrowser->clear();
    QModelIndex index = selected.indexes().first();
    QVariant varValue = ui->paramTreeView->model()->data(index,CameraParamModel::ItemRoles::ParamDescriptionRole);
    QString description = varValue.toString();
    ui->tipTextBrowser->append(description);
}

void ParamWidget::initParamList(QVector<CameraParam> params)
{
    CMCameraMetaInfo currentCamera = controlWidget_->getCurrentCameraInfo();
    QString serial = currentCamera.Serial;
    for(auto& param : params)
    {
        auto ret = CameraContext::getInstance()->readParam(serial,param);
        if(ret != CHONGMING_OK)
        {
            QString error = param.name() + QString(" read failed");
            emit SigUpdateErrorInfo(error);
        }
        paramModel->addCameraParam(param);
    }
}

void ParamWidget::onRespondMessage(int message)
{
    if((message & MESSAGE::CAMERA_ENUMRTION) == MESSAGE::CAMERA_ENUMRTION)
    {
        clearParamWidget();
    }
    else if((message & MESSAGE::CAMERA_CONNECT) == MESSAGE::CAMERA_CONNECT)
    {
        this->setEnabled(true);
        CMCameraMetaInfo currentCamera = controlWidget_->getCurrentCameraInfo();
        QString serial = currentCamera.Serial;
        QVector<CameraParam> params;
        CameraContext::getInstance()->getParamList(serial,params);
        initParamList(params);
        onRefreshButtonClicked();
    }
    else if((message & MESSAGE::CAMERA_DISCONNECT) == MESSAGE::CAMERA_DISCONNECT)
    {
        clearParamWidget();
    }
    else if((message & MESSAGE::CAMERA_STARTGRAB) == MESSAGE::CAMERA_STARTGRAB)
    {
        this->setEnabled(false);
    }
    else if((message & MESSAGE::CAMERA_STOPTGRAB) == MESSAGE::CAMERA_STOPTGRAB)
    {
        this->setEnabled(true);
    }
    else if((message & MESSAGE::CAMERA_CAMERASWICH) == MESSAGE::CAMERA_CAMERASWICH)
    {
        clearParamWidget();
    }
}

void ParamWidget::clearParamWidget()
{
    paramModel->clear();
    ui->paramTreeView->reset();
    ui->tipTextBrowser->clear();
}

void ParamWidget::writeCameraParam(const QModelIndex& index)
{
    QVariant varPram = paramModel->data(index,CameraParamModel::ItemRoles::ParamRole);
    CameraParam param = varPram.value<CameraParam>();

    CMCameraMetaInfo currentCamera = controlWidget_->getCurrentCameraInfo();
    QString serial = currentCamera.Serial;

    CHECK_RETURN(CameraContext::getInstance()->writeParam(serial,param));
}
