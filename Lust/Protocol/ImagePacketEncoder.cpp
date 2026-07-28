#include "ImagePacketEncoder.h"

namespace EncoderUtils {

void setEncoderError(QString* errorMessage, const QString& message)
{
    if(errorMessage != nullptr)
    {
        *errorMessage = message;
    }
}

}


ImagePacketEncoder::ImagePacketEncoder(int jpegQuality)
    : jpegQuality_(std::clamp(jpegQuality,1,100))
{

}



QByteArray ImagePacketEncoder::encode(const cv::Mat& image, quint64 sequence, quint64 timestampMs, QString* errorMessage) const
{
    if(errorMessage != nullptr)
    {
        errorMessage->clear();
    }

    if(image.empty())
    {
        EncoderUtils::setEncoderError(errorMessage,QStringLiteral("Image is Empty"));
        return {};
    }

    if(image.cols <= 0 || image.rows <= 0)
    {
        EncoderUtils::setEncoderError(errorMessage,QStringLiteral("Invalid image dimensions"));
        return {};
    }

    std::vector<uchar> encodedImage;

    const std::vector<int> encodeParameters{cv::IMWRITE_JPEG_QUALITY, jpegQuality_};

    try
    {
        const bool encodeSucceeded = cv::imencode(".jpg",image,encodedImage,encodeParameters);
        if(!encodeSucceeded || encodedImage.empty())
        {
            EncoderUtils::setEncoderError(errorMessage,QStringLiteral("JPEG encoding failed"));
            return {};
        }
    }
    catch (const cv::Exception& exception)
    {
        EncoderUtils::setEncoderError(errorMessage,QStringLiteral("JPEG encoding exception: %1").arg(QString::fromUtf8(exception.what())));
        return {};
    }

    if(encodedImage.size() > FrameProtocol::MAX_PAYLOAD_SIZE)
    {
        EncoderUtils::setEncoderError(errorMessage,QStringLiteral("Encoded image is too large"));
        return {};
    }

    const QByteArray payload(reinterpret_cast<const char*>(encodedImage.data()),static_cast<qsizetype>(encodedImage.size()));

    FrameProtocol::FrameHeader header;

    header.messageType = FrameProtocol::MessageType::ImageFrame;

    header.pixelFormat = FrameProtocol::PixelFormat::Jpeg;

    header.sequence = sequence;
    header.timestampMs = timestampMs;

    header.width = static_cast<quint32>(image.cols);

    header.height = static_cast<quint32>(image.rows);

    QByteArray packet =FrameProtocol::encodePacket(header,payload);

    if(packet.isEmpty())
    {
        EncoderUtils::setEncoderError(errorMessage,QStringLiteral("Frame packet encoding failed"));
        return packet;
    }
    return packet;
}

int ImagePacketEncoder::jpegQuality() const noexcept
{
    return jpegQuality_;
}


