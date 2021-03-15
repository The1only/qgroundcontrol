#include "decodetiff.h"
#include "QGCQGeoCoordinate.h"
#include <QDirIterator>
#include "QGCLoggingCategory.h"
#include "AppSettings.h"
#include "QGCApplication.h"
#include "SettingsManager.h"

#include "gdal.h"
#include "cpl_string.h"
#include "ogr_spatialref.h"
#include "cpl_minixml.h"

#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/*
gdallocationinfo -wgs84 dtm1.tif  10.4902 60.44828
*/
QGC_LOGGING_CATEGORY(decodetiff, "decodetiff")

decodetiff::decodetiff()
{
    this->lastfile = nullptr;
    this->pszSourceSRS =  SanitizeSRS("WGS84");
    this->hSrcSRS = nullptr;
    this->hCT = nullptr;
}

// Fast surge for altitudes in a group of geoTIF files...
bool decodetiff::decode(const QGeoCoordinate& coordinate, QList<double>& altitudes, bool reportallways)
{
    double elevation = NO_DATA;
    double lat = coordinate.latitude();
    double lon = coordinate.longitude();

    // First try the last file... This speeds up the system over 10 times...
    if(lastfile != nullptr && lastfile != ""){
        elevation = getalt_dem(&lon,&lat);
        if(elevation == NO_DATA || elevation == NO_FILE){
            lastfile="";
            close_dem();
        }
    }

    // If we got no data in cach then start look in all the files...
    if(elevation == NO_DATA || elevation == NO_FILE){
        QString path = qgcApp()->toolbox()->settingsManager()->appSettings()->elevationMapPath();
        QDirIterator it(path, QStringList() << "*.tif", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()){
            QString file = it.next();
            if(file != ""){
                if(main_dem(file.toLocal8Bit().data())==0){
                    elevation = getalt_dem(&lon,&lat);
                    if(elevation == NO_DATA || elevation == NO_FILE){
                        lastfile="";
                        close_dem();
                    }
                }
                if(elevation != NO_DATA && elevation != NO_FILE){
                    lastfile = file;
                    break;
                }
            }
        }
    }

    // If no elevation data found then report nanf ... or not...
    if(elevation == NO_DATA || elevation == NO_FILE){
        if(reportallways){
            elevation = nanf(nullptr);
            altitudes.push_back(elevation);
        }
        return false;
    }

    // Data found report altitude...
    altitudes.push_back(elevation);
    return true;
}

#define TERJE
#ifdef TERJE

/************************************************************************/
/*                             SanitizeSRS                              */
/************************************************************************/

char *decodetiff::SanitizeSRS( const char *pszUserInput )
{
    CPLErrorReset();

    OGRSpatialReferenceH hSRS = OSRNewSpatialReference( nullptr );

    char *pszResult = nullptr;
    if( OSRSetFromUserInput( hSRS, pszUserInput ) == OGRERR_NONE )
        OSRExportToWkt( hSRS, &pszResult );
    else
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "Translating source or target SRS failed:\n%s",
                  pszUserInput );
        return nullptr;;
    }

    OSRDestroySpatialReference( hSRS );

    return pszResult;
}

/************************************************************************/
/*                                main()                                */
/************************************************************************/
int decodetiff::main_dem(const char *pszSrcFilename)
{
    this->lastfile = nullptr;
    this->pszSourceSRS =  SanitizeSRS("WGS84");
    this->hSrcSRS = nullptr;
    this->hCT = nullptr;

    GDALAllRegister();

/* -------------------------------------------------------------------- */
/*      Open source file.                                               */
/* -------------------------------------------------------------------- */
    hSrcDS = GDALOpenEx( pszSrcFilename, GDAL_OF_RASTER | GDAL_OF_VERBOSE_ERROR,
                      nullptr,
                      papszOpenOptions, nullptr );
    if( hSrcDS == nullptr )
        return NO_FILE;

/* -------------------------------------------------------------------- */
/*      Setup coordinate transformation, if required                    */
/* -------------------------------------------------------------------- */
    if( this->pszSourceSRS != nullptr && !EQUAL(this->pszSourceSRS,"-geoloc") )
    {
        hSrcSRS = OSRNewSpatialReference( this->pszSourceSRS );
        OSRSetAxisMappingStrategy(hSrcSRS, OAMS_TRADITIONAL_GIS_ORDER);
        auto hTrgSRS = GDALGetSpatialRef( hSrcDS );
        if( !hTrgSRS )
            return NO_DATA;

        this->hCT = OCTNewCoordinateTransformation( hSrcSRS, hTrgSRS );
        if( this->hCT == nullptr )
            return NO_DATA;
    }

/* -------------------------------------------------------------------- */
/*      If no bands were requested, we will query them all.             */
/* -------------------------------------------------------------------- */
    if( anBandList.empty() )
    {
        for( int i = 0; i < GDALGetRasterCount( hSrcDS ); i++ )
            anBandList.push_back( i+1 );
    }
    return 0;
}

/* -------------------------------------------------------------------- */
/*      Turn the location into a pixel and line location.               */
/* -------------------------------------------------------------------- */
double decodetiff::getalt_dem(const double *pszLocX, const double *pszLocY)
{
    double value = NO_DATA;
    int inputAvailable = 1;
    double dfGeoX;
    double dfGeoY;
    CPLString osXML;

    dfGeoX = *pszLocX;
    dfGeoY = *pszLocY;

    while (inputAvailable)
    {
        int iPixel, iLine;

        if (hCT)
        {
            if( !OCTTransform( hCT, 1, &dfGeoX, &dfGeoY, nullptr ) )
                return NO_DATA;
        }

        if( pszSourceSRS != nullptr )
        {
            double adfGeoTransform[6] = {};
            if( GDALGetGeoTransform( hSrcDS, adfGeoTransform ) != CE_None )
            {
                CPLError(CE_Failure, CPLE_AppDefined, "Cannot get geotransform");
                return NO_DATA;
            }

            double adfInvGeoTransform[6] = {};
            if( !GDALInvGeoTransform( adfGeoTransform, adfInvGeoTransform ) )
            {
                CPLError(CE_Failure, CPLE_AppDefined, "Cannot invert geotransform");
                return NO_DATA;
            }

            iPixel = static_cast<int>(floor(
                adfInvGeoTransform[0]
                + adfInvGeoTransform[1] * dfGeoX
                + adfInvGeoTransform[2] * dfGeoY));
            iLine = static_cast<int>(floor(
                adfInvGeoTransform[3]
                + adfInvGeoTransform[4] * dfGeoX
                + adfInvGeoTransform[5] * dfGeoY));
        }
        else
        {
            iPixel = static_cast<int>(floor(dfGeoX));
            iLine  = static_cast<int>(floor(dfGeoY));
        }

    /* -------------------------------------------------------------------- */
    /*      Prepare report.                                                 */
    /* -------------------------------------------------------------------- */
        CPLString osLine;
        bool bPixelReport = true;

        if( iPixel < 0 || iLine < 0
            || iPixel >= GDALGetRasterXSize( hSrcDS )
            || iLine  >= GDALGetRasterYSize( hSrcDS ) )
        {
            bPixelReport = false;
        }

    /* -------------------------------------------------------------------- */
    /*      Process each band.                                              */
    /* -------------------------------------------------------------------- */
        for( int i = 0; bPixelReport && i < static_cast<int>(anBandList.size());
             i++ )
        {
            GDALRasterBandH hBand = GDALGetRasterBand( hSrcDS, anBandList[i] );

            int iPixelToQuery = iPixel;
            int iLineToQuery = iLine;

            if (nOverview >= 0 && hBand != nullptr)
            {
                GDALRasterBandH hOvrBand = GDALGetOverview(hBand, nOverview);
                if (hOvrBand != nullptr)
                {
                    int nOvrXSize = GDALGetRasterBandXSize(hOvrBand);
                    int nOvrYSize = GDALGetRasterBandYSize(hOvrBand);
                    iPixelToQuery = static_cast<int>(
                        0.5 +
                        1.0 * iPixel / GDALGetRasterXSize(hSrcDS) * nOvrXSize);
                    iLineToQuery = static_cast<int>(
                        0.5 +
                        1.0 * iLine / GDALGetRasterYSize(hSrcDS) * nOvrYSize);
                    if (iPixelToQuery >= nOvrXSize)
                        iPixelToQuery = nOvrXSize - 1;
                    if (iLineToQuery >= nOvrYSize)
                        iLineToQuery = nOvrYSize - 1;
                }
                else
                {
                    CPLError(CE_Failure, CPLE_AppDefined,
                             "Cannot get overview %d of band %d",
                             nOverview + 1, anBandList[i] );
                }
                hBand = hOvrBand;
            }

            if (hBand == nullptr)
                continue;

    /* -------------------------------------------------------------------- */
    /*      Report the pixel value of this band.                            */
    /* -------------------------------------------------------------------- */
            double adfPixel[2];

            if( GDALRasterIO( hBand, GF_Read, iPixelToQuery, iLineToQuery, 1, 1,
                              adfPixel, 1, 1, GDT_CFloat64, 0, 0) == CE_None )
            {
                CPLString osValue;
                value =adfPixel[0];

                // Report unscaled if we have scale/offset values.
                int bSuccess;

                double dfOffset = GDALGetRasterOffset( hBand, &bSuccess );
                double dfScale  = GDALGetRasterScale( hBand, &bSuccess );

                if( dfOffset != 0.0 || dfScale != 1.0 )
                {
                    adfPixel[0] = adfPixel[0] * dfScale + dfOffset;
                    adfPixel[1] = adfPixel[1] * dfScale + dfOffset;
                    value =adfPixel[0];
                }
            }
        }

        if( (pszLocX != nullptr && pszLocY != nullptr)  ||
            (fscanf(stdin, "%lf %lf", &dfGeoX, &dfGeoY) != 2) )
        {
            inputAvailable = 0;
        }
    }
    return value;
}


/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */

void decodetiff::close_dem()
{
    /*
    if (hCT) {
        OSRDestroySpatialReference( hSrcSRS );
        OCTDestroyCoordinateTransformation( hCT );
    }
*/
    GDALClose(hSrcDS);
    GDALDumpOpenDatasets( stderr );
    GDALDestroyDriverManager();
    CPLFree(pszSourceSRS);
    CSLDestroy(papszOpenOptions);

}
#endif

