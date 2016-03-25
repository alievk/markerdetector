#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QLabel>

#include <opencv2/core.hpp>

class ImageWidget
        : public QLabel
{
public:
    explicit ImageWidget(QWidget *parent=0, Qt::WindowFlags f=0);

    void setImage(cv::Mat img);
    void setImage(const QImage &img);

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *) override;

private:
    void updateImage();

    QImage qimg;
};

#endif // IMAGEWIDGET_H
