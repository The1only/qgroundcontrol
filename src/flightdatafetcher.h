#ifndef FLIGHTDATAFETCHER_H
#define FLIGHTDATAFETCHER_H
#include <QObject>
#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "mission.h"

class FlightDataFetcher:public QObject
{

    Q_OBJECT
    QNetworkAccessManager * manager;
    inline static QList<Mission*> missions = QList<Mission*>();

public:
    explicit FlightDataFetcher(QObject *parent = 0);
    Q_INVOKABLE void printMessage(QString txt);
    Q_INVOKABLE void callAPI();
    Q_INVOKABLE void onFinish(QNetworkReply *rep);
    Q_INVOKABLE QStringList getMissions();
    Q_INVOKABLE void getCoordinates(QString missionTitle);
signals:
public slots:
};

#endif // FLIGHTDATAFETCHER_H
