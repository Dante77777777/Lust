#ifndef LISTENER_H
#define LISTENER_H

#include <QMap>
#include <QVector>

enum MESSAGE {
    CAMERA_ENUMRTION = 0x00000001, // 相机枚举
    CAMERA_CONNECT = 0x00000002, // 相机连接
    CAMERA_DISCONNECT = 0x00000004, // 相机断连
    CAMERA_STARTGRAB = 0x00000008, // 相机开启拉流
    CAMERA_STOPTGRAB = 0x00000010, // 相机断开拉流
    CAMERA_CAMERASWICH = 0x00000020 // 相机切换
};

class Listener
{
public:
    Listener() {}
    virtual ~Listener(){}
    virtual void onRespondMessage(int message) = 0;
};

class ListenerManager
{
public:
    using ListenerMap = QMap<int,QVector<Listener*>>;
    static ListenerManager* getInstance();
    void notify(int message);
    void registerMessage(int message,Listener* listener);
private:
    ListenerManager() = default;
    ~ListenerManager() = default;
    ListenerMap listenerMap;
};




























#endif
