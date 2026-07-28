#include "AcquireImageProcess.h"

AcquireImageProcess::AcquireImageProcess()
{

}

AcquireImageProcess::~AcquireImageProcess()
{
    requestStop();
    wait();
}

void AcquireImageProcess::setSerial(const QString& serial)
{
    cameraSerial = serial;
    stopRequested = false;
}


void AcquireImageProcess::requestStop()
{
    stopRequested = true;
}


void AcquireImageProcess::run()
{
    while(!stopRequested)
    {
        QImage image;
        const auto ret = CameraContext::getInstance()->getImageLast(cameraSerial,image);

        if(stopRequested)
        {
            break;
        }

        if(ret == GETIMAGE_TIMEOUT)
        {
            continue;
        }

        if(ret != CHONGMING_OK)
        {
            QThread::msleep(10);
            continue;
        }

        if(!image.isNull())
        {
            emit sigUpdateImage(image);
        }


    }
}
