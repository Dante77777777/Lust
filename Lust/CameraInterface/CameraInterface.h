#ifndef CAMERAINTERFACE_H
#define CAMERAINTERFACE_H

#include "CMCameraMetaInfo.h"
#include "CMCameraParam.h"
#include "CameraError.h"
#include "CameraImageQueue.h"
#include "opencv2/opencv.hpp"
#include <QtPlugin>

class CameraInterface
{
public:
    CameraInterface(const CMCameraMetaInfo& info)
    {
        cameraInfo = info;
    }
    virtual ~CameraInterface(){}

    virtual QString UserName()
    {
        return cameraInfo.UserDefineID;
    }

    virtual uint32_t getParamList(QVector<CameraParam>& paramList) = 0;
    // 判断相机是否连接
    virtual bool isConnect() = 0;
    // 判断相机是否拉流
    virtual bool isGrabbing() = 0;
    // 初始化相机对象
    virtual uint32_t acquire() = 0;
    // 释放相机
    virtual uint32_t release() = 0;
    // 连接相机
    virtual uint32_t connect() = 0;
    // 断开连接
    virtual uint32_t disconnect() = 0;
    // 创建拉流资源
    virtual uint32_t creatStream() = 0;
    // 销毁拉流资源
    virtual uint32_t destroyStream() = 0;
    // 开启拉流
    virtual uint32_t startGrabbing() = 0;
    // 停止拉流
    virtual uint32_t stopGrabbing() = 0;
    // 导入配置文件
    virtual uint32_t loadConfig(const QString path) = 0;
    // 导出配置文件
    virtual uint32_t saveConfig(const QString path) = 0;
    // 获取配置文件格式
    virtual QString configFormat() = 0;
    // 读取相机参数
    virtual uint32_t readParam(CameraParam& param) = 0;
    // 写入相机参数
    virtual uint32_t writeParam(CameraParam& param) = 0;
    // 获取实时图像
    virtual uint32_t getImageLast(cv::Mat& image) = 0;
    // 获取图像队列
    virtual CameraImageQueue& getImageQueue()
    {
        return imageQueue;
    }
protected:
    CameraImageQueue imageQueue;
    QVector<CameraParam> cameraParams;
    CMCameraMetaInfo cameraInfo;
};

#endif
