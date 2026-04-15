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

    // CameraParamMetaInfo intMeta = {"111", "整数参数", INT, "", "这是一个整数参数测试"};
    // CameraParam intParam(intMeta);
    // IntParam intValue;
    // intValue.value = 100;
    // intValue.min = 0;
    // intValue.max = 255;
    // intValue.increment = 1;
    // intParam.setValue(QVariant::fromValue(intValue));
    // intParam.setValid(true);
    // intParam.setWriteable(true);
    // paramModel->addCameraParam(intParam);

    // CameraParamMetaInfo doubleMeta = {"测试组", "浮点数参数", DOUBLE, "", "这是一个浮点数参数测试"};
    // CameraParam doubleParam(doubleMeta);
    // DoubleParam doubleValue;
    // doubleValue.value = 3.14;
    // doubleValue.min = 0.0;
    // doubleValue.max = 10.0;
    // doubleParam.setValue(QVariant::fromValue(doubleValue));
    // doubleParam.setValid(true);
    // doubleParam.setReadable(true);
    // doubleParam.setWriteable(true);
    // paramModel->addCameraParam(doubleParam);

    // // 测试布尔参数
    // CameraParamMetaInfo boolMeta = {"111", "布尔参数", BOOL, "", "这是一个布尔参数测试"};
    // CameraParam boolParam(boolMeta);
    // BoolParam boolValue;
    // boolValue.value = true;
    // boolParam.setValue(QVariant::fromValue(boolValue));
    // boolParam.setValid(true);
    // boolParam.setReadable(true);
    // boolParam.setWriteable(true);
    // paramModel->addCameraParam(boolParam);

    // // 测试字符串参数
    // CameraParamMetaInfo stringMeta = {"测试组", "字符串参数", STRING, "", "这是一个字符串参数测试"};
    // CameraParam stringParam(stringMeta);
    // StringParam stringValue;
    // stringValue.value = "测试字符串";
    // stringValue.nMaxLength = 50;
    // stringParam.setValue(QVariant::fromValue(stringValue));
    // stringParam.setValid(true);
    // stringParam.setReadable(true);
    // stringParam.setWriteable(true);
    // paramModel->addCameraParam(stringParam);

    // // 测试枚举参数
    // CameraParamMetaInfo enumMeta = {"111", "枚举参数", ENUM, "", "这是一个枚举参数测试"};
    // CameraParam enumParam(enumMeta);
    // EnumParam enumValue;
    // enumValue.value = "选项2";
    // enumValue.availableValue = {"选项1", "选项2", "选项3"};
    // enumValue.valueInt = 1;
    // enumValue.availableInt = {0, 1, 2};
    // enumParam.setValue(QVariant::fromValue(enumValue));
    // enumParam.setValid(true);
    // enumParam.setReadable(true);
    // enumParam.setWriteable(true);
    // paramModel->addCameraParam(enumParam);

    // // 测试命令参数
    // CameraParamMetaInfo cmdMeta = {"测试组", "命令参数", CMD, "", "这是一个命令参数测试"};
    // CameraParam cmdParam(cmdMeta);
    // CmdParam cmdValue;
    // cmdParam.setValue(QVariant::fromValue(cmdValue));
    // cmdParam.setValid(true);
    // cmdParam.setReadable(true);
    // cmdParam.setWriteable(true);
    // paramModel->addCameraParam(cmdParam);

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
