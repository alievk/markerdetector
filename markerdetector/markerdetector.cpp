#include "markerdetector.h"
#include "utils.h"

#include <opencv2/imgproc.hpp>

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

Line fitEdgeToLine(const PointArray &edge)
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

bool extractEdges(const PointArray &quad, const PointArray &contour, vector<PointArray> &edges)
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

    return true;
}

void findBlobCorners(cv::Mat srcImg,
                     std::vector<PointArraySp> &outPoints,
                     BlobFinderInternals &interm)
{
    CV_Assert(!srcImg.empty());
    CV_Assert(srcImg.type() == CV_8UC1);

    Mat bwImg(srcImg.size(), CV_8UC1);

    int blockSize = 41;
    double C = 10;
    cv::adaptiveThreshold(srcImg, bwImg, 255, CV_THRESH_BINARY_INV, CV_ADAPTIVE_THRESH_MEAN_C, blockSize, C);

    interm.bwImg = bwImg.clone();

    vector<PointArray> contoursAll;
    cv::findContours(bwImg, contoursAll, RETR_LIST, CV_CHAIN_APPROX_NONE);

    const int minContourVerts = 20;
    const double minQuadArea = 100;
    vector<PointArray> quads;
    vector<PointArray> quadContours;
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
            quads.push_back(approxContour);
            quadContours.push_back(contour);
        }
    }

    CV_Assert(quads.size() == quadContours.size());

    vector<PointArraySp> quadListSp;

    vector<vector<PointArray> > edgesOfAllQuads;

    for (int q = 0; q < (int)quads.size(); ++q) {
        const PointArray &quad = quads[q];
        const PointArray &contour = quadContours[q];
        vector<PointArray> edges;
        CV_Assert(extractEdges(quad, contour, edges));
        CV_Assert(edges.size() == 4);

        edgesOfAllQuads.push_back(edges);

        vector<Line> lines;
        for_each(edges.begin(), edges.end(),
                 [&](PointArray edge)
        {
            lines.push_back(fitEdgeToLine(edge));
        });

        PointArraySp cornerSp(4);
        for (int k = 0; k < 4; ++k) {
            const Line lineA = lines[k];
            const Line lineB = lines[(k + 1) % 4];
            cornerSp[k] = calcIntersectionPoint(lineA, lineB);
        }

        quadListSp.push_back(cornerSp);
    }

    interm.edges = edgesOfAllQuads;

    outPoints = quadListSp;
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

} // MarkerDetector namespace
