#include "spy.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Spy::Spy(QObject *parent) :
    CometClient(parent)
{

}

QString Spy::requestIdentifierToString(int requestType) {
    //enum RequestType {UnknownRequest, StartRequest, DisconnectRequest, SendMessageRequest, StartTypingRequest, StopTypingRequest, RequestPollEvents};
    char *requestTypeString[] = {"ErrorRequest", "StartRequest", "PollEventsRequest", "EndConversationRequest"};
    int stringsCount = sizeof(requestTypeString)/sizeof(char*);
    if(requestType < stringsCount)
        return QString(requestTypeString[requestType]);
    else
        return QString("requestTypeToString() error: int requestType exceeds size of requestTypeString array");
}

void Spy::requestFailed(int requestIdentifier, QNetworkReply::NetworkError errorCode) {

}

void Spy::StartConversation(QString questionToDiscuss) {
    EndConversation();

    this->post("http://front7.omegle.com/start?rcs=1&firstevents=1&spid=&randid=HNMESDAE&ask="+questionToDiscuss+"&lang=en", "", StartRequest);
}

void Spy::pollNewEvents() {
    post("http://front7.omegle.com/events", "id=" + QUrl::toPercentEncoding(clientID), PollEventsRequest);
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
    } else {
        //search for "connected" event
        bool isConnectedEvent = false;
        QString questionText;

        foreach(QJsonValue item, eventArray) {
            if(!item.isArray())
                continue;

            QJsonArray subArray = item.toArray(); //subarray is a small 1 or 2 element array, that contains things like ["connected"], ["serverMessage", "msg"]

            if(subArray.count() == 1 && subArray[0].isString()) {
                if(subArray[0].toString()=="connected") {
                    isConnectedEvent = true;
                } else if(subArray[0].toString() == "waiting") {
                    emit WaitingForStranger();
                }
            } else if(subArray.count() == 2 && subArray[0].isString()) {
                if(subArray[0].toString() == "question")
                    questionText = subArray[1].toString();
            }
        }

        if(isConnectedEvent) {
            emit ConversationStartedWithQuestion(questionText);
        }
    }

        /*else if(eventName == "connected") {
        const QString questionText = eventArray[0].toArray()[0].toString();
        if(eventArray[1].isArray() && eventArray[1].toArray()[0].toString() == "question")
            emit ConversationStartedWithQuestion(eventArray[1].toArray()[1].toString());
    }*/
    return true;
}

void Spy::EndConversation() {
    postSynchronously("http://front7.omegle.com/disconnect", "id="+QUrl::toPercentEncoding(clientID), EndConversationRequest);
    this->cancelAllRequests();
}

void Spy::requestFinished(int requestIdentifier, const QString &responseString) {
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(responseString.toUtf8(), &parseError);
    if(parseError.error != QJsonParseError::NoError)
        return; //error parsing json object


    qDebug() << document;

    if(document.isObject() && document.object().find("clientID")!=document.object().end()) {
        //we received a clientID
        clientID = document.object()["clientID"].toString();
        qDebug() << "Got client id: " << clientID;

        //process event - maybe its "connected" or "waiting"...
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
