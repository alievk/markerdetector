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

struct BlobFinderInternals
{
    // thresholded image
    cv::Mat bwImg;

    // edge points of each found blob
    std::vector<std::vector<PointArray> > edges;
};

void findBlobCorners(cv::Mat srcImg,
                       std::vector<PointArraySp> &outPoints,
                       BlobFinderInternals &interm);
}

#endif // MARKERDETECTOR_H
