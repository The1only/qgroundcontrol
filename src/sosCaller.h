#ifndef SOSCALLER_H
#define SOSCALLER_H
#include <QObject>

class SosCaller : public QObject
{
Q_OBJECT
public:
explicit SosCaller(QObject *parent = 0);
Q_INVOKABLE void printMessage(QString txt);

signals:

public slots:

};
#endif // SOSCALLER_H
