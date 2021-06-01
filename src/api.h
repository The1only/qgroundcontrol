#ifndef API_H
#define API_H
#include <QObject>
#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "mission.h"
#include "oauthwrapper.h"
#include <QHttpMultiPart>

class FlightDataFetcher:public QObject
{

    Q_OBJECT
    QNetworkAccessManager * manager;
    inline static QList<Mission*> missions = QList<Mission*>();

public:
    explicit FlightDataFetcher(QObject *parent = 0);
    Q_INVOKABLE void printMessage(QString txt);
    Q_INVOKABLE void getAPI();  // Get Request to Fetch Missions
    Q_INVOKABLE void postAPI(QString fileName,QString missionID); // Post Request to Upload Asset Images
    Q_INVOKABLE void onGetFinish(QNetworkReply *rep);
    Q_INVOKABLE void onPostFinish(QNetworkReply *rep);
    Q_INVOKABLE QStringList getMissions();
    Q_INVOKABLE QString getSelectedMissionID(QString mTitle);
    Q_INVOKABLE QJsonArray getCoordinates(QString missionTitle);

signals:
public slots:
};

#endif // API_H
