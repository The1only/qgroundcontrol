#include "oauthwrapper.h"


OAuthWrapper::OAuthWrapper(QObject *parent) : QObject(parent)
{
    //oAuthRequest.setScope("openid profile https://esmartglobalb2c.onmicrosoft.com/esmartapi-com/user_impersonation");
    oAuthRequest.setModifyParametersFunction(buildModifyParametersFunction());



    connect(&oAuthRequest, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [=](QUrl url) {
        QUrlQuery query(url);

   //     query.addQueryItem("prompt", "consent");
    //    query.addQueryItem("access_type", "offline");
        query.addQueryItem("GRANT TYPE", "Implicit");
   //     query.addQueryItem("REDIRECT URL", "https://esmartapi.com");
        url.setQuery(query);
        QDesktopServices::openUrl(url);
    });

    oAuthRequest.setAuthorizationUrl(QUrl("https://esmartglobalb2c.b2clogin.com/esmartglobalb2c.onmicrosoft.com/b2c_1a_signupsignin_production/oauth2/v2.0/authorize"));//https://accounts.oAuthRequest.com/o/oauth2/auth
    oAuthRequest.setAccessTokenUrl(QUrl("openid profile https://esmartglobalb2c.onmicrosoft.com/esmartapi-com/user_impersonation"));//https://oauth2.oAuthRequestapis.com/token
    oAuthRequest.setClientIdentifier("fdb6e380-84dd-447c-96f3-01e8c4d19bf3");
    oAuthRequest.setClientIdentifierSharedKey("fdb6e380-84dd-447c-96f3-01e8c4d19bf3");

    auto replyHandler = new QOAuthHttpServerReplyHandler(5476, this);
    oAuthRequest.setReplyHandler(replyHandler);

    connect(&oAuthRequest, &QOAuth2AuthorizationCodeFlow::granted, [=]() {
        qDebug() << "Access Granted!";
    });
}

void OAuthWrapper::click()
{
    oAuthRequest.grant();
}
// return ModifyParametersFunction()
QAbstractOAuth::ModifyParametersFunction OAuthWrapper::buildModifyParametersFunction()
{
    const QUrl clientIdentifier = oAuthRequest.clientIdentifier();
    const QUrl clientIdentifierSharedKey = oAuthRequest.clientIdentifierSharedKey();
    return [clientIdentifier,clientIdentifierSharedKey]
            (QAbstractOAuth::Stage stage, QVariantMap *parameters){
        if(stage == QAbstractOAuth::Stage::RequestingAuthorization){
            parameters->insert("redirect_uri","https://127.0.0.1:5000/"); /*change redirect uri*/ //"redirect_uri","https://127.0.0.1:8080/cb"
        }
    };
}
