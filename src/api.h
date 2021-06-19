#ifndef API_H
#define API_H
#include <QObject>
#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "mission.h"
#include "oauthwrapper.h"
#include <QHttpMultiPart>
#include <QDebug>
#include <QQuickWindow>
#include <QQmlApplicationEngine>

class FlightDataFetcher:public QObject
{

    Q_OBJECT
    QNetworkAccessManager * manager;
    inline static QList<Mission*> missions = QList<Mission*>();
    QString accessToken;

public:
    explicit FlightDataFetcher(QObject *parent = 0);
    Q_INVOKABLE void printMessage(QString txt);
    Q_INVOKABLE void authorize();
    Q_INVOKABLE void deleteMissions();
    Q_INVOKABLE void getAPI();  // Get Request to Fetch Missions
//    Q_INVOKABLE void postAPI(QString fileName,QString missionID); // Post Request to Upload single Asset Image
    Q_INVOKABLE void postImagesAPI(QList<QString> urls,QString missionID); // Post Request to Upload Asset Images
    Q_INVOKABLE void onGetFinish(QNetworkReply *rep);
    Q_INVOKABLE void onPostFinish(QNetworkReply *rep);
    Q_INVOKABLE QStringList getMissions();
    Q_INVOKABLE QString getSelectedMissionID(QString mTitle);
    Q_INVOKABLE QJsonArray getCoordinates(QString missionTitle);

signals:
    void tokenReceived(QVariant text);
public slots:
};

#endif // API_H
