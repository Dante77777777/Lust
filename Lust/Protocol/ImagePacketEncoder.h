#ifndef IMAGEPACKETENCODER_H
#define IMAGEPACKETENCODER_H

#include <QByteArray>
#include <QString>
#include <QtGlobal>

#include <algorithm>
#include <vector>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/mat.hpp"

#include "FrameProtocol.h"

class ImagePacketEncoder final
{
public:
    explicit ImagePacketEncoder(int jpegQuality = 90);

    QByteArray encode(const cv::Mat& image, quint64 sequence, quint64 timestampMs, QString* errorMessage = nullptr) const;

    int jpegQuality() const noexcept;

private:
    int jpegQuality_ = 90;
};














#endif
