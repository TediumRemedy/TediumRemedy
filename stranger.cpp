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
    if(!topics.isEmpty()) {
        requestUrlString+="&topics="+QUrl::toPercentEncoding("["+topics+"]");
    }

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

QString Stranger::requestIdentifierToString(int requestType) {
    //enum RequestType {UnknownRequest, StartRequest, DisconnectRequest, SendMessageRequest, StartTypingRequest, StopTypingRequest, RequestPollEvents};
    char *requestTypeString[] = {"UnknownRequest", "StartRequest", "DisconnectRequest", "SendMessageRequest", "StartTypingRequest", "StopTypingRequest", "RequestPollEvents"};
    int stringsCount = sizeof(requestTypeString)/sizeof(char*);
    if(requestType < stringsCount)
        return QString(requestTypeString[requestType]);
    else
        return QString("requestTypeToString() error: int requestType exceeds size of requestTypeString array");
}

void Stranger::requestFailed(int requestIdentifier, QNetworkReply::NetworkError errorCode)
{

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
    } else if(eventName == "recaptchaRequired") {
        const QString recaptchaCode = eventArray[0].toArray()[1].toString();
        qDebug() << "Recaptcha required with code " << recaptchaCode;
        emit SystemMessage(QString("Captcha verification required"));
        this->processRecaptcha(recaptchaCode);
    } else {
        //search for "connected" event
        bool isConnectedEvent = false;
        bool isFoundLanguageMatch = false;
        QStringList matchingInterests;
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
                QString firstPart = subArray[0].toString();
                QJsonValue secondPart = subArray[1];

                if(firstPart == "serverMessage") {
                    if(secondPart.isString() && secondPart.toString()=="You both speak the same language.") {
                        isFoundLanguageMatch = true;
                    }
                } else if(firstPart == "question") {
                    questionText = secondPart.toString();
                } else if(firstPart == "commonLikes") {
                    if(secondPart.isArray()) {
                        foreach(QJsonValue like, secondPart.toArray()) {
                            matchingInterests.append(like.toString());
                        }
                    }
                }
            }
        }

        if(isConnectedEvent && questionText.isEmpty()) {
            emit ConversationStarted(matchingInterests, isFoundLanguageMatch);
        } else if(isConnectedEvent) {
            emit ConversationStartedWithQuestion(questionText);
        }

    }
    return true;
}

void Stranger::processRecaptcha(QString recaptchaKey) {
    // > GET google.com/recaptcha/api/challenge?k=<RECAPTCHA KEY>
    // < text/javascript, var RecaptchaState = {challenge : '<...>', timeout: 1800, ...} document.write('..');
    // > GET google.com/recaptcha/api/reload?c=<...>
    // < text/javascript, Recaptcha.finish_reload('<...>');
    // > POST front5.omegle.com/recaptcha
    //    challenge=<...>&response=<ANSWER HERE>&id=<URL ENCODED CONVO ID>
    // < text/plain win


    //and we should get "connected event" for the POST /events that had previously been submitted
}
