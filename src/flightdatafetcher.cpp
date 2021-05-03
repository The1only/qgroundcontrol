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

    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
        connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onfinish(QNetworkReply*)));
        connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
        mgr->get(QNetworkRequest(QUrl("https://608acc18737e470017b73ee1.mockapi.io/users")));


}
void FlightDataFetcher::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QJsonDocument doc(bts);
    QString jsonString = doc.toJson(QJsonDocument::Indented);


    qDebug() << jsonString;
    qDebug() << "API Done" ;

}
