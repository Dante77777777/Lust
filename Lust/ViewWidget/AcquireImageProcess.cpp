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
    stopRequested.store(false);
    nextSequence = 0;
}


void AcquireImageProcess::requestStop()
{
    stopRequested = true;
}

void AcquireImageProcess::setPublishingEnabled(bool enabled)
{
    packetPublishingEnabled.store(enabled);
}


bool AcquireImageProcess::isPacketPublishingEnabled() const noexcept
{
    return packetPublishingEnabled.load();
}


void AcquireImageProcess::run()
{
    while(!stopRequested.load())
    {
        cv::Mat frame;
        const auto ret = CameraContext::getInstance()->getImageLastMat(cameraSerial,frame);

        if(stopRequested.load())
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

        if(frame.empty())
        {
            continue;
        }

        /*
         * 当前线程是双队列唯一消费者。
         * 同一张 frame 同时分发给显示和网络编码。
         */

        const QImage displayImage = ImageConver::cvMat2QImage(frame);

        if(!displayImage.isNull())
        {
            emit sigUpdateImage(displayImage);
        }

        if(!packetPublishingEnabled.load())
        {
            continue;
        }

        const quint64 sequence = nextSequence++;

        const qint64 currentTimeStamp = QDateTime::currentMSecsSinceEpoch();

        QString errorMessage;

        const QByteArray packet = packetEncoder.encode(frame,sequence,static_cast<quint64>(currentTimeStamp),&errorMessage);

        if(packet.isEmpty())
        {
            if(errorMessage.isEmpty())
            {
                errorMessage = QStringLiteral("Unknown packet encoding error");
            }
            emit sigPacketError(errorMessage);
            continue;
        }
        emit sigPacketReady(sequence, packet);
    }

}
