#ifndef CAMERAFACTORY_H
#define CAMERAFACTORY_H

#include "../CameraInterface/CameraInterface.h"
#include "VirtualCamera.h"
#include <QMap>
#include <QMutex>
#include <QString>
#include <QVector>

class CameraFactory
{
public:
    static CameraFactory* getInstance();

    template<typename T>
    void registerCamera(const QString& venderName)
    {
        creatorMap[venderName] = [](const CMCameraMetaInfo& info) -> CameraInerface* {
            return new T(info);
        };
    }

    CameraInerface* createCamera(const CMCameraMetaInfo& info);

    QStringList getSupportedVenders() const;

    bool isVenderSupported(const QString& venderName) const;

private:
    CameraFactory() = default;
    ~CameraFactory() = default;

    CameraFactory(const CameraFactory&) = delete;
    CameraFactory& operator=(const CameraFactory&) = delete;

private:
    using CameraCreator = std::function<CameraInerface*(const CMCameraMetaInfo&)>;
    QMap<QString,CameraCreator> creatorMap;
};

#endif
