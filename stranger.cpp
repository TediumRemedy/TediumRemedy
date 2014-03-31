#include "stranger.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QEventLoop>

Stranger::Stranger(QObject *parent) :
    CometClient(parent)
{

}

void Stranger::StartConversation(const QString language, const QString topics, const bool wantSpy, const bool unmonitored) {
    EndConversation();

    //QString language="en";
    QString requestUrlString = "http://front2.omegle.com/start?rcs=1&firstevents=1&spid=&randid=MG6PZ6ZP&lang="+
            language;
    //QString topics="\"games\",\"music\"";
    if(!topics.isEmpty())
        requestUrlString+="&topics="+QUrl::toPercentEncoding("["+topics+"]");

    if(wantSpy)
        requestUrlString+="&wantsspy=1";

    if(unmonitored)
        requestUrlString+="&group=unmon";

    post(requestUrlString, "", StartRequest);
}

void Stranger::EndConversation() {
    //post("http://front2.omegle.com/disconnect", "id="+QUrl::toPercentEncoding(clientID), DisconnectRequest);
    this->postSynchronously("http://front2.omegle.com/disconnect", "id="+QUrl::toPercentEncoding(clientID), DisconnectRequest);
    cancelAllRequests();
}

void Stranger::SendMessage(QString &messageText) {
    post("http://front2.omegle.com/send", "msg=" + QUrl::toPercentEncoding(messageText) +"&id="+QUrl::toPercentEncoding(clientID), SendMessageRequest);
}

void Stranger::StartTyping() {
    post("http://front2.omegle.com/typing", "id="+QUrl::toPercentEncoding(clientID), StartTypingRequest);
}

void Stranger::StopTyping() {
    post("http://front2.omegle.com/stoppedtyping", "id="+QUrl::toPercentEncoding(clientID), StopTypingRequest);
}

void Stranger::requestFinished(int requestIdentifier, const QString &responseString) {
    QString replyText(responseString);
    //qDebug() << replyText;

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(replyText.toUtf8(), &parseError);
    if(parseError.error != QJsonParseError::NoError)
        return; //error parsing json object


    if(document.isObject() && document.object().find("clientID")!=document.object().end()) {
        //we received a clientID
        clientID = document.object()["clientID"].toString();
        qDebug() << "Got client id: " << clientID;

        //process event - maybe its "connected"...
        if(document.object().find("events") != document.object().end()) {
            QJsonArray events = document.object()["events"].toArray();
            //qDebug() << "AAAA: " << events[1].toArray()[0].toString();
            processEvent(events);
        }

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

void Stranger::pollNewEvents() {
    post("http://front2.omegle.com/events", "id=" + QUrl::toPercentEncoding(clientID), RequestPollEvents);
}

bool Stranger::processEvent(QJsonArray eventArray) {
    QString eventName = eventArray[0].toArray()[0].toString();

    if(eventName == "strangerDisconnected") {
        emit StrangerDisconnected();
        return false;
    } else if(eventName == "gotMessage") {
        const QString messageText = eventArray[0].toArray()[1].toString();
        emit ReceivedMessage(messageText);
    } else if(eventName == "typing") {
        emit StrangerStartsTyping();
    } else if(eventName == "stoppedTyping") {
        emit StrangerStopsTyping();
    } else if(eventName == "antinudeBanned") {
        emit SystemMessage(QString("Banned, going to unmonitored section"));
        qDebug() << "We are banned, going to unmonitored";
        this->StartConversation("en", "", false, true);
    } else {
        //search for "connected" event
        for(int i=0; i<eventArray.count(); i++) {
            //qDebug() << "Looking for cnt: " << eventArray[0].toArray()[i].toString();
            if(eventArray[i].toArray()[0].toString() == "connected") {
                if(eventArray[i+1].isArray() && eventArray[i+1].toArray()[0].toString() == "question")
                    emit ConversationStartedWithQuestion(eventArray[1].toArray()[1].toString());
                else
                    emit ConversationStarted();
            }
        }
    }
    return true;
}
