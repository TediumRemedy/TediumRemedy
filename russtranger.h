#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#ifndef CVSTRANGER_H
#define CVSTRANGER_H

class RusStranger : public QObject
{
    Q_OBJECT
public:

public:
    RusStranger();

    void requestChatKey();
    void requestUid();
    void waitOpponentPoll();

private:
    enum RequestType {ErroneousType, RequestChatKey, RequestUid, RequestWaitOpponent};

    QNetworkAccessManager *nam;

    QString chatKey;
    QString uid;
    QString cid;

private slots:
    void urlRequestFinished(QNetworkReply *reply);
    void waitOpponentTimerHandler();
};

#endif // CVSTRANGER_H
