#include "VirtualCamera.h"

const QString VirtualCamera::VIRTUAL_CAMERA_NAME = "VirtualCamera";
const QString VirtualCamera::VIRTUAL_CAMERA_SERIAL = "Vir123456";
const QString VirtualCamera::VIRTUAL_CAMERA_VENDER = "Virtual";

VirtualCamera::VirtualCamera(const CMCameraMetaInfo& info)
    :CameraInerface(info)
{

}

VirtualCamera::~VirtualCamera()
{

}

// 枚举相机
uint32_t VirtualCamera::enumCamera(QVector<CMCameraMetaInfo>& cameraInfos)
{
    cameraInfos.push_back(CMCameraMetaInfo{VIRTUAL_CAMERA_NAME,VIRTUAL_CAMERA_SERIAL,VIRTUAL_CAMERA_VENDER});
    return CHONGMING_OK;
}

// 获取相机参数列表
uint32_t VirtualCamera::getParamList(QVector<CameraParam>& paramList)
{
    ParseUiJson* parser = ParseUiJson::getInstance();
    parser->loadFromFile(":/resource/VirtualCameraParam.json");
    QList<CameraParamMetaInfo> paramMetaInfoList = parser->getParamList();

    for (auto var : paramMetaInfoList) {
        paramList.push_back(CameraParam(var));
    }

    return CHONGMING_OK;
}

// 判断相机是否连接
bool VirtualCamera::isConnect()
{
    return isConnect_;
}

// 判断相机是否拉流
bool VirtualCamera::isGrabbing()
{
    return isGrabbing_;
}

// 获取相机
uint32_t VirtualCamera::acquire()
{
    return CHONGMING_OK;
}

// 释放相机
uint32_t VirtualCamera::release()
{
    return CHONGMING_OK;
}

// 连接相机
uint32_t VirtualCamera::connect()
{
    isConnect_ = true;
    return CHONGMING_OK;
}

// 断开连接
uint32_t VirtualCamera::disconnect()
{
    isConnect_ = false;
    return CHONGMING_OK;
}

// 创建拉流资源
uint32_t VirtualCamera::creatStream()
{
    return CHONGMING_OK;
}

// 销毁资源
uint32_t VirtualCamera::destroyStream()
{
    return CHONGMING_OK;
}

// 开启拉流
uint32_t VirtualCamera::startGrabbing()
{
    auto CreateImage = [this]()
    {
        while(this->isGrabbing())
        {
            cv::Mat canvas = cv::Mat::zeros(512,512,CV_8UC3);
            cv::RNG rng(time(NULL));
            int b = rng.uniform(0,255);
            int g = rng.uniform(0,255);
            int r = rng.uniform(0,255);
            canvas.setTo(cv::Scalar(b,g,r));
            this->getImageQueue().push(canvas,300);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    };

    isGrabbing_ = true;
    std::thread grabbingThred(CreateImage);
    grabbingThred.detach();
    return CHONGMING_OK;
}

// 停止拉流
uint32_t VirtualCamera::stopGrabbing()
{
    isGrabbing_ = false;
    return CHONGMING_OK;
}

// 加载配置文件
uint32_t VirtualCamera::loadConfig(const QString path)
{
    return CHONGMING_OK;
}

// 保存配置文件
uint32_t VirtualCamera::saveConfig(const QString path)
{
    qDebug("saveConfig");
    return CHONGMING_OK;
}

// 获取配置文件格式
QString VirtualCamera::configFormat()
{
    return "xml";
}

// 读取参数
uint32_t VirtualCamera::readParam(CameraParam& param)
{
    param.setValid(true);
    param.setReadable(true);
    param.setWriteable(true);

    // 读取参数
    if (param.type() == INT) {
        IntParam varParam = param.getValue().value<IntParam>();
        varParam.value = 30;
        varParam.increment = 1;
        varParam.min = 0;
        varParam.max = 100;
        param.setValue(QVariant::fromValue(varParam));
    } else if (param.type() == DOUBLE) {
        DoubleParam varParam = param.getValue().value<DoubleParam>();
        varParam.value = 55.6;
        varParam.min = 0;
        varParam.max = 1000;
        param.setValue(QVariant::fromValue(varParam));
    } else if (param.type() == ENUM) {
        // 获取当前枚举值的条目
        EnumParam varParam = param.getValue().value<EnumParam>();
        varParam.value = "item1";
        varParam.valueInt = 0;
        varParam.availableInt = QVector<int> { 0, 1, 2 };
        varParam.availableValue = QVector<QString> { "item1", "item2", "item3" };
        param.setValue(QVariant::fromValue(varParam));
    } else if (param.type() == BOOL) {
        BoolParam varParam = param.getValue().value<BoolParam>();
        varParam.value = false;
        param.setValue(QVariant::fromValue(varParam));
    } else if (param.type() == CMD) {
        // CMD参数不需要任何读取操作
    } else if (param.type() == STRING) {
        StringParam varParam = param.getValue().value<StringParam>();
        varParam.value = "string_value";
        varParam.nMaxLength = 16;
        param.setValue(QVariant::fromValue(varParam));
    }

    return CHONGMING_OK;
}

// 写入参数
uint32_t VirtualCamera::writeParam(CameraParam& param)
{
    return CHONGMING_OK;
}

// 获取图像
uint32_t VirtualCamera::getImageLast(cv::Mat& image)
{
    cv::Mat srcImage;
    auto ret = getImageQueue().pop(srcImage,500);
    if(ret == POP_IMGAE_FAILED)
    {
        return POP_IMGAE_FAILED;
    }
    srcImage.copyTo(image);
    return CHONGMING_OK;
}
