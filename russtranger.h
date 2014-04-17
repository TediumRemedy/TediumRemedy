#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QSet>
#include "cometclient.h"

#ifndef CVSTRANGER_H
#define CVSTRANGER_H

class RusStranger : public CometClient
{
    Q_OBJECT
public:

public:
    RusStranger(QObject *parent = 0);

    void requestChatKey();
    void requestUid();
    void waitOpponentPoll();

    void setReady();
    void getIdentifier();

    void SendAction(QString actionName, QMap<QString, QString> params = QMap<QString, QString>());

private:
    int waitOpponentPollCounter;

    enum RequestType {ErroneousType, RequestChatKey, RequestUid, RequestWaitOpponent, RequestSetReady, RequestGetIdentifier, RequestSendAction};

    QString chatKey;
    QString uid;
    QString cid;
    QString rpId;

    virtual void requestFinished(int requestIdentifier, const QString &responseString);
    virtual void requestFailed(int requestIdentifier, QNetworkReply::NetworkError errorCode);
    virtual QString requestIdentifierToString(int requestType);
private slots:
    //void urlRequestFinished(QNetworkReply *reply);
    void waitOpponentTimerHandler();

signals:
    void ConversationStarted();
    void ReceivedMessage(const QString &messageText);
    void StrangerStartsTyping();
    void StrangerStopsTyping();
    void StrangerDisconnected();
    void WaitingForStranger();

public slots:
    void StartConversation();
    void EndConversation();
    //void EndConversationSynchronously();
    void SendMessage(QString &messageText);
    void StartTyping();
    void StopTyping();
};

#endif // CVSTRANGER_H
