#ifndef ACQUIREIMAGEPROCESS_H
#define ACQUIREIMAGEPROCESS_H

#include <QString>
#include <QThread>
#include <QImage>
#include "../CameraInterface/CameraContext.h"

class AcquireImageProcess : public QThread
{
    Q_OBJECT
public:
    AcquireImageProcess();
    ~AcquireImageProcess();

    void setSrial(const QString serial);

signals:
    void sigUpdateImage(const QImage& image);

protected:
    void run() override;
private:
    QString cameraSerial;
};


#endif
