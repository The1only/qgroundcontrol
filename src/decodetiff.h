#pragma once

#ifndef DECODETIFF_H
#define DECODETIFF_H

#include <QObject>
#include "QGCQGeoCoordinate.h"
#include "QGCLoggingCategory.h"

#include "gdal.h"

Q_DECLARE_LOGGING_CATEGORY(decodetiff)

#define NO_DATA -32767
#define NO_FILE -32768

class decodetiff
{

public:
    decodetiff();

    bool decode(const QGeoCoordinate& coordinate, QList<double>& altitudes, bool reportallways);
    char *SanitizeSRS( const char *pszUserInput );

    int main_dem(const char *pszSrcFilename);
    double getalt_dem(const double *pszLocX, const double *pszLocY);
    void close_dem();

private:
    OGRSpatialReferenceH hSrcSRS;
    OGRCoordinateTransformationH hCT;
    GDALDatasetH hSrcDS;

    QString            lastfile;
    char               *pszSourceSRS;
    std::vector<int>   anBandList;
    int                nOverview = -1;
    char             **papszOpenOptions = nullptr;
};

#endif // DECODETIFF_H
