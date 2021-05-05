#ifndef FLIGHTDATAFETCHER_H
#define FLIGHTDATAFETCHER_H
#include <QObject>
#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class FlightDataFetcher:public QObject
{
Q_OBJECT
    QNetworkAccessManager * manager;
   // QNetworkRequest request;

public:
explicit FlightDataFetcher(QObject *parent = 0);
Q_INVOKABLE void printMessage(QString txt);
Q_INVOKABLE void callApi();
Q_INVOKABLE void onfinish(QNetworkReply *rep);

signals:

public slots:

};

#endif // FLIGHTDATAFETCHER_H
