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
void FlightDataFetcher::callApi() {

    qDebug() << "Inside API Caller Function" ;
    QNetworkRequest request(QUrl("https://608acc18737e470017b73ee1.mockapi.io/users"));
    NetRepl = NetAccMan.get(request);

}
