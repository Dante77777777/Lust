#include "Listener.h"

ListenerManager* ListenerManager::getInstance()
{
    static ListenerManager instance;
    return &instance;
}

void ListenerManager::notify(int message)
{
    ListenerMap::Iterator it = listenerMap.find(message);
    qDebug("respond");
    if(it != listenerMap.end())
    {
        QVector<Listener*>::Iterator vecIt = it.value().begin();

        while(vecIt != it.value().end())
        {

            (*vecIt)->onRespondMessage(message);
            vecIt++;
        }
    }
    else
    {
        qDebug("This mesaage is not registered or have no listener care");
    }
}

void ListenerManager::registerMessage(int message,Listener* listener)
{

    auto Register = [&](int singleMessage) -> void{
        ListenerMap::Iterator it = listenerMap.find(singleMessage);
        if(it == listenerMap.end())
        {
            QVector<Listener*> listeners;
            listeners.push_back(listener);
            listenerMap[singleMessage] = listeners;
        }
        else
        {
            it.value().push_back(listener);
        }
    };

    if ((message & MESSAGE::CAMERA_ENUMRTION) == MESSAGE::CAMERA_ENUMRTION) {

        Register(MESSAGE::CAMERA_ENUMRTION);
    }
    if ((message & MESSAGE::CAMERA_CONNECT) == MESSAGE::CAMERA_CONNECT) {

        Register(MESSAGE::CAMERA_CONNECT);
    }
    if ((message & MESSAGE::CAMERA_DISCONNECT) == MESSAGE::CAMERA_DISCONNECT) {

        Register(MESSAGE::CAMERA_DISCONNECT);
    }
    if ((message & MESSAGE::CAMERA_STARTGRAB) == MESSAGE::CAMERA_STARTGRAB) {
        Register(MESSAGE::CAMERA_STARTGRAB);
    }
    if ((message & MESSAGE::CAMERA_STOPTGRAB) == MESSAGE::CAMERA_STOPTGRAB) {
        Register(MESSAGE::CAMERA_STOPTGRAB);
    }
}
