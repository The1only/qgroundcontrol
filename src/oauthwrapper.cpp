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
#define TOKEN_URI "https://esmartglobalb2c.b2clogin.com/esmartglobalb2c.onmicrosoft.com/b2c_1a_signupsignin_production/oauth2/v2.0/authorize"
#define REDIRECT_URI "msalf78c6c92-20c9-4a39-b8c6-fcc6b8f618a7://auth"

OAuthWrapper::OAuthWrapper(QObject *parent) : QObject(parent) {

    this->azure = new QOAuth2AuthorizationCodeFlow(this);
    this->azure->setScope("openid profile https://esmartglobalb2c.onmicrosoft.com/esmartapi-com/user_impersonation");
    const QUrl authUri(AUTH_URI);
    const auto clientId = CLIENT_ID;
    const QUrl tokenUri(TOKEN_URI);
    const QUrl redirectUri(REDIRECT_URI);
    this->azure->setAuthorizationUrl(authUri);
    this->azure->setClientIdentifier(clientId);
    this->azure->setAccessTokenUrl(tokenUri);
    this->azure->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QVariantMap* parameters) {
        if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
            parameters->insert("scope", "openid profile https://esmartglobalb2c.onmicrosoft.com/esmartapi-com/user_impersonation");
        }
    });
    this->azure->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QVariantMap* parameters) {
        if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
            parameters->insert("redirect_uri","http://localhost:5000");
            parameters->insert("response_type","token");
        }
    });
    // For opening in Deafult Browser
    // connect(this->azure, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    connect(this->azure, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [this](const QUrl & url){
        browser.resize(640, 500);
        browser.load(url);
        browser.show();
    });

    auto replyHandler = new QOAuthHttpServerReplyHandler(5000, this);
    connect(&browser, &QWebEngineView::urlChanged, [replyHandler, this](const QUrl & url){
        QString tempUrl =url.toEncoded();
        tempUrl = tempUrl.replace("?","#");
        QUrlQuery query (tempUrl);
        accessToken = query.queryItemValue("access_token");
        browser.close();
        if(accessToken !="") {
            emit gotToken(accessToken);
        }
    });


}
// Invoked externally to initiate
QString OAuthWrapper::authenticate() {

    this->azure->grant();
    return accessToken;
}

OAuthWrapper::~OAuthWrapper() {
    delete this->azure;
}


