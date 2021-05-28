#include "mission.h"

Mission::Mission( QString mTitle, QString mid,QJsonArray mCoordinates )
{
    title = mTitle;
    id = mid;
    coordinates = mCoordinates;
}
QString Mission::getTitle() {
    return title;
};
QString Mission::getID() {
    return id;
};
QJsonArray Mission::getAssetCoordinates() {
    return coordinates;
};
