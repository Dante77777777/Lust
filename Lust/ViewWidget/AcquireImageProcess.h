#ifndef ACQUIREIMAGEPROCESS_H
#define ACQUIREIMAGEPROCESS_H

#include <QString>
#include <QThread>
#include <QImage>
#include <atomic>
#include "../CameraInterface/CameraContext.h"

class AcquireImageProcess : public QThread
{
    Q_OBJECT
public:
    AcquireImageProcess();
    ~AcquireImageProcess();

    void setSerial(const QString serial);
    void requestStop();

signals:
    void sigUpdateImage(const QImage& image);

protected:
    void run() override;
private:
    QString cameraSerial;
    std::atomic<bool> stopRequested = false;
};


#endif
