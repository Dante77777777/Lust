#include "CameraContext.h"


CameraContext::CameraContext()
{

}

CameraContext* CameraContext::getInstance()
{
    static CameraContext instance;
    return &instance;
}

CameraContext::~CameraContext()
{
    QMap<QString,CameraInterface*>::Iterator it;
    for(it = serialCamMap.begin();it != serialCamMap.end(); ++it)
    {
        it.value()->stopGrabbing();
        it.value()->disconnect();
    }
    serialCamMap.clear();
}

// 枚举相机
uint32_t CameraContext::enumerationCamera(QVector<CMCameraMetaInfo>& cameraInfos)
{
    QMap<QString,CameraInterface*>::Iterator it;
    for(it = serialCamMap.begin(); it != serialCamMap.end(); ++it)
    {
        it.value()->stopGrabbing();
        it.value()->disconnect();
    }
    serialCamMap.clear();

    QVector<CMCameraMetaInfo> infos;
    VirtualCamera::enumCamera(infos);
    //HikCamera::enumCamera(infos);

    for(auto info : infos)
    {
        QVector<CMCameraMetaInfo>::Iterator it = std::find(cameraInfos.begin(),cameraInfos.end(),info);
        if(it == cameraInfos.end())
        {
            cameraInfos.push_back(info);
            CameraInterface* camera = CameraFactory::getInstance()->createCamera(info);
            if(camera)
            {
                serialCamMap[info.Serial] = camera;
                qDebug() << "创建相机成功:" << info.VenderName << info.Serial;
            }
            else
            {
                qWarning() << "创建相机失败，不支持的厂商:" << info.VenderName;
            }
        }
    }
    return CHONGMING_OK;
}

// 获取相机参数列表
uint32_t CameraContext::getParamList(const QString serial, QVector<CameraParam>& paramList)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }

    auto camera = serialCamMap[serial];
    QVector<CameraParam> paramListTemp;
    camera->getParamList(paramListTemp);
    for(auto param : paramListTemp)
    {
        camera->readParam(param);
        paramList.push_back(param);
    }
    return CHONGMING_OK;
}

// 判断相机是否连接
uint32_t CameraContext::isConnect(const QString serial, bool& state)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];
    state = camera->isConnect();
    return CHONGMING_OK;
}

// 判断相机是否拉流
uint32_t CameraContext::isGrabbing(const QString serial, bool& state)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];
    state = camera->isGrabbing();
    return CHONGMING_OK;
}

// 连接相机
uint32_t CameraContext::connect(const QString serial)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }

    auto camera = serialCamMap[serial];
    auto ret = camera->acquire();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    ret = camera->connect();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 断开连接
uint32_t CameraContext::disconnect(const QString serial)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }

    auto camera = serialCamMap[serial];
    auto ret = camera->disconnect();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    ret = camera->release();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 开启拉流
uint32_t CameraContext::startGrabbing(const QString serial)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];

    if(!camera->isConnect())
    {
        return CAMERA_NOT_CONNECTED;
    }
    if(camera->isGrabbing())
    {
        return CHONGMING_OK;
    }
    auto ret = camera->creatStream();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    ret = camera->startGrabbing();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 停止拉流
uint32_t CameraContext::stopGrabbing(const QString serial)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];
    if(camera->isConnect() == false || camera->isGrabbing() == false)
    {
        return CHONGMING_OK;
    }
    auto ret = camera->stopGrabbing();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    ret = camera->destroyStream();
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 加载配置文件
uint32_t CameraContext::loadConfig(const QString serial, const QString path)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];
    if(camera->isConnect() == false || camera->isGrabbing() == true)
    {
        return DISCONNECT_ERROR;
    }
    auto ret = camera->loadConfig(path);
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 保存配置文件
uint32_t CameraContext::saveConfig(const QString serial, const QString path)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];
    if(camera->isConnect() == false || camera->isGrabbing() == true)
    {
        return DISCONNECT_ERROR;
    }
    auto ret = camera->saveConfig(path);
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 获取配置文件
QString CameraContext::getConfigFormat(const QString serial)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return "";
    }
    auto camera = serialCamMap[serial];
    QString format = camera->configFormat();
    return QString(format.data());
}

// 读取参数
uint32_t CameraContext::readParam(const QString serial, CameraParam& param)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];
    auto ret = camera->readParam(param);
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 写入参数
uint32_t CameraContext::writeParam(const QString serial,CameraParam& param)
{
    if(serialCamMap.find(serial) == serialCamMap.end())
    {
        return NOCAMERA_ERROR;
    }
    auto camera = serialCamMap[serial];
    auto ret = camera->writeParam(param);
    if(ret != CHONGMING_OK)
    {
        return ret;
    }
    return CHONGMING_OK;
}

// 获取图像
uint32_t CameraContext::getImageLast(const QString& serial,QImage& image)
{
    image = QImage();
    cv::Mat mat;
    const auto ret = getImageLastMat(serial,mat);

    if(ret != CHONGMING_OK)
    {
        return ret;
    }

    if(mat.empty())
    {
        return GETIMAGE_TIMEOUT;
    }

    image = ImageConver::cvMat2QImage(mat);
    if(image.isNull())
    {
        return GETIMAGE_TIMEOUT;
    }
    return CHONGMING_OK;
}


uint32_t CameraContext::getImageLastMat(const QString& serial, cv::Mat& image)
{
    image.release();
    const auto camaraIterator = serialCamMap.constFind(serial);

    if(camaraIterator == serialCamMap.constEnd())
    {
        return NOCAMERA_ERROR;
    }

    CameraInterface* camera = camaraIterator.value();

    if(camera == nullptr)
    {
        return NOCAMERA_ERROR;
    }
    return camera->getImageLast(image);
}
