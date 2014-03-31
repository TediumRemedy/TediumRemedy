#ifndef SPY_H
#define SPY_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>
#include "cometclient.h"

class Spy : public CometClient
{
    Q_OBJECT
public:
    explicit Spy(QObject *parent = 0);

signals:
    void ConversationStartedWithQuestion(QString question);
    void ReceivedMessage(const QString &strangerID, const QString &messageText);
    void StrangerStartsTyping(const QString &strangerID);
    void StrangerStopsTyping(const QString &strangerID);
    void StrangerDisconnected(const QString &strangerID);

public slots:
    void StartConversation(QString questionToDiscuss);
    void EndConversation();

private:
    enum RequestType {ErrorRequest, StartRequest, PollEventsRequest, EndConversationRequest};

    void pollNewEvents();
    bool processEvent(QJsonArray eventArray); //returns false if the conversation has ended

    QString clientID;

    virtual void requestFinished(int requestIdentifier, const QString &responseString);

};

#endif // SPY_H
