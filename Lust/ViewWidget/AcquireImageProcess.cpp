#include "AcquireImageProcess.h"

AcquireImageProcess::AcquireImageProcess()
{

}

AcquireImageProcess::~AcquireImageProcess()
{

}

void AcquireImageProcess::setSrial(const QString serial)
{
    cameraSerial = serial;
}

void AcquireImageProcess::run()
{
    while(true)
    {
        QImage image;
        auto ret = CameraContext::getInstance()->getImageLast(cameraSerial,image);
        if(ret == GETIAMGE_TIMEOUT)
        {
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        emit sigUpdateImage(image);
    }
}
