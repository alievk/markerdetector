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
            imread(QT_STRINGIFY(DATADIR)+String("/marker_synthetic.jpg"));

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

//    for (auto contour : interm.edges[0]) {
//        static int colorChanger {0};
//        const int thickness = 2;
//        const Scalar color(((colorChanger+1)*50)%255,
//                           ((colorChanger+2)*50)%255,
//                           ((colorChanger+5)*50)%255);
//        for (int i = 0; i < contour.size(); ++i) {
//            Point p1 = contour[i];
//            Point p2 = contour[(i+1)%contour.size()];
//            line(bw_rgb, p1, p2, color, thickness);
//        }
//        ++colorChanger;
//    }

    for (auto corners : blobCorners) {
        const Scalar color(0, 0, 255);
        for (int i = 0; i < 4; ++i) {
            circle(bw_rgb, corners[i], 4, color, -1);
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
