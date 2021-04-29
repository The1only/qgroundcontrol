#ifndef FLIGHTDATAFETCHER_H
#define FLIGHTDATAFETCHER_H
#include <QObject>

class FlightDataFetcher:public QObject
{
Q_OBJECT
public:
explicit FlightDataFetcher(QObject *parent = 0);
Q_INVOKABLE void printMessage(QString txt);
Q_INVOKABLE void callApi();

signals:

public slots:

};

#endif // FLIGHTDATAFETCHER_H
