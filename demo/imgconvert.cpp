#include "imgconvert.h"

int qimage2cvmat(QImage::Format qFormat)
{
    switch (qFormat) {
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        return CV_8UC1;
    case QImage::Format_RGB888:
        return CV_8UC3;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
        return CV_8UC4;
    default:
        return -1;
    }
}

cv::Mat qimage2cvmat(const QImage &qImage)
{
    const int cvFormat = qimage2cvmat(qImage.format());
    Q_ASSERT(cvFormat != -1);

    void *imgData = const_cast<uchar*>(qImage.constBits());
    return cv::Mat(qImage.height(), qImage.width(), cvFormat, imgData, qImage.bytesPerLine());
}

QImage::Format cvmat2qimage(int cvFormat)
{
    switch (cvFormat) {
    case CV_8UC1:
        return QImage::Format_Indexed8;
    case CV_8UC3:
        return QImage::Format_RGB888;
    default:
        return QImage::Format_Invalid;
    }
}

QImage cvmat2qimage(const cv::Mat &mat)
{
    const QImage::Format qFormat = cvmat2qimage(mat.type());

    Q_ASSERT(qFormat != QImage::Format_Invalid);

    QImage image(mat.data, mat.cols, mat.rows, (int)mat.elemSize() * mat.cols, qFormat);

    if (qFormat == QImage::Format_Indexed8) {
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; ++i) {
            colorTable.append(qRgb(i, i, i));
        }

        image.setColorTable(colorTable);
    }

    // OpenCV uses BGR chanel order
    if (qFormat == QImage::Format_RGB888) {
        image = image.rgbSwapped();
    }

    return image;
}

