#include "api.h"


FlightDataFetcher::FlightDataFetcher(QObject *parent):

    QObject(parent)
{
}
void FlightDataFetcher::printMessage(QString txt)
{
    qDebug() << "Message from QML: " << txt;
}
void FlightDataFetcher::authorize() {
    qDebug() << "Autorizing" ;
    // Azure AD B2C authentication connection.
    OAuthWrapper* connection = new OAuthWrapper();
    connection->authenticate();
    connect(connection, &OAuthWrapper::gotToken, [this](const QString& token) {
        qDebug() <<"Access Token Received." ;
        accessToken = token;
    });

}
void FlightDataFetcher::getAPI() {
    qDebug() << "Inside Get Request Caller Function" ;
    QNetworkAccessManager * mgr = new QNetworkAccessManager(this);
    QUrl url = QUrl("https://dronefacade-new.stamp-we-dev-01.service.esmartapi.com/api/v2/missions?participantId=c74e2cc2-079f-45cf-9c0b-d43b710eeb84&source=m-gs-dji&withinDueDate=true"); //?participantId=c74e2cc2-079f-45cf-9c0b-d43b710eeb84&source=m-gs-dji&withinDueDate=true
    QNetworkRequest request(url);
    auto header = QString("Bearer %1").arg(accessToken);
    request.setRawHeader("X-SortOrder", "Desc");
    request.setRawHeader("x-tenantkey", "esmart-dev");
    request.setRawHeader(QByteArray("Authorization"), header.toUtf8());
    mgr->get(request);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onGetFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
}


void FlightDataFetcher::onGetFinish(QNetworkReply *rep)
{
    qDebug() << "Inside Get Request Callback Function :: onGetFinish" ;

    QString jsonString = QString::fromUtf8(rep->readAll());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonArray ary = jsonDoc.array();
    deleteMissions();
    foreach (const QJsonValue & value, ary)
    {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        QString id = obj["id"].toString();
        QJsonArray assetCoordinates = obj["assetCoordinates"].toArray();
        Mission* mission = new Mission(title,id,assetCoordinates);
        missions.push_back(mission);
    }

    QVariant statusCode = rep->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    if ( !statusCode.isValid() )
        return;
    int status = statusCode.toInt();
    if(status == 200){
        qDebug() <<"Response Received. Status : " <<status;
        qDebug() <<"Missions Received = "<< missions.size();
        qDebug() <<"API Call Successful" ;
    }
    else {
        QString reason = rep->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        qDebug() <<"Response Received. Status : " <<status;
        qDebug() <<"API Call Failed" ;
        qDebug() <<"Reason:  " <<reason;
    }

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
    }
    return assetCoordinates;
}
void FlightDataFetcher::postImagesAPI(QList<QString> urls,QString missionID) {

    qDebug() << "Inside Post Request Caller Function";

    QNetworkAccessManager * manager= new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onPostFinish(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    for (int i=0;i<urls.size();i++) {
        qDebug() << "Image To be Uploaded :  " + urls[i];
        QFile *file = new QFile(urls[i]);
        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+ file->fileName() + "\""));
        file->open(QIODevice::ReadOnly);
        filePart.setBodyDevice(file);
        file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart
        multiPart->append(filePart);
    }
    QNetworkRequest request(QUrl("https://dronefacade-new.stamp-we-dev-01.service.esmartapi.com/api/v2/missions/"+ missionID +"/upload")); //26814280-f49b-45a2-a809-6276bad3fc8f
    auto header = QString("Bearer %1").arg(accessToken);
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
void FlightDataFetcher::deleteMissions() {
    qDebug() << "Deleting Missions";
    QListIterator<Mission*> i(missions);
    while (i.hasNext()){
        Mission* mission = i.next();
        delete mission;
    }
    missions.clear();
}
