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

    QVERIFY(udistPoints[0] == distPoints[0]);
    QVERIFY(udistPoints[1] != distPoints[1]);
}

