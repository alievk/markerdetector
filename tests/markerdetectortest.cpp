#include "markerdetectortest.h"

#include "markerdetector.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <QImage>
#include <QDebug>

QTEST_APPLESS_MAIN(MarkerDetectorTest)

using namespace MarkerDetector;
using namespace std;
using namespace cv;

void MarkerDetectorTest::distort()
{
    CameraData camData;
    double fx = 1.f;
    double fy = 1.f;
    double cx = 0.5f;
    double cy = 0.5f;
    camData.cameraMatrix = (cv::Mat_<double>(3, 3) <<
                            fx, 0., cx,
                            0., fy, cy,
                            0., 0., 1.);
    camData.distCoefs = vector<double>{0., 0., 0., 0.};

    PointArraySp udistPoints;
    PointArraySp distPoints;

    udistPoints.push_back(Point2d{cx, cy});
    udistPoints.push_back(Point2d{0., 0.});

    distortPoints(udistPoints, distPoints, camData);

    QCOMPARE(distPoints.size(), udistPoints.size());
    QVERIFY(udistPoints[0] == distPoints[0]);
    QVERIFY(udistPoints[1] == distPoints[1]);

    camData.distCoefs = vector<double>{0.1, 0.1, 0.1, 0.1};

    distortPoints(udistPoints, distPoints, camData);

    QCOMPARE(distPoints.size(), udistPoints.size());
    QVERIFY(udistPoints[0] == distPoints[0]);
    QVERIFY(udistPoints[1] != distPoints[1]);
}

void MarkerDetectorTest::undistort()
{
    PointArraySp distPoints;
    PointArraySp udistPoints;

    CameraData camData;
    double fx = 1.f;
    double fy = 1.f;
    double cx = 0.5f;
    double cy = 0.5f;
    camData.cameraMatrix = (cv::Mat_<double>(3, 3) <<
                            fx, 0., cx,
                            0., fy, cy,
                            0., 0., 1.);

    camData.distCoefs = vector<double>{0., 0., 0., 0.};

    distPoints.push_back(Point2d{cx, cy});
    distPoints.push_back(Point2d{0., 0.});

    undistortPoints(distPoints, udistPoints, camData);

    QCOMPARE(distPoints.size(), udistPoints.size());
    QVERIFY(distPoints[0] == udistPoints[0]);
    QVERIFY(distPoints[1] == udistPoints[1]);

    camData.distCoefs = vector<double>{0.1, 0.1, 0.1, 0.1};

    undistortPoints(distPoints, udistPoints, camData);

    QCOMPARE(distPoints.size(), udistPoints.size());
    QVERIFY(distPoints[0] == udistPoints[0]);
    QVERIFY(distPoints[1] != udistPoints[1]);
}

void MarkerDetectorTest::distortUndistort()
{
    PointArraySp distPoints;
    PointArraySp udistPoints, udistPoints2;
    CameraData camData;

    double fx = 1000.;
    double fy = 950.;
    double cx = 324.;
    double cy = 249.;
    camData.cameraMatrix = (cv::Mat_<double>(3, 3) <<
                            fx, 0., cx,
                            0., fy, cy,
                            0., 0., 1.);

    // k1, k2, p1, p2, k3
    camData.distCoefs = vector<double>{0.5, -0.5, 0.07, -0.05, 0.000005};

    udistPoints.push_back(Point2d{cx, cy});
    udistPoints.push_back(Point2d{0., 0.});

    distortPoints(udistPoints, distPoints, camData);
    undistortPoints(distPoints, udistPoints2, camData);

    QCOMPARE(udistPoints.size(), udistPoints2.size());
    for (int i {0}; i < (int)udistPoints.size(); ++i) {
        Point2d residual = udistPoints[i] - udistPoints2[i];
        double norm = sqrt(residual.dot(residual));
        QVERIFY(norm < 2e-3);
    }
}
