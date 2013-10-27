#ifndef SPY_H
#define SPY_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>

class Spy : public QObject
{
    Q_OBJECT
public:
    explicit Spy(QObject *parent = 0);

signals:
    void ConversationStarted();
    void ReceivedMessage(const QString &strangerID, const QString &messageText);
    void StrangerStartsTyping(const QString &strangerID);
    void StrangerStopsTyping(const QString &strangerID);
    void StrangerDisconnected(const QString &strangerID);

public slots:
    void StartConversation(QString &questionToDiscuss);
    void EndConversation();

private:
    void pollNewEvents();
    bool processEvent(QJsonArray &commandWithArgs); //returns false if the conversation has ended

    QNetworkAccessManager *nam;
    QString clientID;

private slots:
    void urlRequestFinished(QNetworkReply *reply);
};

#endif // SPY_H
