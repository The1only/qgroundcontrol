#include "api.h"

#include <QDebug>

FlightDataFetcher::FlightDataFetcher(QObject *parent):

    QObject(parent)
{
}
void FlightDataFetcher::printMessage(QString txt)
{
    qDebug() << "Message from QML: " << txt;
}
void FlightDataFetcher::getAPI() {

    qDebug() << "Inside Get Request Caller Function" ;

    // TAzure AD B2C authentication connection.

    //    OAuthWrapper* connection = new OAuthWrapper();
    //    connection->authenticate();

    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onGetFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QUrl url = QUrl("https://dronefacade-new.stamp-we-dev-01.service.esmartapi.com/api/v2/missions?participantId=c74e2cc2-079f-45cf-9c0b-d43b710eeb84&source=m-gs-dji&withinDueDate=true");
    QNetworkRequest request(url);
    QString token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6ImF1UmlQbkVrNUNPWlNkWGdaSkhxRVZYblN0XzhVMkZxYmdjaWVNNENxU1EifQ.eyJpc3MiOiJodHRwczovL2VzbWFydGdsb2JhbGIyYy5iMmNsb2dpbi5jb20vZWVlYzczZGUtYTU5Ni00MTBiLWEwMTMtNDYyZWIzZjAxY2M5L3YyLjAvIiwiZXhwIjoxNjIyNTU0MTcxLCJuYmYiOjE2MjI1NTA1NzEsImF1ZCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInN1YiI6IjMwN2RhNzQ2LThjNGUtNDBmMi1hN2JiLTllZjcwYjgwMjI5ZiIsImVtYWlsIjoiYWhtYWQuYi5hc2xhbUBoaW9mLm5vIiwibmFtZSI6IkFobWFkIEJpbGFsIEFzbGFtIiwiZ2l2ZW5fbmFtZSI6IkFobWFkIEJpbGFsIiwiZmFtaWx5X25hbWUiOiJBc2xhbSIsIm5vbmNlIjoiMjA3ODQ0OTcxNjA4Iiwic2NwIjoidXNlcl9pbXBlcnNvbmF0aW9uIiwiYXpwIjoiZmRiNmUzODAtODRkZC00NDdjLTk2ZjMtMDFlOGM0ZDE5YmYzIiwidmVyIjoiMS4wIiwiaWF0IjoxNjIyNTUwNTcxfQ.HoogK7oyBRDQgpw9fuaomX_eohfbZYiY2YOrhEB4tZFfyKblisxFhitvEvtavGnmBQZLOn1uVbIU5uyhOpGlyMZAmMUr1Gy7dnFOBS1dWQhFFqiDFJzXl7a_2NB90SNtmucc_NRjMpzE1wKZ9fTZOv4B6lmxjwQT3yBhMs-0uRi2KBNuy3HLl3yMo_AmaJfCnc1_bCLGhecqBidmfVtRbe6wWbU6YqZGeF61aVBPEjcHhkpWwq8konBDl3J-OsMrSG1Kd3YyzCtaT_OmQs_2on2M8lRQw2iollqZKWaBYHjPA6GmwrKEWK9ygV4jzNJq6XlRn0lfRm5uPIAESMlKNw";
    auto header = QString("Bearer %1").arg(token);
    request.setRawHeader("X-SortOrder", "Desc");
    request.setRawHeader("x-tenantkey", "esmart-dev");
    request.setRawHeader(QByteArray("Authorization"), header.toUtf8());
    mgr->get(request);

}

void FlightDataFetcher::onGetFinish(QNetworkReply *rep)
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

// Function Returns list of all the mission that have been received from the GET Request
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
QString FlightDataFetcher::getSelectedMissionID(QString mTitle){

    QString missionID;
    QListIterator<Mission*> i(missions);
    while (i.hasNext()){
        Mission* mission = i.next();
        QString missionTitle = mission->getTitle();
        // Assuming Mission Title is Unique
        if ( missionTitle == mTitle) {
            missionID = mission->getID();
            break;
        }
    }
    return missionID;
}
// Function Returns the Coordinates of the Specified Mission
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
        //        delete mission;
    }
    return assetCoordinates;
}

void FlightDataFetcher::postAPI(QString fileName,QString missionID) {
    qDebug() << "Inside Post Request Caller Function" ;
    qDebug() << "File To be Uploaded :  " + fileName ;

    QNetworkAccessManager * manager= new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onPostFinish(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";filename=\"1.jpg\"")); //QVariant("name=\"file\"")); //
    QFile *file = new QFile(fileName);
    file->open(QIODevice::ReadOnly);
    imagePart.setBodyDevice(file);
    file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart
    multiPart->append(imagePart);
    QNetworkRequest request(QUrl("https://dronefacade-new.stamp-we-dev-01.service.esmartapi.com/api/v2/missions/" + missionID +"/upload"));
    QString token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsImtpZCI6ImF1UmlQbkVrNUNPWlNkWGdaSkhxRVZYblN0XzhVMkZxYmdjaWVNNENxU1EifQ.eyJpc3MiOiJodHRwczovL2VzbWFydGdsb2JhbGIyYy5iMmNsb2dpbi5jb20vZWVlYzczZGUtYTU5Ni00MTBiLWEwMTMtNDYyZWIzZjAxY2M5L3YyLjAvIiwiZXhwIjoxNjIyNTU0MTcxLCJuYmYiOjE2MjI1NTA1NzEsImF1ZCI6ImZkYjZlMzgwLTg0ZGQtNDQ3Yy05NmYzLTAxZThjNGQxOWJmMyIsInN1YiI6IjMwN2RhNzQ2LThjNGUtNDBmMi1hN2JiLTllZjcwYjgwMjI5ZiIsImVtYWlsIjoiYWhtYWQuYi5hc2xhbUBoaW9mLm5vIiwibmFtZSI6IkFobWFkIEJpbGFsIEFzbGFtIiwiZ2l2ZW5fbmFtZSI6IkFobWFkIEJpbGFsIiwiZmFtaWx5X25hbWUiOiJBc2xhbSIsIm5vbmNlIjoiMjA3ODQ0OTcxNjA4Iiwic2NwIjoidXNlcl9pbXBlcnNvbmF0aW9uIiwiYXpwIjoiZmRiNmUzODAtODRkZC00NDdjLTk2ZjMtMDFlOGM0ZDE5YmYzIiwidmVyIjoiMS4wIiwiaWF0IjoxNjIyNTUwNTcxfQ.HoogK7oyBRDQgpw9fuaomX_eohfbZYiY2YOrhEB4tZFfyKblisxFhitvEvtavGnmBQZLOn1uVbIU5uyhOpGlyMZAmMUr1Gy7dnFOBS1dWQhFFqiDFJzXl7a_2NB90SNtmucc_NRjMpzE1wKZ9fTZOv4B6lmxjwQT3yBhMs-0uRi2KBNuy3HLl3yMo_AmaJfCnc1_bCLGhecqBidmfVtRbe6wWbU6YqZGeF61aVBPEjcHhkpWwq8konBDl3J-OsMrSG1Kd3YyzCtaT_OmQs_2on2M8lRQw2iollqZKWaBYHjPA6GmwrKEWK9ygV4jzNJq6XlRn0lfRm5uPIAESMlKNw";
    auto header = QString("Bearer %1").arg(token);
    request.setRawHeader(QByteArray("Authorization"), header.toUtf8());
    request.setRawHeader("x-tenantkey", "esmart-dev");
    request.setRawHeader("x-source", "m-gs-dji");
    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply);
}
void FlightDataFetcher::onPostFinish(QNetworkReply *rep)
{
    QString jsonString = QString::fromUtf8(rep->readAll());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    qDebug() << jsonDoc;

    QVariant statusCode = rep->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    if ( status==200 )
        qDebug() << "Upload Successfull";
    return;

    if ( status != 200 )
    {
        QString reason = rep->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() << reason;
    }
}
