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
    double fx = 4. * 800. / 8.; // focal length * image width / sensor width
    double fy = 4. * 600. / 6.;
    double cx = 800. / 2.;
    double cy = 600. / 2.;
    camData.cameraMatrix = (cv::Mat_<double>(3, 3) <<
                            fx, 0., cx,
                            0., fy, cy,
                            0., 0., 1.);

    camData.distCoefs = vector<double>{0.1, 0.01, 0.1, 0.01};

    udistPoints.push_back(Point2d{cx, cy});
    udistPoints.push_back(Point2d{0., 0.});

    distortPoints(udistPoints, distPoints, camData);
    undistortPoints(distPoints, udistPoints2, camData);

    QCOMPARE(udistPoints.size(), udistPoints2.size());
    QVERIFY(udistPoints[0] == udistPoints2[0]);
    QVERIFY(qAbs(udistPoints[1].x - udistPoints2[1].x) < 0.1);
    QVERIFY(qAbs(udistPoints[1].y - udistPoints2[1].y) < 0.1);
}
