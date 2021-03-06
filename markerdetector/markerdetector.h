#ifndef MARKERDETECTOR_H
#define MARKERDETECTOR_H

#include <opencv2/core.hpp>

#include <vector>

namespace MarkerDetector
{
// Types for internal usage
// point array to hold 2d pixel coords
typedef std::vector<cv::Point2i> PointArray;
// subpixel precision 2d point array
typedef std::vector<cv::Point2d> PointArraySp;

// dst - must not be pre-allocated; always assigned to a new instance.
bool convertToGray(cv::Mat src, cv::Mat &dst, bool rgbSwapped);

struct CameraData
{
    cv::Mat cameraMatrix;
    std::vector<double> distCoefs;
};

void undistortPoints(cv::InputArray distPoints,
                     cv::OutputArray udistPoints,
                     CameraData &camData);

void distortPoints(const PointArraySp &udistPoints,
                   PointArraySp &distPoints,
                   CameraData &cameraData);

struct BlobFinderInternals
{
    // thresholded image
    cv::Mat bwImg;

    // edge points of each found blob
    std::vector<std::vector<PointArray> > edges;
};

void findBlobCorners(cv::Mat srcImg, CameraData camData,
                       std::vector<PointArraySp> &outPoints,
                       BlobFinderInternals &interm);

struct MarkerDecoderInternals
{
    PointArraySp probePoints;
};

cv::Mat readMarkerContent(PointArraySp blobCorners,
                       cv::Mat image,
                       int resolution,
                       int margin,
                       CameraData camData,
                       MarkerDecoderInternals &intern);

int decodeOrientation(cv::Mat content);
}

#endif // MARKERDETECTOR_H
