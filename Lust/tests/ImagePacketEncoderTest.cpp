#include <vector>

#include <QtTest>

#include <opencv2/imgcodecs.hpp>

#include "Protocol/FrameProtocol.h"
#include "Protocol/ImagePacketEncoder.h"

class ImagePacketEncoderTest : public QObject
{
    Q_OBJECT

private slots:
    void clampsJpegQuality();
    void rejectsEmptyImage();
    void encodesColorImageToProtocolPacket();
    void encodesMonoImage();
    void qualityAffectsPayloadSize();
};

namespace
{

bool decodePacket(
    QByteArray packet,
    FrameProtocol::FrameHeader& header,
    QByteArray& payload
)
{
    return FrameProtocol::tryDecodePacket(
               packet,
               header,
               payload
           )
           == FrameProtocol::DecodeResult::Complete
           && packet.isEmpty();
}

cv::Mat decodeJpeg(const QByteArray& payload)
{
    const std::vector<uchar> jpegBytes(
        payload.cbegin(),
        payload.cend()
    );

    return cv::imdecode(
        jpegBytes,
        cv::IMREAD_UNCHANGED
    );
}

}

void ImagePacketEncoderTest::clampsJpegQuality()
{
    const ImagePacketEncoder tooLow(-10);
    const ImagePacketEncoder tooHigh(200);
    const ImagePacketEncoder valid(75);

    QCOMPARE(tooLow.jpegQuality(), 1);
    QCOMPARE(tooHigh.jpegQuality(), 100);
    QCOMPARE(valid.jpegQuality(), 75);
}

void ImagePacketEncoderTest::rejectsEmptyImage()
{
    const ImagePacketEncoder encoder;
    QString errorMessage;

    const QByteArray packet =
        encoder.encode(
            cv::Mat(),
            1,
            1785211200123ULL,
            &errorMessage
        );

    QVERIFY(packet.isEmpty());
    QVERIFY(!errorMessage.isEmpty());
}

void ImagePacketEncoderTest::
    encodesColorImageToProtocolPacket()
{
    const cv::Mat image(
        48,
        64,
        CV_8UC3,
        cv::Scalar(10, 120, 230)
    );

    const ImagePacketEncoder encoder(90);
    QString errorMessage;

    QByteArray packet =
        encoder.encode(
            image,
            42,
            1785211200123ULL,
            &errorMessage
        );

    QVERIFY(!packet.isEmpty());
    QVERIFY(errorMessage.isEmpty());

    FrameProtocol::FrameHeader header;
    QByteArray payload;

    QVERIFY(decodePacket(packet, header, payload));
    QCOMPARE(
        static_cast<int>(header.messageType),
        static_cast<int>(
            FrameProtocol::MessageType::ImageFrame
        )
    );
    QCOMPARE(
        static_cast<int>(header.pixelFormat),
        static_cast<int>(
            FrameProtocol::PixelFormat::Jpeg
        )
    );
    QCOMPARE(header.sequence, 42ULL);
    QCOMPARE(header.timestampMs, 1785211200123ULL);
    QCOMPARE(header.width, 64U);
    QCOMPARE(header.height, 48U);
    QCOMPARE(
        header.payloadSize,
        static_cast<quint32>(payload.size())
    );

    QVERIFY(payload.size() > 4);
    QCOMPARE(
        static_cast<uchar>(payload[0]),
        static_cast<uchar>(0xFF)
    );
    QCOMPARE(
        static_cast<uchar>(payload[1]),
        static_cast<uchar>(0xD8)
    );

    const cv::Mat decoded = decodeJpeg(payload);

    QVERIFY(!decoded.empty());
    QCOMPARE(decoded.cols, image.cols);
    QCOMPARE(decoded.rows, image.rows);
    QCOMPARE(decoded.channels(), 3);
}

void ImagePacketEncoderTest::encodesMonoImage()
{
    const cv::Mat image(
        32,
        24,
        CV_8UC1,
        cv::Scalar(127)
    );

    const ImagePacketEncoder encoder;
    const QByteArray packet =
        encoder.encode(
            image,
            5,
            1785211200999ULL
        );

    QVERIFY(!packet.isEmpty());

    FrameProtocol::FrameHeader header;
    QByteArray payload;

    QVERIFY(decodePacket(packet, header, payload));

    const cv::Mat decoded = decodeJpeg(payload);

    QVERIFY(!decoded.empty());
    QCOMPARE(decoded.cols, image.cols);
    QCOMPARE(decoded.rows, image.rows);
    QCOMPARE(decoded.channels(), 1);
}

void ImagePacketEncoderTest::qualityAffectsPayloadSize()
{
    cv::Mat noisyImage(256, 256, CV_8UC3);
    cv::RNG random(123456);
    random.fill(
        noisyImage,
        cv::RNG::UNIFORM,
        0,
        256
    );

    const ImagePacketEncoder lowQuality(20);
    const ImagePacketEncoder highQuality(95);

    QByteArray lowPacket =
        lowQuality.encode(
            noisyImage,
            1,
            1785211200001ULL
        );
    QByteArray highPacket =
        highQuality.encode(
            noisyImage,
            2,
            1785211200002ULL
        );

    QVERIFY(!lowPacket.isEmpty());
    QVERIFY(!highPacket.isEmpty());

    FrameProtocol::FrameHeader lowHeader;
    FrameProtocol::FrameHeader highHeader;
    QByteArray lowPayload;
    QByteArray highPayload;

    QVERIFY(
        decodePacket(
            lowPacket,
            lowHeader,
            lowPayload
        )
    );
    QVERIFY(
        decodePacket(
            highPacket,
            highHeader,
            highPayload
        )
    );

    QVERIFY(highPayload.size() > lowPayload.size());
}

QTEST_APPLESS_MAIN(ImagePacketEncoderTest)

#include "ImagePacketEncoderTest.moc"
