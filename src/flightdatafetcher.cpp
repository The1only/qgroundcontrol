#include "flightdatafetcher.h"
#include <QDebug>

FlightDataFetcher::FlightDataFetcher(QObject *parent):
QObject(parent)
{
}
void FlightDataFetcher::printMessage(QString txt)
{
qDebug() << "Message from QML: " << txt;
}
void FlightDataFetcher::callApi()
{
qDebug() << "Inside API Caller Function" ;

}
