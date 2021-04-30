
/*
  Copyright (c) 2011 - Tőkés Attila

  This file is based upon part of SmtpClient for Qt.

  SmtpClient for Qt is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  SmtpClient for Qt is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY.

  See the LICENSE file for more details.
*/

#include "emailclient.h"
#include <QTextStream>
#include <QtCore>
#include "SmtpMime"

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


void EmailClient::sendMail2(const QString &touser, const QString &subject, const QString &body, QStringList files)
{

    // First create the SmtpClient object and set the user and the password.

    SmtpClient smtp("smtp.domeneshop.com", 465, SmtpClient::SslConnection);
 //   SmtpClient smtp("smtp.domeneshop.com", 587, SmtpClient::TlsConnection);

    smtp.setUser(this->user);
    smtp.setPassword(this->pass);

    // Create a MimeMessage

    MimeMessage message;

    EmailAddress sender("drone1@9tek.no", "Error message");
    message.setSender(&sender);

    EmailAddress to(touser, "Recipient's Name");
    message.addRecipient(&to);

    message.setSubject("SmtpClient for Qt - Demo");

    // Add some text
    MimeText text;
    text.setText("Hi!\n This is an email with some attachments.");
    message.addPart(&text);

    // Now we create the attachment object
    MimeAttachment attachment (new QFile("/Users/teni/Dropbox/Esmart_QGC/IMG_0750.jpg"));

    // the file type can be setted. (by default is application/octet-stream)
    attachment.setContentType("image/jpg");

    // Now add it to message
    message.addPart(&attachment);

    // Add an another attachment
 //   MimeAttachment document(new QFile("/Users/teni/Dropbox/Esmart_QGC/IMG_0750.jpg"));
 //   message.addPart(&document);

    // Now we can send the mail

    if (!smtp.connectToHost()) {
        qDebug() << "Failed to connect to host!" ;
        smtp.quit();
        return;
    }

    if (!smtp.login()) {
        qDebug() << "Failed to login!";
        smtp.quit();
        return;
    }

    if (!smtp.sendMail(message)) {
        qDebug() << "Failed to send mail!";
        smtp.quit();
        return;
    }

    smtp.quit();

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

    //qDebug() << "Server response code: " <<  responseLine;
    qDebug() << "Server response: " << response;

    responseLine.truncate(3);

    if (state == Init && responseLine == "220") {
        t << "EHLO localhost" <<"\r\n";
        qDebug() << "send: " << "EHLO localhost";
        t.flush();
        state = HandShake;
    }
    //No need, because I'm using socket.startClienEncryption() which makes the SSL handshake for you
    else if (state == Tls && responseLine == "250")
    {
        // Trying AUTH
        qDebug() << "STarting Tls";
//        t << "STARTTLS" << "\r\n";
        t << "STARTSSL" << "\r\n";
        qDebug() << "send: " << "STARTTLS";
        t.flush();
        state = HandShake;
    }
    else if (state == HandShake && responseLine == "250") {
        socket.startClientEncryption();
        if(!socket.waitForEncrypted(timeout)) {
            qDebug() << socket.errorString();
            state = Close;
        }
        t << "EHLO localhost" << "\r\n";
        qDebug() << "send: " << "EHLO localhost";
        t.flush();
        state = Auth;
    }
    else if (state == HandShake && responseLine == "502") {
         qDebug() << "Unsupported command...";
    }
    else if (state == Auth && responseLine == "250") {
        // Trying AUTH
        t << "AUTH LOGIN" << "\r\n";
        qDebug() << "send: " << "AUTH LOGIN" ;
        t.flush();
        state = User;
    }
    else if (state == User && responseLine == "334") {
        t << user << "\r\n";
        qDebug() << "send: " <<  user ;
        t.flush();
        state = Pass;
    }
    else if (state == Pass && responseLine == "334") {
        t << pass << "\r\n";
        qDebug() << "send: " <<  pass;
        t.flush();

        state = Mail;
    }
    else if (state == Mail && responseLine == "235") {
        t << "MAIL FROM:<" << user << ">\r\n";
        qDebug() << "send: " << "MAIL FROM:<" << user;
        t.flush();
        state = Rcpt;
    }
    else if (state == Rcpt && responseLine == "250") {
        t << "RCPT TO:<" << rcpt << ">\r\n"; //r
        qDebug() << "send: " << "RCPT TO:<" << rcpt;
        t.flush();
        state = Data;
    }
    else if (state == Data && responseLine == "250") {
        t << "DATA\r\n";
        qDebug() << "send: " << "DATA";
        t.flush();
        state = Body;
    }
    else if (state == Body && responseLine == "354") {
        t << message << "\r\n.\r\n";
        qDebug() << "send: " <<  message;
        t.flush();
        state = Quit;
    }
    else if (state == Quit && responseLine == "250") {
        t << "QUIT\r\n";
        qDebug() << "send: " << "QUIT";
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

