#include <QSignalSpy>
#include <QtTest>

#include "CameraInterface/CameraContext.h"
#include "Protocol/FrameProtocol.h"
#include "ViewWidget/AcquireImageProcess.h"

class AcquireImageProcessTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void publishingDisabledOnlyEmitsDisplayImages();
    void publishingEnabledEmitsOrderedPackets();
    void stopRequestEndsWorker();

private:
    QString serial_;
};

void AcquireImageProcessTest::init()
{
    QVector<CMCameraMetaInfo> cameraInfos;

    QCOMPARE(
        CameraContext::getInstance()
            ->enumerationCamera(cameraInfos),
        static_cast<uint32_t>(CHONGMING_OK)
    );
    QVERIFY(!cameraInfos.isEmpty());

    serial_ = cameraInfos.first().Serial;

    QCOMPARE(
        CameraContext::getInstance()
            ->connect(serial_),
        static_cast<uint32_t>(CHONGMING_OK)
    );
    QCOMPARE(
        CameraContext::getInstance()
            ->startGrabbing(serial_),
        static_cast<uint32_t>(CHONGMING_OK)
    );
}

void AcquireImageProcessTest::cleanup()
{
    CameraContext::getInstance()
        ->stopGrabbing(serial_);

    CameraContext::getInstance()
        ->disconnect(serial_);

    serial_.clear();
}

void AcquireImageProcessTest::
    publishingDisabledOnlyEmitsDisplayImages()
{
    AcquireImageProcess process;
    process.setSerial(serial_);

    QVERIFY(!process.isPacketPublishingEnabled());

    QSignalSpy imageSpy(
        &process,
        &AcquireImageProcess::sigUpdateImage
    );
    QSignalSpy packetSpy(
        &process,
        &AcquireImageProcess::sigPacketReady
    );
    QSignalSpy errorSpy(
        &process,
        &AcquireImageProcess::sigPacketError
    );

    process.start();

    QTRY_VERIFY_WITH_TIMEOUT(
        imageSpy.count() >= 2,
        2500
    );

    process.requestStop();
    QVERIFY(process.wait(1500));

    QVERIFY(imageSpy.count() >= 2);
    QCOMPARE(packetSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
}

void AcquireImageProcessTest::
    publishingEnabledEmitsOrderedPackets()
{
    AcquireImageProcess process;
    process.setSerial(serial_);
    process.setPublishingEnabled(true);

    QVERIFY(process.isPacketPublishingEnabled());

    QSignalSpy imageSpy(
        &process,
        &AcquireImageProcess::sigUpdateImage
    );
    QSignalSpy packetSpy(
        &process,
        &AcquireImageProcess::sigPacketReady
    );
    QSignalSpy errorSpy(
        &process,
        &AcquireImageProcess::sigPacketError
    );

    process.start();

    QTRY_VERIFY_WITH_TIMEOUT(
        packetSpy.count() >= 2,
        3000
    );

    process.requestStop();
    QVERIFY(process.wait(1500));

    QVERIFY(imageSpy.count() >= 2);
    QVERIFY(packetSpy.count() >= 2);
    QCOMPARE(errorSpy.count(), 0);

    for(int index = 0; index < 2; ++index)
    {
        QByteArray receiveBuffer =
            packetSpy.at(index).at(0).toByteArray();

        FrameProtocol::FrameHeader header;
        QByteArray payload;

        QCOMPARE(
            static_cast<int>(
                FrameProtocol::tryDecodePacket(
                    receiveBuffer,
                    header,
                    payload
                )
            ),
            static_cast<int>(
                FrameProtocol::DecodeResult::Complete
            )
        );

        QVERIFY(receiveBuffer.isEmpty());
        QVERIFY(!payload.isEmpty());
        QCOMPARE(
            header.sequence,
            static_cast<quint64>(index)
        );
        QVERIFY(header.timestampMs > 0);
        QCOMPARE(header.width, 512U);
        QCOMPARE(header.height, 512U);
        QCOMPARE(
            static_cast<int>(header.pixelFormat),
            static_cast<int>(
                FrameProtocol::PixelFormat::Jpeg
            )
        );
    }
}

void AcquireImageProcessTest::stopRequestEndsWorker()
{
    AcquireImageProcess process;
    process.setSerial(serial_);
    process.start();

    QTRY_VERIFY_WITH_TIMEOUT(
        process.isRunning(),
        1000
    );

    process.requestStop();

    QVERIFY(process.wait(1500));
    QVERIFY(!process.isRunning());
}

QTEST_GUILESS_MAIN(AcquireImageProcessTest)

#include "AcquireImageProcessTest.moc"
