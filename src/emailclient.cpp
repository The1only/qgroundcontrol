#include "emailclient.h"

int main_test(void);

QTextCodec *EmailClient::codecx = QTextCodec::codecForName("UTF-8");

EmailClient::EmailClient(const QString &user, const QString &pass, const QString &host, quint16 port, int timeout )
{
    initialize(user,pass, host, port, timeout);
}


void EmailClient::initialize(const QString &user, const QString &pass, const QString &host, quint16 port, int timeout)
{
    connect(&socket, &QSslSocket::readyRead, this, &EmailClient::readyRead);
    connect(&socket, &QSslSocket::connected, this, &EmailClient::connected);
    connect(&socket, &QSslSocket::disconnected, this, &EmailClient::disconnected);
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorReceived(QAbstractSocket::SocketError)));

    this->user = user;
    this->pass = pass;

    this->host = host;
    this->port = port;
    this->timeout = timeout;
}

void EmailClient::sendMail(const QString &to, const QString &subject, const QString &body, QStringList files)
{
    message = "To: " + to + "\n";
    message.append("From: " + user + "\n");
    //subject = "=?UTF-8?B?" + QByteArray().append(subject).toBase64() + "?=\r\n";

    //message.append("Subject: " + subject + "\n");
    message.append("Subject: =?UTF-8?B?" + subject + "?=\r\n");
    message.append("MIME-Version: 1.0\n");
    message.append("Content-Type: multipart/mixed; boundary=frontier\n\n");

    message.append( "--frontier\n" );
    //message.append( "Content-Type: text/html\n\n" );  //Uncomment this for HTML formating, coment the line below
    message.append( "Content-Type: text/plain\n\n" );
    message.append(body).append("\n\n");

    if (!files.isEmpty()) {
        qDebug() << "Files to be sent: " << files.size();
        for (const auto &filePath : files) {
            QFile file(filePath);
            if (file.exists()) {
                if (!file.open(QIODevice::ReadOnly)) {
                    qDebug() << "Couldn't open file " + filePath;
                    lastResponse = "Couldn't open file " + filePath;
                    sendResult = SendResult::CannotOpenAttachedFile;
                    emit finished(sendResult);
                    return;
                }
                QByteArray bytes = file.readAll();
                message.append("--frontier\n");
                message.append("Content-Type: application/octet-stream\nContent-Disposition: attachment; filename=" + QFileInfo(file.fileName()).fileName() +";\nContent-Transfer-Encoding: base64\n\n");
                message.append(bytes.toBase64()).append("\n");
            }
            else {
                qDebug() << "Couldn't find file " + filePath;
                lastResponse = "Couldn't find file " + filePath;
                sendResult = SendResult::CannotFindAttachedFile;
                emit finished(sendResult);
                return;
            }
        }
    }

    message.append( "--frontier--\n" );

    message.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) );
    message.replace( QString::fromLatin1( "\r\n.\r\n" ),QString::fromLatin1( "\r\n..\r\n" ) );


    rcpt = to;
    state = Init;
    socket.connectToHostEncrypted(host, port);
    if (!socket.waitForConnected(timeout)) {
         qDebug() << socket.errorString();
         lastResponse = socket.errorString();
         sendResult = SendResult::Timeout;
     }

    //t = new QTextStream(socket);
    t.setDevice(&socket);

    t.setCodec(codecx);
}

EmailClient::~EmailClient()
{

}


void EmailClient::errorReceived(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Error: " << socketError;
}

void EmailClient::disconnected()
{
    qDebug() <<"Disconneted.";
    qDebug() << "Error: "  << socket.errorString();
    emit finished(sendResult);
}

void EmailClient::connected()
{
    qDebug() << "Connected to host.";

}

void EmailClient::readyRead()
{
    QString responseLine;
    do {
        responseLine = socket.readLine();
        response += responseLine;
    }
    while (socket.canReadLine() && responseLine[3] != ' ');

    responseLine.truncate(3);

    //qDebug() << "Server response code: " <<  responseLine;
    qDebug() << "Server response: " << response;

    if (state == Init && responseLine == "220") {
        t << "EHLO localhost" <<"\r\n";
        t.flush();
        state = HandShake;
    }
    //No need, because I'm using socket.startClienEncryption() which makes the SSL handshake for you
    /*else if (state == Tls && responseLine == "250")
    {
        // Trying AUTH
        qDebug() << "STarting Tls";
        t << "STARTTLS" << "\r\n";
        t.flush();
        state = HandShake;
    }*/
    else if (state == HandShake && responseLine == "250") {
        socket.startClientEncryption();
        if(!socket.waitForEncrypted(timeout)) {
            qDebug() << socket.errorString();
            state = Close;
        }
        t << "EHLO localhost" << "\r\n";
        t.flush();
        state = Auth;
    }
    else if (state == HandShake && responseLine == "502") {
         qDebug() << "Unsupported command...";
    }
    else if (state == Auth && responseLine == "250") {
        // Trying AUTH
        t << "AUTH LOGIN" << "\r\n";
        t.flush();
        state = User;
    }
    else if (state == User && responseLine == "334") {
        t << user << "\r\n";
        t.flush();
        state = Pass;
    }
    else if (state == Pass && responseLine == "334") {
        t << pass << "\r\n";
        t.flush();

        state = Mail;
    }
    else if (state == Mail && responseLine == "235") {
        t << "MAIL FROM:<" << user << ">\r\n";
        t.flush();
        state = Rcpt;
    }
    else if (state == Rcpt && responseLine == "250") {
        t << "RCPT TO:<" << rcpt << ">\r\n"; //r
        t.flush();
        state = Data;
    }
    else if (state == Data && responseLine == "250") {
        t << "DATA\r\n";
        t.flush();
        state = Body;
    }
    else if (state == Body && responseLine == "354") {
        t << message << "\r\n.\r\n";
        t.flush();
        state = Quit;
    }
    else if (state == Quit && responseLine == "250") {
        t << "QUIT\r\n";
        t.flush();
        state = Close;
        lastResponse = "Message sent!";
        sendResult = SendResult::Sent;
    }
    else if (state == Close) {
        deleteLater();
        return;
    }
    else {
        qDebug() << "Unexpected reply from SMTP server: " << response;
        lastResponse = response;
        state = Close;
        sendResult = SendResult::Error;
    }
    response = "";
}

QString EmailClient::getLastResponse() const
{
    return lastResponse;
}


EmailClient::SendResult EmailClient::getSendResult() const
{
    return sendResult;
}

