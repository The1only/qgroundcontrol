#ifndef OAUTHWRAPPER_H
#define OAUTHWRAPPER_H
#include <QOAuth2AuthorizationCodeFlow>

class OAuthWrapper : public QObject {
    Q_OBJECT
public:
    OAuthWrapper(QObject *parent=nullptr);
    virtual ~OAuthWrapper();

public slots:
    void authenticate();

signals:
    void gotToken(const QString& token);

private:
    QOAuth2AuthorizationCodeFlow * azure;
};




#endif // OAUTHWRAPPER_H




