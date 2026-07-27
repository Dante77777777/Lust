#ifndef CAMERACONTEXT_H
#define CAMERACONTEXT_H

#include "CameraError.h"
#include <QImage>
#include <QMap>
#include <QString>
#include <QVector>
#include <map>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include "CMCameraMetaInfo.h"
#include "CMCameraParam.h"
#include "CameraInterface.h"
#include "CameraImageQueue.h"
#include "../CameraFactory/CameraFactory.h"
#include "../Utils/ImageConver.h"
#include "../CameraFactory/VirtualCamera.h"



// 宏定义，用于检查CameraContext接口的返回值
#define CHECK_RETURN(Ret)                    \
if (Ret != CHONGMING_OK) {                   \
        QString error = getErrorInfoEn(Ret); \
        emit SigUpdateErrorInfo(error);      \
        return;                              \
}

class CameraContext
{
public:
    static CameraContext* getInstance();
    //[1]相机接口组
    // 枚举相机
    uint32_t enumerationCamera(QVector<CMCameraMetaInfo>& cameraInfos);
    // 获取相机参数列表
    uint32_t getParamList(const QString serial, QVector<CameraParam>& paramList);
    // 判断相机是否连接
    uint32_t isConnect(const QString serial, bool& state);
    // 判断相机是否拉流
    uint32_t isGrabbing(const QString serial, bool& state);
    // 连接相机
    uint32_t connect(const QString serial);
    // 断开连接
    uint32_t disconnect(const QString serial);
    // 开启拉流
    uint32_t startGrabbing(const QString serial);
    // 停止拉流
    uint32_t stopGrabbing(const QString serial);
    // 加载配置文件
    uint32_t loadConfig(const QString serial, const QString path);
    // 保存配置文件
    uint32_t saveConfig(const QString serial, const QString path);
    // 获取配置文件
    QString getConfigFormat(const QString serial);
    // 读取参数
    uint32_t readParam(const QString serial, CameraParam& param);
    // 写入参数
    uint32_t writeParam(const QString serial,CameraParam& param);
    // 获取图像
    uint32_t getImageLast(const QString serial,QImage& image);

private:
    CameraContext();
    ~CameraContext();

private:
    QMap<QString, CameraInterface*> serialCamMap;
};


#endif
