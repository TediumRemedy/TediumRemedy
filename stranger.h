#ifndef STRANGER_H
#define STRANGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class Stranger : public QObject
{
    Q_OBJECT
public:
    explicit Stranger(QObject *parent = 0);

signals:
    void ConversationStarted();
    void ReceivedMessage(QString *messageText);
    void StrangerStartsTyping();
    void StrangerStopsTyping();
    void StrangerDisconnected();

public slots:
    void StartConversation();
    void SendMessage(QString *messageText);
    void StartTyping();
    void StopTyping();

private:
    QNetworkAccessManager *nam;
    QString cliendID;

private slots:
    void urlRequestFinished(QNetworkReply *reply);
};

#endif // STRANGER_H
