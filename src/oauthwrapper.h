#ifndef OAUTHWRAPPER_H
#define OAUTHWRAPPER_H
#include <QObject>
#include <QtNetworkAuth>
#include <QOAuth2AuthorizationCodeFlow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>

class OAuthWrapper:public QObject
{
public:
    explicit OAuthWrapper(QObject *parent = nullptr);
    Q_INVOKABLE void click();
private:
    QOAuth2AuthorizationCodeFlow oAuthRequest;
   QAbstractOAuth::ModifyParametersFunction buildModifyParametersFunction();
};

#endif // OAUTHWRAPPER_H




