#ifndef MARKERDETECTORTEST_H
#define MARKERDETECTORTEST_H

#include <QString>
#include <QtTest>

class MarkerDetectorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void distort();
    void undistort();
    void distortUndistort();

    void orientDecoding();
};

#endif // MARKERDETECTORTEST_H
