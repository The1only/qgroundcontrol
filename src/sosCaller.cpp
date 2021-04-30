#include "sosCaller.h"
#include <QDebug>

#include <QDesktopServices>

#include "emailclient.h"







SosCaller::SosCaller(QObject *parent) :
QObject(parent)
{

}

void SosCaller::printMessage(QString txt)
{
qDebug() << "Message from QML: " << txt;
QList <QString> file;

file.append("C:/Users/utlan/Desktop/DP.jpg");



QString message = "\nURL: https://www.google.com/maps/search/?api=1&query="+QString::number(10)+","+QString::number(20)+"\n";

EmailClient *email = new EmailClient("email","password","smtp.gmail.com",465,6000);

email->sendMail("ahmadbilal07@live.com","Header",message,file);
}
