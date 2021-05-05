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
    QUrl url ="https://dronefacade-new.stamp-we-dev-01.service.esmartapi.com/api/v2/missions?participantId=c74e2cc2-079f-45cf-9c0b-d43b710eeb84&source=m-gs-dji&withinDueDate=true";
    QNetworkRequest request(url);
    QString token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6ImF1UmlQbkVrNUNPWlNkWGdaSkhxRVZYblN0XzhVMkZxYmdjaWVNNENxU1EifQ.eyJpc3MiOiJodHRwczovL2VzbWFydGdsb2JhbGIyYy5iMmNsb2dpbi5jb20vZWVlYzczZGUtYTU5Ni00MTBiLWEwMTMtNDYyZWIzZjAxY2M5L3YyLjAvIiwiZXhwIjoxNjIwMjM1NDMyLCJuYmYiOjE2MjAyMzE4MzIsImF1ZCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInN1YiI6IjMwN2RhNzQ2LThjNGUtNDBmMi1hN2JiLTllZjcwYjgwMjI5ZiIsImVtYWlsIjoiYWhtYWQuYi5hc2xhbUBoaW9mLm5vIiwibmFtZSI6IkFobWFkIEJpbGFsIEFzbGFtIiwiZ2l2ZW5fbmFtZSI6IkFobWFkIEJpbGFsIiwiZmFtaWx5X25hbWUiOiJBc2xhbSIsIm5vbmNlIjoiNzM3NzM3ODcxODIwNSIsInNjcCI6InVzZXJfaW1wZXJzb25hdGlvbiIsImF6cCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInZlciI6IjEuMCIsImlhdCI6MTYyMDIzMTgzMn0.IBpQ6Sp1WW0uOMirNJGWFtFrSfckAeHVYPz1gTGVYha8c-FTGtFlkLGm4eoT-IdjyeLQwy4cQifQxeW9UH55B5OBszGKm6kJFNIiSBkXl0GWVNBe2KU4UCpWImMyF_c2rMiNup6wa_La1ifZpywS1khjTqg5zf2wzthz_iIIgfiHoacz55JOSjGP68V62MzxTR1JV73vCm6i9EaZs2_nwkmwxt9IIn-vM8AATkvLzmjsW0P0Kkhea7K-Cf7HW_bK2rz3XiAbjkNGcdljfpiB6X-QVgYqtcN6UkcTuKbsKu9RU84g8UdIreHBp0hD2rKL7bd4Ff-7pIcUNI9lyzSKUQ";
    auto header = QString("Bearer %1").arg(token);
    request.setRawHeader("X-SortOrder", "Desc");
    request.setRawHeader("x-tenantkey", "esmart-dev");
    request.setRawHeader(QByteArray("Authorization"), header.toUtf8());
    mgr->get(request);


}
void FlightDataFetcher::onfinish(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QString str(bts);
    qDebug() << bts;
    qDebug() << "API Successful" ;

}
