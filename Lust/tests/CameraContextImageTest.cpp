#include <QImage>
#include <QtTest>

#include <opencv2/core.hpp>

#include "CameraInterface/CameraContext.h"

class CameraContextImageTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void invalidSerialClearsOutputs();
    void virtualCameraProvidesRawMat();
    void qImageWrapperStillProvidesImage();

private:
    QString serial_;
};

void CameraContextImageTest::init()
{
    QVector<CMCameraMetaInfo> cameraInfos;

    QCOMPARE(
        CameraContext::getInstance()
            ->enumerationCamera(cameraInfos),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(!cameraInfos.isEmpty());

    serial_ = cameraInfos.first().Serial;
    QVERIFY(!serial_.isEmpty());

    QCOMPARE(
        CameraContext::getInstance()
            ->connect(serial_),
        static_cast<uint32_t>(CHONGMING_OK)
    );
}

void CameraContextImageTest::cleanup()
{
    CameraContext::getInstance()
        ->stopGrabbing(serial_);

    CameraContext::getInstance()
        ->disconnect(serial_);

    serial_.clear();
}

void CameraContextImageTest::invalidSerialClearsOutputs()
{
    cv::Mat mat(
        4,
        4,
        CV_8UC3,
        cv::Scalar(1, 2, 3)
    );

    QCOMPARE(
        CameraContext::getInstance()
            ->getImageLastMat(
                QStringLiteral("missing-camera"),
                mat
            ),
        static_cast<uint32_t>(NOCAMERA_ERROR)
    );
    QVERIFY(mat.empty());

    QImage image(
        4,
        4,
        QImage::Format_RGB888
    );
    image.fill(Qt::red);

    QCOMPARE(
        CameraContext::getInstance()
            ->getImageLast(
                QStringLiteral("missing-camera"),
                image
            ),
        static_cast<uint32_t>(NOCAMERA_ERROR)
    );
    QVERIFY(image.isNull());
}

void CameraContextImageTest::virtualCameraProvidesRawMat()
{
    QCOMPARE(
        CameraContext::getInstance()
            ->startGrabbing(serial_),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    cv::Mat image;

    QCOMPARE(
        CameraContext::getInstance()
            ->getImageLastMat(serial_, image),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(!image.empty());
    QCOMPARE(image.cols, 512);
    QCOMPARE(image.rows, 512);
    QCOMPARE(image.type(), CV_8UC3);
}

void CameraContextImageTest::
    qImageWrapperStillProvidesImage()
{
    QCOMPARE(
        CameraContext::getInstance()
            ->startGrabbing(serial_),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QImage image;

    QCOMPARE(
        CameraContext::getInstance()
            ->getImageLast(serial_, image),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(!image.isNull());
    QCOMPARE(image.width(), 512);
    QCOMPARE(image.height(), 512);
    QCOMPARE(
        image.format(),
        QImage::Format_RGB888
    );
}

QTEST_GUILESS_MAIN(CameraContextImageTest)

#include "CameraContextImageTest.moc"
