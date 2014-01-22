#include "spy.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Spy::Spy(QObject *parent) :
    QObject(parent)
{
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(urlRequestFinished(QNetworkReply*)));
}

void Spy::StartConversation(QString questionToDiscuss) {
    EndConversation();

    QUrl requestUrl("http://front7.omegle.com/start?rcs=1&firstevents=1&spid=&randid=HNMESDAE&ask="+questionToDiscuss+"&lang=en");
    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    const QByteArray data;
    QNetworkReply *reply = nam->post(request, data);
}
void Spy::urlRequestFinished(QNetworkReply *reply) {
    QByteArray replyData = reply->readAll();
    delete reply;

    QString replyText(replyData);
    qDebug() << replyText;

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(replyData, &parseError);
    if(parseError.error != QJsonParseError::NoError)
        return; //error parsing json object


    if(document.isObject() && document.object().find("clientID")!=document.object().end()) {
        //we received a clientID
        clientID = document.object()["clientID"].toString();
        qDebug() << "Got client id: " << clientID;
        pollNewEvents();
    } else if(document.isArray() && !document.array().isEmpty()) {
        //first element of this array should be an array of [command, param1, ...]

        if(document.array()[0].isArray()) {
            QJsonArray commandWithArgs = document.array()[0].toArray();
            if(!commandWithArgs.isEmpty()) {
                if(processEvent(document.array()))
                    pollNewEvents();
            }
        }
    }
}

void Spy::pollNewEvents() {
    QUrl requestUrl("http://front7.omegle.com/events");
    QNetworkRequest request(requestUrl);
    const QByteArray data = QByteArray("id=" + QUrl::toPercentEncoding(clientID));
    QNetworkReply *reply = nam->post(request, data);
}

bool Spy::processEvent(QJsonArray eventArray) {
    QString eventName = eventArray[0].toArray()[0].toString();
    if(eventName == "spyDisconnected") {
        const QString strangerID = eventArray[0].toArray()[1].toString();
        emit StrangerDisconnected(strangerID);
        return false;
    } else if(eventName == "spyMessage") {
        const QString strangerID = eventArray[0].toArray()[1].toString();
        const QString messageText = eventArray[0].toArray()[2].toString();
        emit ReceivedMessage(strangerID, messageText);
    } else if(eventName == "spyTyping") {
        const QString strangerID = eventArray[0].toArray()[1].toString();
        emit StrangerStartsTyping(strangerID);
        //qDebug() << "*****stranger starts typing: \""<<strangerID<<"\"*****";
    } else if(eventName == "spyStoppedTyping") {
        const QString strangerID = eventArray[0].toArray()[1].toString();
        emit StrangerStopsTyping(strangerID);
    } else if(eventName == "connected") {
        const QString questionText = eventArray[0].toArray()[0].toString();
        if(eventArray[1].isArray() && eventArray[1].toArray()[0].toString() == "question")
            emit ConversationStartedWithQuestion(eventArray[1].toArray()[1].toString());
    }
    return true;
}

void Spy::EndConversation() {
    QUrl requestUrl("http://front7.omegle.com/disconnect");
    QNetworkRequest request(requestUrl);
    const QByteArray data = QByteArray("id="+QUrl::toPercentEncoding(clientID));
    QNetworkReply *reply = nam->post(request, data);
}
