#ifndef MISSION_H
#define MISSION_H
using namespace std;

class Mission
{

    QString title;
    QString id;
    QJsonArray coordinates;

public:
    Mission(QString mKey, QString mid,QJsonArray mCoordinates);
    QString getTitle();
    QString getID();
    QJsonArray getAssetCoordinates();

};

#endif // MISSION_H
