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
void FlightDataFetcher::callAPI() {

    qDebug() << "Inside API Caller Function" ;

    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QUrl url ="https://dronefacade-new.stamp-we-dev-01.service.esmartapi.com/api/v2/missions?participantId=c74e2cc2-079f-45cf-9c0b-d43b710eeb84&source=m-gs-dji&withinDueDate=true";
    QNetworkRequest request(url);
    QString token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6ImF1UmlQbkVrNUNPWlNkWGdaSkhxRVZYblN0XzhVMkZxYmdjaWVNNENxU1EifQ.eyJpc3MiOiJodHRwczovL2VzbWFydGdsb2JhbGIyYy5iMmNsb2dpbi5jb20vZWVlYzczZGUtYTU5Ni00MTBiLWEwMTMtNDYyZWIzZjAxY2M5L3YyLjAvIiwiZXhwIjoxNjIxNjMxNTY5LCJuYmYiOjE2MjE2Mjc5NjksImF1ZCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInN1YiI6IjMwN2RhNzQ2LThjNGUtNDBmMi1hN2JiLTllZjcwYjgwMjI5ZiIsImVtYWlsIjoiYWhtYWQuYi5hc2xhbUBoaW9mLm5vIiwibmFtZSI6IkFobWFkIEJpbGFsIEFzbGFtIiwiZ2l2ZW5fbmFtZSI6IkFobWFkIEJpbGFsIiwiZmFtaWx5X25hbWUiOiJBc2xhbSIsIm5vbmNlIjoiNjY1ODkxMDM4OTc3OCIsInNjcCI6InVzZXJfaW1wZXJzb25hdGlvbiIsImF6cCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInZlciI6IjEuMCIsImlhdCI6MTYyMTYyNzk2OX0.HdsEVcsegGHYqMEUIFLZxIRR_Xk3mr3o5Yq7ZpTN0pLQEW4oCeA1v2qpKvnyXdl_nAGC6R3rul64CBo8WwfjVGSyF4Cw741ZTxjptYOcJWYkCslRKOYCtp7JGtyp1ExGKfhU3FlOoCMGV8B9reRufOw3KymbL_wbqsB5Y7FB93GkKqbZcUuLulprbFJAcmtsAC5YwzYPVm643ObHlVCiU4lapzeFCSYCbCtBGmM_UyUb8vmBe6MabHLd41duMJOulTHtS48RKQgZsi88tcnATVyGLzsITB4IPFSBJ1ATpFYHJL3Q-Y1tvU5qv-lxj6hWne10hB-ecQOFFBqgEWqwxw";
    auto header = QString("Bearer %1").arg(token);
    request.setRawHeader("X-SortOrder", "Desc");
    request.setRawHeader("x-tenantkey", "esmart-dev");
    request.setRawHeader(QByteArray("Authorization"), header.toUtf8());
    mgr->get(request);

// Testing

//OAuthWrapper* temp = new OAuthWrapper();
//temp->click();



}
void FlightDataFetcher::onFinish(QNetworkReply *rep)
{


    QString jsonString = QString::fromUtf8(rep->readAll());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonArray ary = jsonDoc.array();

    foreach (const QJsonValue & value, ary)
    {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        qDebug() <<"Title: "<< title;
        QString id = obj["id"].toString();
        qDebug() <<"id: "<< id;
        QJsonArray assetCoordinates = obj["assetCoordinates"].toArray();
        Mission* mission = new Mission(title,id,assetCoordinates);

        missions.push_back(mission);
    }
    qDebug() <<"Coordinates Size: API called \n "<< missions.size();
    qDebug() << "API Successful" ;
}

QStringList FlightDataFetcher::getMissions()
{
    qDebug() <<"Getting Missions";
    QStringList missionsTitle ;
    QListIterator<Mission*> i(missions);
    while (i.hasNext()){
        QString missionTitle = i.next()->getTitle();
        missionsTitle.append(missionTitle);
    }
    return missionsTitle;
}

// Function Return the Coordinates of the Specified Mission
QJsonArray FlightDataFetcher::getCoordinates(QString mTitle)
{
    QJsonArray assetCoordinates;
    QListIterator<Mission*> i(missions);
    while (i.hasNext()){
        Mission* mission = i.next();
        QString missionTitle = mission->getTitle();
        // Assuming Mission Title is Unique
        if ( missionTitle == mTitle) {
            assetCoordinates = mission->getAssetCoordinates();
            }
        }
    return assetCoordinates;
}

