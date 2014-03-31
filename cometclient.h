#ifndef COMETCLIENT_H
#define COMETCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QSet>

class CometClient : public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager *nam;
    QSet<QNetworkReply*> requestsMade;

public:
    explicit CometClient(QObject *parent = 0);
    static QString decodeUnicode(const QString &unicodeCypher);

public:
    void post(const QString &requestUrlString, const QString &postDataString, int requestIdentifier = 0);
    void postSynchronously(const QString &requestUrlString, const QString &postDataString, int requestIdentifier = 0, int timeoutMilliseconds = 1000);
    void get(const QString &requestUrlString, int requestIdentifier = 0);

    void cancelAllRequests(); //memory leak due to Qt bug (in urlRequestFinished)


private slots:
    void urlRequestFinished(QNetworkReply *reply);
    void urlRequestError(QNetworkReply::NetworkError code);
signals:

public slots:

protected:
    //these methods should be overridden in subclasses. Due to a bug in Qt, aborted request doesnt always call this method
    virtual void requestFinished(int requestIdentifier, const QString &responseString);
    virtual void requestFailed(int requestIdentifier, QNetworkReply::NetworkError errorCode);
};

#endif // COMETCLIENT_H
