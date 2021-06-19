#ifndef OAUTHWRAPPER_H
#define OAUTHWRAPPER_H
#include <QOAuth2AuthorizationCodeFlow>
#include <QWebEngineView>


class OAuthWrapper : public QObject {
    Q_OBJECT
public:
    OAuthWrapper(QObject *parent=nullptr);
    virtual ~OAuthWrapper();

public slots:
    QString authenticate();

signals:
    void gotToken(const QString& token);


private:
    QOAuth2AuthorizationCodeFlow * azure;
    QWebEngineView browser;
    QString accessToken;
};

#endif
