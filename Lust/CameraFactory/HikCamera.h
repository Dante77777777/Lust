#ifndef HIKCAMERA_H
#define HIKCAMERA_H


#include "../CameraInterface/CameraInterface.h"
#include "../depends/MvCameraControl.h"
#include "../ParseUiJson/ParseUiJson.h"
#include <QVector>


class HikCamera : public CameraInterface
{
public:
    static const QString VIRTUAL_CAMERA_VENDER;
    HikCamera(const CMCameraMetaInfo& info);
    ~HikCamera();

    // 枚举相机
    static uint32_t EnumCamera(QVector<CMCameraMetaInfo>& cameraInfos);
    // 获取相机参数列表
    uint32_t getParamList(QVector<CameraParam>& paramList) override;
    // 判断相机是否连接
    bool isConnect() override;
    // 判断相机是否拉流
    bool isGrabbing() override;
    // 初始化相机对象
    uint32_t acquire() override;
    // 释放相机
    uint32_t release() override;
    // 连接相机
    uint32_t connect() override;
    // 断开连接
    uint32_t disconnect() override;
    // 创建拉流资源
    uint32_t creatStream() override;
    // 销毁拉流资源
    uint32_t destroyStream() override;
    // 开启拉流
    uint32_t startGrabbing() override;
    // 停止拉流
    uint32_t stopGrabbing() override;
    // 导入配置文件
    uint32_t loadConfig(const QString path) override;
    // 导出配置文件
    uint32_t saveConfig(const QString path) override;
    // 获取配置文件格式
    QString configFormat() override;
    // 读取相机参数
    uint32_t readParam(CameraParam& param) override;
    // 写入相机参数
    uint32_t writeParam(CameraParam& param) override;
    // 获取实时图像
    uint32_t getImageLast(cv::Mat& image) override;

    void* CameraHandle()
    {
        return m_cameraHandle;
    }
private:
    uint32_t getFeatureAccessMode(CameraParam& param);
private:
    void* m_cameraHandle = NULL;
    MV_CC_DEVICE_INFO* m_pDeviceInfo = NULL;
    bool isStartGrabbing = false;
};

#endif
