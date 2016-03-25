#include "imagewidget.h"

#include "imgconvert.h"

#include <QDebug>

ImageWidget::ImageWidget(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
}

void ImageWidget::setImage(cv::Mat img)
{
    QImage qimg = cvmat2qimage(img);
    setImage(qimg);
}

void ImageWidget::setImage(const QImage &img)
{
    qimg = img.copy();
}

void ImageWidget::resizeEvent(QResizeEvent *)
{
    updateImage();
}

void ImageWidget::updateImage()
{
    if (qimg.isNull()) {
        return;
    }

    QImage scaled = qimg.scaled(size(), Qt::KeepAspectRatio);
    QLabel::setPixmap(QPixmap::fromImage(scaled));
}
