#ifndef ACQUIREIMAGEPROCESS_H
#define ACQUIREIMAGEPROCESS_H

#include <QString>
#include <QThread>
#include <QImage>
#include <atomic>
#include <QByteArray>
#include <QDateTime>

#include "../Utils/ImageConver.h"
#include "../Protocol/ImagePacketEncoder.h"
#include "../CameraInterface/CameraContext.h"

class AcquireImageProcess : public QThread
{
    Q_OBJECT
public:
    AcquireImageProcess();
    ~AcquireImageProcess() override;

    void setSerial(const QString& serial);
    void requestStop();
    void setPublishingEnabled(bool enabled);
    bool isPacketPublishingEnabled() const noexcept;
signals:
    void sigUpdateImage(const QImage& image);
    void sigPacketReady(const QByteArray& packet);
    void sigPacketError(const QString& errorMessage);
protected:
    void run() override;
private:
    QString cameraSerial;
    std::atomic<bool> stopRequested = false;
    std::atomic<bool> packetPublishingEnabled = false;

    quint64 nextSequence = 0;

    ImagePacketEncoder packetEncoder;
};


#endif
