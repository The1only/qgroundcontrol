#ifndef EMAILCLIENT_H
#define EMAILCLIENT_H

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslSocket>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>


class EmailClient : public QObject
{
    Q_OBJECT
public:

    EmailClient(const QString &user, const QString &pass,
          const QString &host, quint16 port = 465, int timeout = 30000);
    EmailClient() = default;

    ~EmailClient();

    void sendMail(const QString &to,
                   const QString &subject, const QString &body,
                   QStringList files = QStringList());

    QString getLastResponse() const;

    enum SendResult { Sent, Error, Timeout, CannotOpenAttachedFile, CannotFindAttachedFile };
    SendResult getSendResult() const;

signals:
    void finished(int code);

private slots:
    void errorReceived(QAbstractSocket::SocketError socketError);
    void disconnected();
    void connected();
    void readyRead();

private:
    SendResult sendResult;
    int timeout;
    QString message;
    static QTextCodec *codecx;

    QTextStream t;
    QSslSocket socket;
    QString rcpt;
    QString response;
    QString user;
    QString pass;
    QString host;
    quint16 port;
    enum states { Tls, HandShake, Auth, User, Pass, Rcpt, Mail, Data, Init, Body, Quit, Close };
    int state;

    QString lastResponse;

    void initialize(const QString &user, const QString &pass,
                    const QString &host, quint16 port, int timeout);

};


#endif // EMAILCLIENT_H
