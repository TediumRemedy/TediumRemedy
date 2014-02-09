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
    RusStranger(QObject *parent = 0);

    void requestChatKey();
    void requestUid();
    void waitOpponentPoll();

    void setReady();
    void getIdentifier();

    void SendAction(QString actionName, QMap<QString, QString> params = QMap<QString, QString>());

private:
    enum RequestType {ErroneousType, RequestChatKey, RequestUid, RequestWaitOpponent, RequestSetReady, RequestGetIdentifier, RequestSendAction};

    QNetworkAccessManager *nam;

    QString chatKey;
    QString uid;
    QString cid;
    QString rpId;

private slots:
    void urlRequestFinished(QNetworkReply *reply);
    void waitOpponentTimerHandler();

signals:
    void ConversationStarted();
    void ReceivedMessage(const QString &messageText);
    void StrangerStartsTyping();
    void StrangerStopsTyping();
    void StrangerDisconnected();

public slots:
    void StartConversation();
    void EndConversation();
    void SendMessage(QString &messageText);
    void StartTyping();
    void StopTyping();
};

#endif // CVSTRANGER_H
