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
    QString token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6ImF1UmlQbkVrNUNPWlNkWGdaSkhxRVZYblN0XzhVMkZxYmdjaWVNNENxU1EifQ.eyJpc3MiOiJodHRwczovL2VzbWFydGdsb2JhbGIyYy5iMmNsb2dpbi5jb20vZWVlYzczZGUtYTU5Ni00MTBiLWEwMTMtNDYyZWIzZjAxY2M5L3YyLjAvIiwiZXhwIjoxNjIwODIyMDk5LCJuYmYiOjE2MjA4MTg0OTksImF1ZCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInN1YiI6IjMwN2RhNzQ2LThjNGUtNDBmMi1hN2JiLTllZjcwYjgwMjI5ZiIsImVtYWlsIjoiYWhtYWQuYi5hc2xhbUBoaW9mLm5vIiwibmFtZSI6IkFobWFkIEJpbGFsIEFzbGFtIiwiZ2l2ZW5fbmFtZSI6IkFobWFkIEJpbGFsIiwiZmFtaWx5X25hbWUiOiJBc2xhbSIsIm5vbmNlIjoiMjAzNzE2NDMxMzcxMCIsInNjcCI6InVzZXJfaW1wZXJzb25hdGlvbiIsImF6cCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInZlciI6IjEuMCIsImlhdCI6MTYyMDgxODQ5OX0.ghfVWG2STXlop7OIBSoD7kXeuiIrKB6EtQE8qqvRIEmM-IODDQI3KDu4P30bxXqPN-EUx5oOH1_Qg6j6ZaEQkI0gbaNHkizajXF9zHeLbf_QrEdx1Jqml0An-R2pd74RDF5236IdlQv1Gt0t-rfjLqmoLPacTmKxJOcASVVohv2LhlUpm9f7R0VHTBBrWQRkbULBtHsAFS43udYOdu5Q9Ri7sjNbic5fdZZEQ3lB0QOhrIl8ii9XXuWNQ6O7CgAnkuGk2VVYvkgmxy5Q12hR-iq0WD6jfd4eYXaDpF_M2hGHyS9tTrwaT2RLgo2w8R1j4670yDYJUt-1B1cqfsC35w";
    auto header = QString("Bearer %1").arg(token);
    request.setRawHeader("X-SortOrder", "Desc");
    request.setRawHeader("x-tenantkey", "esmart-dev");
    request.setRawHeader(QByteArray("Authorization"), header.toUtf8());
    mgr->get(request);


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
// Assuming Mission Title is Unique
void FlightDataFetcher::getCoordinates(QString mTitle)
{
    QListIterator<Mission*> i(missions);
    while (i.hasNext()){
        Mission* mission = i.next();
        QString missionTitle = mission->getTitle();
        if ( missionTitle == mTitle) {
            QJsonArray assetCoordinates = mission->getAssetCoordinates();
            foreach (const QJsonValue & c, assetCoordinates) {
                qDebug() <<"assetId: "<< c["assetId"].toString();
                qDebug() <<"long ->  "<< c["longitude"].toDouble();
                qDebug() <<"lat  ->  " <<c["latitude"].toDouble();
            }
        }
    }

}
