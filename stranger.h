#ifndef STRANGER_H
#define STRANGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>
#include "cometclient.h"

class Stranger : public CometClient
{
    Q_OBJECT
public:
    explicit Stranger(QObject *parent = 0);

signals:
    void ConversationStarted();
    void ConversationStarted(QStringList matchingInterests, bool foundLanguageMatch);
    void ConversationStartedWithQuestion(QString questionText);
    void ReceivedMessage(const QString &messageText);
    void StrangerStartsTyping();
    void StrangerStopsTyping();
    void StrangerDisconnected();
    void SystemMessage(const QString &message);
    void WaitingForStranger();

public slots:
    void StartConversation(const QString language, const QString topics, const bool wantSpy = false, const bool unmonitored = false);
    void EndConversation();
    void SendMessage(QString &messageText);
    void StartTyping();
    void StopTyping();

private:
    enum RequestType {UnknownRequest, StartRequest, DisconnectRequest, SendMessageRequest,
                      StartTypingRequest, StopTypingRequest, RequestPollEvents};

    QString requestIdentifierToString(int requestType);
    virtual void requestFailed(int requestIdentifier, QNetworkReply::NetworkError errorCode);


    void pollNewEvents();
    bool processEvent(QJsonArray eventArray); //returns false if the conversation has ended

    void processRecaptcha(QString recaptchaKey);

    QString clientID;

protected:
    void requestFinished(int requestIdentifier, const QString &responseString);
};

#endif // STRANGER_H
