#include "demomainwindow.h"
#include "ui_demomainwindow.h"

#include "markerdetector.h"
#include "imgconvert.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <QTime>
#include <QDebug>

#include <iostream>

using namespace std;
using namespace cv;

DemoMainWindow::DemoMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DemoMainWindow)
{
    ui->setupUi(this);

    QTime t;

    Mat origImg =
//            imread(QT_STRINGIFY(DATADIR)+String("/marker_on_wall_1.jpg"));
            imread(QT_STRINGIFY(DATADIR)+String("/marker_synthetic0.jpg"));

    Mat grayImg;
    Q_ASSERT(MarkerDetector::convertToGray(origImg, grayImg, true));

    MarkerDetector::CameraData camData;
    double fx = 1.f;
    double fy = 1.f;
    double cx = 0.5f;
    double cy = 0.5f;
    camData.cameraMatrix = (cv::Mat_<double>(3, 3) <<
                            fx, 0., cx,
                            0., fy, cy,
                            0., 0., 1.);
    camData.distCoefs = vector<double>{0., 0., 0., 0., 0.};

    MarkerDetector::BlobFinderInternals bfInternals;
    vector<vector<Point2d> > blobCorners;
    t.start();
    MarkerDetector::findBlobCorners(grayImg, camData, blobCorners, bfInternals);
    std::cout << "findMarkerCornes took: " << t.elapsed() << "ms" << std::endl;

    MarkerDetector::MarkerDecoderInternals mdInternals;
    cv::Mat markerContent =
            MarkerDetector::readMarkerContent(blobCorners.back(), grayImg, 5, 2, camData, mdInternals);

    // --------------------------------------------------

    Mat bw_rgb(bfInternals.bwImg.size(), CV_8UC1);
    cvtColor(bfInternals.bwImg, bw_rgb, CV_GRAY2BGR);

    const Scalar colors[] = {
        Scalar(0, 0, 255),
        Scalar(0, 255, 0),
        Scalar(255, 0, 0),
        Scalar(255, 255, 0)
    };
    for (auto corners : blobCorners) {
        for (int i = 0; i < 4; ++i) {
            circle(bw_rgb, corners[i], 4, colors[i], -1);
        }
    }

    cv::Mat probePointsImg = origImg.clone();

    for (auto point : mdInternals.probePoints) {
        const Scalar color(0, 255, 0);
        circle(probePointsImg, point, 4, color, -1);
    }

    ui->origImg->setImage(origImg);
    ui->cornerFinderImg->setImage(bw_rgb);
    ui->probePointsImg->setImage(probePointsImg);
    ui->contentImg->setImage(markerContent);
}

DemoMainWindow::~DemoMainWindow()
{
    delete ui;
}
