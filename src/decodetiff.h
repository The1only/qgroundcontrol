#pragma once

#ifndef DECODETIFF_H
#define DECODETIFF_H

#include <QObject>
#include "QGCQGeoCoordinate.h"
#include "QGCLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(decodetiff)

#define NO_DATA -32767
#define NO_FILE -32768

class decodetiff
{

public:
    decodetiff();

    double decode(const QGeoCoordinate& coordinate);
    double main_dem(const char *pszSrcFilename, const double *pszLocX, const double *pszLocY);

};

#endif // DECODETIFF_H
