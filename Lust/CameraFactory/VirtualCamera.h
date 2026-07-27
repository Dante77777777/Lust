#ifndef VIRTUALCAMERA_H
#define VIRTUALCAMERA_H
#include <chrono>
#include <ctime>
#include <thread>
#include "../CameraInterface/CameraInterface.h"
#include "../ParseUiJson/ParseUiJson.h"

class VirtualCamera : public CameraInterface
{
public:
    static const QString VIRTUAL_CAMERA_NAME;
    static const QString VIRTUAL_CAMERA_SERIAL;
    static const QString VIRTUAL_CAMERA_VENDER;

    explicit VirtualCamera(const CMCameraMetaInfo& info);
    ~VirtualCamera();
    // 枚举相机
    static uint32_t enumCamera(QVector<CMCameraMetaInfo>& cameraInfos);
    // 获取相机参数列表
    uint32_t getParamList(QVector<CameraParam>& paramList) override;
    // 判断相机是否连接
    bool isConnect() override;
    // 判断相机是否拉流
    bool isGrabbing() override;
    // 获取相机
    uint32_t acquire() override;
    // 释放相机
    uint32_t release() override;
    // 连接相机
    uint32_t connect() override;
    // 断开连接
    uint32_t disconnect() override;
    // 创建拉流资源
    uint32_t creatStream() override;
    // 销毁资源
    uint32_t destroyStream() override;
    // 开启拉流
    uint32_t startGrabbing() override;
    // 停止拉流
    uint32_t stopGrabbing() override;
    // 加载配置文件
    uint32_t loadConfig(const QString path) override;
    // 保存配置文件
    uint32_t saveConfig(const QString path) override;
    // 获取配置文件格式
    QString configFormat() override;
    // 读取参数
    uint32_t readParam(CameraParam& param) override;
    // 写入参数
    uint32_t writeParam(CameraParam& param) override;
    // 获取图像
    uint32_t getImageLast(cv::Mat& image) override;
    // 获取图像队列
    CameraImageQueue& getImageQueue() override
    {
        return imageQueue;
    }

private:
    bool isConnect_ = false;
    bool isGrabbing_ = false;
};

#endif
