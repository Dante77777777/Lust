#include "CameraFactory.h"


CameraFactory* CameraFactory::getInstance()
{
    static CameraFactory factory;
    factory.registerCamera<VirtualCamera>(VirtualCamera::VIRTUAL_CAMERA_VENDER);
    return &factory;
}

CameraInerface* CameraFactory::createCamera(const CMCameraMetaInfo& info)
{
    QString venderName = info.VenderName;
    if(!creatorMap.contains(venderName))
    {
        qWarning() << "This vender is not registered";
        return nullptr;
    }
    return creatorMap[venderName](info);
}

QStringList CameraFactory::getSupportedVenders() const
{
    return creatorMap.keys();
}

bool CameraFactory::isVenderSupported(const QString& venderName) const
{
    return creatorMap.contains(venderName);
}
