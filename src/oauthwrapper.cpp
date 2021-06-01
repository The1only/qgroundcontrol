#include "oauthwrapper.h"
#include <QString>
#include <QDir>
#include <QUrl>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QNetworkReply>

// Get these from https://console.developers.azure.com/apis/credentials
#define CLIENT_ID "fdb6e380-84dd-447c-96f3-01e8c4d19bf3"
#define CLIENT_SECRET "fdb6e380-84dd-447c-96f3-01e8c4d19bf3"
#define AUTH_URI "https://esmartglobalb2c.b2clogin.com/esmartglobalb2c.onmicrosoft.com/b2c_1a_signupsignin_production/oauth2/v2.0/authorize"
#define TOKEN_URI "openid profile https://esmartglobalb2c.onmicrosoft.com/esmartapi-com/user_impersonation"



//#define REDIRECT_URI "urn:ietf:wg:oauth:2.0:oob"
//#define REDIRECT_URI "http://localhost"
//#define REDIRECT_URI "http://localhost:1234/"
//#define REDIRECT_URI "http://127.0.0.1:1234/"

#define REDIRECT_URI "msalf78c6c92-20c9-4a39-b8c6-fcc6b8f618a7://auth"

OAuthWrapper::OAuthWrapper(QObject *parent) : QObject(parent) {
    this->azure = new QOAuth2AuthorizationCodeFlow(this);
    this->azure->setScope("openid profile https://esmartglobalb2c.onmicrosoft.com/esmartapi-com/user_impersonation");
    connect(this->azure, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    const QUrl authUri(AUTH_URI);
    const auto clientId = CLIENT_ID;
    const QUrl tokenUri(TOKEN_URI);
    const auto clientSecret(CLIENT_SECRET);
    const QUrl redirectUri(REDIRECT_URI);
    const auto port = static_cast<quint16>(redirectUri.port());
    this->azure->setAuthorizationUrl(authUri);
    this->azure->setClientIdentifier(clientId);
    this->azure->setAccessTokenUrl(tokenUri);
    this->azure->setClientIdentifierSharedKey(clientSecret);

    this->azure->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QVariantMap* parameters) {
        // Percent-decode the "code" parameter so azure can match it
        if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
            QByteArray code = parameters->value("code").toByteArray();
            (*parameters)["code"] = QUrl::fromPercentEncoding(code);
        }
    });


     this->azure->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QVariantMap* parameters) {
        if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
            parameters->insert("resource", "openid profile https://esmartglobalb2c.onmicrosoft.com/esmartapi-com/user_impersonation");
        }
    });
    this->azure->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QVariantMap* parameters) {
        if(stage == QAbstractOAuth::Stage::RequestingAuthorization){
                   parameters->insert("redirect_uri","https://127.0.0.1:8080/cb"); /*change redirect uri*/ //"redirect_uri","https://127.0.0.1:8080/cb"
               }
   });


    QOAuthHttpServerReplyHandler* replyHandler = new QOAuthHttpServerReplyHandler(port, this);
    this->azure->setReplyHandler(replyHandler);

    connect(this->azure, &QOAuth2AuthorizationCodeFlow::granted, [=](){
        const QString token = this->azure->token();
      //  emit gotToken(token);
         qDebug() << token;

// Alternatively, just use the token for your purposes
//        auto reply = this->azure->get(QUrl("https://people.azureapis.com/v1/{resourceName=people/me}"));
//        connect(reply, &QNetworkReply::finished, [reply](){
//            qInfo() << reply->readAll();
//            qDebug() << reply->readAll();
//        });
    });
}

OAuthWrapper::~OAuthWrapper() {
    delete this->azure;
}

// Invoked externally to initiate
void OAuthWrapper::authenticate() {
    this->azure->grant();
}
