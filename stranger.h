#ifndef STRANGER_H
#define STRANGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>

class Stranger : public QObject
{
    Q_OBJECT
public:
    explicit Stranger(QObject *parent = 0);

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

private:
    void pollNewEvents();
    bool processEvent(QJsonArray &commandWithArgs); //returns false if the conversation has ended

    QNetworkAccessManager *nam;
    QString clientID;

private slots:
    void urlRequestFinished(QNetworkReply *reply);
};

#endif // STRANGER_H
