#include "markerdetector.h"
#include "utils.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <iostream>

using namespace std;
using namespace cv;

namespace MarkerDetector
{

struct Line
{
    Point2d c; // point on the line
    Point2d v; // normalized collinear vector
};

Line fitEdgeToLine(InputArray edge)
{
    Vec4f linePrm;
    cv::fitLine(edge, linePrm, DIST_L2, 0., 0.01, 0.01);

    return Line{Point2d{linePrm(2), linePrm(3)}, Point2d{linePrm(0), linePrm(1)}};
}

Point2d calcIntersectionPoint(Line l1, Line l2)
{
    double vx1 = l1.v.x;
    double vy1 = l1.v.y;
    double vx2 = l2.v.x;
    double vy2 = l2.v.y;

    double det = vx1 * vy2 - vy1 * vx2;
    CV_Assert(det * det > 1e-6 && "Lines are collinear");

    Point2d dlt = l2.c - l1.c;
    double t = (vy1 * dlt.x - vx1 * dlt.y) / det;
    return l2.c + t * l2.v;
}

void extractEdges(const PointArray &quad, const PointArray &contour, vector<PointArray> &edges)
{
    CV_Assert(quad.size() == 4);

    edges.resize(4);

    for (int iQuadVert {0}; iQuadVert < 4; ++iQuadVert) {
        Point quadPoint1 = quad[iQuadVert];
        Point quadPoint2 = quad[(iQuadVert+1)%4];
        int iQuadVertInCont1 {-1};
        int iQuadVertInCont2 {-1};

        for (int iContVert {0}; iContVert < (int)contour.size(); ++iContVert) {
            const Point contourPoint = contour[iContVert];
            if (contourPoint == quadPoint1) {
                iQuadVertInCont1 = iContVert;
            }
            if (contourPoint == quadPoint2) {
                iQuadVertInCont2 = iContVert;
            }
        }

        int edgeLen = iQuadVertInCont2 > iQuadVertInCont1 ?
                    iQuadVertInCont2 - iQuadVertInCont1 :
                    iQuadVertInCont2 + (int)contour.size() - iQuadVertInCont1;

        int iEdge = iQuadVert;
        for (int i {0}; i < edgeLen - 1; ++i) {
             int iVert = (iQuadVertInCont1 + i) % contour.size();
            edges[iEdge].push_back(contour[iVert]);
        }
    }
}

void findBlobCorners(cv::Mat srcImg,
                     CameraData camData,
                     std::vector<PointArraySp> &outPoints,
                     BlobFinderInternals &internals)
{
    CV_Assert(!srcImg.empty());
    CV_Assert(srcImg.type() == CV_8UC1);

    outPoints.clear();

    Mat bwImg(srcImg.size(), CV_8UC1);

    int blockSize = 41;
    double C = 10;
    cv::adaptiveThreshold(srcImg, bwImg, 255, CV_THRESH_BINARY_INV, CV_ADAPTIVE_THRESH_MEAN_C, blockSize, C);

    internals.bwImg = bwImg.clone();

    vector<PointArray> contoursAll;
    cv::findContours(bwImg, contoursAll, RETR_LIST, CV_CHAIN_APPROX_NONE);

    const int minContourVerts = 20;
    const double minQuadArea = 100;
    for (PointArray contour : contoursAll) {
        if (contour.size() < minContourVerts) {
            continue;
        }

        PointArray approxContour;
        const double epsilon = 3.;
        const bool closed = true;
        cv::approxPolyDP(contour, approxContour, epsilon, closed);

        if (approxContour.size() == 4
                && cv::isContourConvex(approxContour)
                && cv::contourArea(approxContour) >= minQuadArea) {
            vector<PointArray> edges;
            extractEdges(approxContour, contour, edges);
            CV_Assert(edges.size() == 4);

            internals.edges.push_back(edges);

            vector<Line> lines;
            for (PointArray edge : edges) {
                PointArraySp udistEdge;
                MarkerDetector::undistortPoints(edge, udistEdge, camData);

                Line line = fitEdgeToLine(udistEdge);
                lines.push_back(line);
            }

            PointArraySp cornerSp(4);
            for (int k = 0; k < 4; ++k) {
                const Line lineA = lines[k];
                const Line lineB = lines[(k + 1) % 4];
                cornerSp[k] = calcIntersectionPoint(lineA, lineB);
            }

            PointArraySp distCornerSp;
            distortPoints(cornerSp, distCornerSp, camData);

            outPoints.push_back(distCornerSp);
        }
    }
}

bool convertToGray(cv::Mat src, cv::Mat &dst, bool rgbSwapped)
{
    CV_Assert(!src.empty());
    CV_Assert(src.type() == CV_8UC1
              || src.type() == CV_8UC3
              || src.type() == CV_8UC4);

    int code {0};
    const int type = src.type();

    if (type == CV_8UC1) {
        dst = src.clone();
        return true;
    }

    if (rgbSwapped) {
        if (type == CV_8UC3) {
            code = CV_BGR2GRAY;
        }
        else if (type == CV_8UC4) {
            code = CV_BGRA2GRAY;
        }
    }
    else {
        if (type == CV_8UC3) {
            code = CV_RGB2GRAY;
        }
        else if (type == CV_8UC4) {
            code = CV_RGBA2GRAY;
        }
    }

    if (code == 0) {
        return false;
    }

    cv::Mat gray(src.size(), CV_8UC1);
    cv::cvtColor(src, gray, code);
    dst = gray;

    return true;
}

void distortPoints(const PointArraySp &udistPoints, PointArraySp &distPoints, CameraData &cameraData)
{
    distPoints.clear();

    double fx = cameraData.cameraMatrix.at<double>(0, 0);
    double fy = cameraData.cameraMatrix.at<double>(1, 1);
    double cx = cameraData.cameraMatrix.at<double>(0, 2);
    double cy = cameraData.cameraMatrix.at<double>(1, 2);
    CV_Assert(fx * fy);
    vector<Point3d> objectPoints;
    for (Point2d up : udistPoints) {
        Point3d pnt;
        pnt.x = (up.x - cx) / fx;
        pnt.y = (up.y - cy) / fy;
        pnt.z = 1.;
        objectPoints.push_back(pnt);
    }

    projectPoints(objectPoints, Vec3d{}, Vec3d{}, cameraData.cameraMatrix, cameraData.distCoefs, distPoints);
}

void undistortPoints(InputArray distPoints, OutputArray udistPoints, CameraData &camData)
{
    CV_Assert(distPoints.type() == CV_32SC2 || distPoints.type() == CV_32FC2 || distPoints.type() == CV_64FC2);
    CV_Assert(udistPoints.type() == CV_64FC2);

    PointArraySp srcPoints;
    if (distPoints.type() == CV_32S) {
        PointArray arr = distPoints.getMat();
        for (Point p : arr) {
            srcPoints.push_back(Point2d{(double)p.x, (double)p.y});
        }
    }
    else {
        srcPoints = distPoints.getMat();
    }

    cv::undistortPoints(srcPoints, udistPoints, camData.cameraMatrix, camData.distCoefs, Mat{}, camData.cameraMatrix);
}

Mat readMarkerContent(PointArraySp blobCorners,
                       Mat image,
                       int contentRes,
                       int margin,
                       CameraData camData,
                       MarkerDecoderInternals &intern)
{
    // we describe points in our auxilary reference frame and find
    // perspective transformation (homography matrix) between these points and
    // points on the image, then apply that transformation to point grid
    // that we use further to read potential marker's content

    const double s = 1.; // reference marker edge length
    const double hs = s/2;

    PointArraySp cornersRef;
    cornersRef.push_back(Point2d{-hs, -hs});
    cornersRef.push_back(Point2d{hs, -hs});
    cornersRef.push_back(Point2d{hs, hs});
    cornersRef.push_back(Point2d{-hs, hs});

    const double contFrac = double(contentRes) / (contentRes + 2 * margin);
    const double hcont = contFrac * hs;
    const double cstep = s / (contentRes + 2 * margin);

    PointArraySp probePointsRef;
    for (int i {0}; i < contentRes; ++i) {
        const double cy = hcont - cstep * (i + 1./2);
        for (int j {0}; j < contentRes; ++j) {
            const double cx = -hcont + cstep * (j + 1./2);
            probePointsRef.push_back(Point2d{cx, cy});
        }
    }

    PointArraySp blobCornersUndist;
    undistortPoints(blobCorners, blobCornersUndist, camData);

    vector<Point3d> probePoints3D;
    Mat H = cv::findHomography(cornersRef, blobCornersUndist);
    cv::transform(probePointsRef, probePoints3D, H);

    vector<Point2d> probePointsUndist;
    cv::convertPointsFromHomogeneous(probePoints3D, probePointsUndist);

    vector<Point2d> probePoints;
    distortPoints(probePointsUndist, probePoints, camData);

    Mat content(contentRes, contentRes, CV_8U);
    for (int i {0}; i < contentRes; ++i) {
        for (int j {0}; j < contentRes; ++j) {
            Point2d p = probePoints[j + i * contentRes];
            content.at<uchar>(j, i) = image.at<uchar>((int)p.y, (int)p.x);
        }
    }

    intern.probePoints = probePoints;

    return content;
}

int decodeOrientation(Mat content)
{
    // given marker with resolution of 5
    // marker has 4 possible orientations
    // "0" orientation is when middle row has alternation colors, b-w-b-w.. if you read from left to right
    //  and middle column has 2 black and 3 white colors if you read from up to down
    // "1", "2", "3" orientations are achieved by rotation that pattern clockwise.

    CV_Assert(content.rows == content.cols);

    const int res = content.rows;
    CV_Assert(res == 5 && "Only markers with resolution 5 are supported for now");
    const int hres = res / 2;

    static auto isAlternating = [](const vector<uchar> &v) -> bool {
        for (int i {0}; i < v.size(); ++i) {
            bool w = v[i] != 0;
            bool odd = i % 2 != 0;
            if (odd == w) {
                return false;
            }
        }
        return true;
    };

    static auto isB2W3 = [](const vector<uchar> &v, bool reverse) -> bool {
        const int hs = (int)v.size() / 2;
        for (int i {0}; i < v.size(); ++i) {
            bool w = v[i] != 0;
            if (!reverse && (i < hs) == w || reverse && (i <= hs) != w) {
                return false;
            }
        }
        return true;
    };

    Mat midRow = content.row(hres);
    Mat midCol;
    cv::transpose(content.col(hres), midCol);

    if (isAlternating(midRow)) {
        if (isB2W3(midCol, false)) {
            return 0;
        }
        else if (isB2W3(midCol, true)) {
            return 2;
        }
    }
    else if (isAlternating(midCol)) {
        if (isB2W3(midRow, true)) {
            return 3;
        }
        else if (isB2W3(midRow, false)) {
            return 1;
        }
    }

    return -1;
}

} // MarkerDetector namespace
