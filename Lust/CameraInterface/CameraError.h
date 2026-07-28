#ifndef CAMERAERROR_H
#define CAMERAERROR_H

#include <QString>

namespace CAMERAERROR {
#define CHONGMING_OK 0x0000 // 运行成功
#define CONNECT_ERROR 0X0001 // 连接相机失败
#define DISCONNECT_ERROR 0X0002 // 断开相机失败
#define STARTGRAB_ERROR 0x0003 // 开启拉流失败
#define STOPGRAB_ERROR 0x0004 // 停止拉流失败
#define NOCAMERA_ERROR 0x0005 // 未找到相机
#define INVALID_INPUT 0x0006 // 输入参数无效
#define INVALID_CAMERA_HANDLE 0x0007 // 相机句柄无效
#define WRITE_PARAM_FAILED 0x0008 // 参数设置失败
#define READ_PARAM_FAILED 0x0009 // 参数读取失败
#define CAMERA_NOT_CONNECTED 0x000A // 相机未连接
#define CAMERA_ACQUIRE_FAILED 0x000B // 相机创建失败
#define CAMERA_CONFIG_SAVE_FAILED 0x000C // 相机配置文件导出失败
#define CAMERA_CONFIG_LOAD_FAILED 0x000D // 相机配置文件导入失败
#define GETIMAGE_TIMEOUT 0x000E // 采图超时
#define DEVICE_NOT_ACCESSIBLE 0x000F // 设备被占用不可达
#define PUSH_IMGAE_FAILED 0x0010 // 加入队列失败
#define POP_IMGAE_FAILED 0x0011 //弹出队列失败
}

static QString getErrorInfoEn(unsigned int error)
{
    QString info {};
    switch (error) {
    case CHONGMING_OK:
    {
        info = "Run Success";
        break;
    }
    case CONNECT_ERROR:
    {
        info = "Camera Connect failed";
        break;
    }
    case DISCONNECT_ERROR:
    {
        info = "Camera DisConnect failed";
        break;
    }
    case STARTGRAB_ERROR:
    {
        info = "Camera StartGrabing failed";
        break;
    }
    case STOPGRAB_ERROR:
    {
        info = "Camera StopGrabing failed";
        break;
    }
    case NOCAMERA_ERROR:
    {
        info = "Not Find Any Camera";
        break;
    }
    case INVALID_INPUT:
    {
        info = "Invalid Input Parameter";
        break;
    }
    case INVALID_CAMERA_HANDLE:
    {
        info = "The camera handle is invalid";
        break;
    }
    case WRITE_PARAM_FAILED:
    {
        info = "Write Param failed";
        break;
    }
    case READ_PARAM_FAILED:
    {
        info = "Read Param failed";
        break;
    }
    case CAMERA_NOT_CONNECTED:
    {
        info = "Camera not connected";
        break;
    }
    case CAMERA_ACQUIRE_FAILED:
    {
        info = "Camera creation failure";
        break;
    }
    case CAMERA_CONFIG_SAVE_FAILED:
    {
        info = "Failed to export the camera configuration file";
        break;
    }
    case CAMERA_CONFIG_LOAD_FAILED:
    {
        info = "Failed to import the camera configuration file";
        break;
    }
    case GETIMAGE_TIMEOUT:
    {
        info = "Get Image TimeOut";
        break;
    }
    case DEVICE_NOT_ACCESSIBLE:
    {
        info = "The device is  UnAccessible";
        break;
    }
    case PUSH_IMGAE_FAILED:
    {
        info = "Can not push image into queue";
        break;
    }
    case POP_IMGAE_FAILED:
    {
        info = "Can not pop image into queue";
        break;
    }
    default: {
        info = "Unkonw Error";
        break;
    }
    }
    return info;
}


#endif
