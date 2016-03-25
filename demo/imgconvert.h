#ifndef UTILS_H
#define UTILS_H

#include <QImage>

#include <opencv2/core.hpp>

int qimage2cvmat(QImage::Format qFormat);

cv::Mat qimage2cvmat(const QImage &qImage);

QImage::Format cvmat2qimage(int cvFormat);

QImage cvmat2qimage(const cv::Mat &mat);

#endif // UTILS_H
