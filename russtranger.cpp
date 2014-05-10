#include "russtranger.h"

#include <QtScript/QScriptEngine>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QEventLoop>
#include <QStringList>
#include <QMap>
#include <QSet>

/*
chatvdvoem.ru protocol
======================

GET /?identifier=1391629470.72621608176:113916278117fd2b3&ncrnd=1391629778011
Response:
96

[
  {
    "ids": { "113916278117fd2b3": "1391629773.5821591001" },
    "data": {"action":"start_chat","cid":"139162977397bbc3","online":2576}
  }
]
0



0) GET http://chatvdvoem.ru/key?_=1389605408955

   response:
   [obfurscated javascript code - see cv_key.js]


//1) POST http://chatvdvoem.ru/send
//   action=get_uid
//
//   response:
//   {"result":"ok","uid":"1389604140f7a87c","online":1292}

// in /send there can be following actions:
get_uid
start_typing
stop_typing
stop_chat
send_message, message=text
set_ready
wait_opponent
wait_new_opponent

2) POST send
   key=k13896054099090093305f4a-1-1&action=wait_opponent&uid=1389604140f7a87c

   response:
   wait opponent

3) POST send
   uid=1389604140f7a87c&cid=13896054099a4a23&key=k13896054099090093305f4a-1-1&action=set_ready

   response:
   {"action":"wait"}

//this is some kind of event polling mechanism (like Omegle's /events)
4) GET /?identifier=1389604573.78126196424:11389604140f7a87c&ncrnd=1389604573938

   response:
   [{
         "ids": { "11389604140f7a87c": "1389604575.93484661425" },
         "data": {"action":"get_ready","cid":"1389604141e43254","online":1292}
   }]

   sometimes its
       "data": {"action":"stop_typing","online":1320}\n
       "data": {"action":"new_message","user":"stranger","message":"\u043f\u0440\u0438\u0432\u0435\u0442","online":1320}\n
       "data": {"action":"start_typing","online":1320}\n
       "data": {"action":"start_chat","cid":"1389604573ea9eaf","online":1320}\n
       "data": {"action":"stop_chat","user":"stranger","online":1320}\n

5) POST /send
   action=start_typing&uid=1389604140f7a87c&cid=1389604573ea9eaf

   reponse:
   ok

6) POST /send
   message=hi&action=send_message&uid=1389604140f7a87c&cid=1389604573ea9eaf

   response:
   ?

7) POST /send
   action=stop_typing&uid=1389604140f7a87c&cid=1389604573ea9eaf

   response:
   ok

8) POST /send
   action=start_typing&uid=1389604140f7a87c&cid=1389604573ea9eaf

   response:
   ok

   POST /send
   message=%3A)&action=send_message&uid=1389604140f7a87c&cid=1389604573ea9eaf

   response:
   ok

   POST /send
   action=stop_typing&uid=1389604140f7a87c&cid=1389604573ea9eaf

   response:
   ok

*/

QString RusStranger::requestIdentifierToString(int requestType) {
    //enum RequestType {ErroneousType, RequestChatKey, RequestUid, RequestWaitOpponent, RequestSetReady, RequestGetIdentifier, RequestSendAction};
    char *requestTypeString[] = {"ErroneousType", "RequestChatKey", "RequestUid", "RequestWaitOpponent", "RequestSetReady", "RequestGetIdentifier", "RequestSendAction"};
    int stringsCount = sizeof(requestTypeString)/sizeof(char*);
    if(requestType < stringsCount)
        return QString(requestTypeString[requestType]);
    else
        return QString("requestTypeToString() error: int requestType exceeds size of requestTypeString array");
}

void RusStranger::StartConversation() {
    EndConversation();
    uid="";
    cid="";
    rpId="";
    chatKey="";
    chatIsGoingOn = false;

    this->requestChatKey();
    this->requestUid();
}

void RusStranger::EndConversation() {
    QString requestString("action=stop_chat");
    if(!uid.isEmpty())
        requestString.append("&uid="+uid);
    if(!cid.isEmpty())
        requestString.append("&cid="+cid);

    this->postSynchronously("http://chatvdvoem.ru/send", requestString, RequestSendAction);
    cancelAllRequests();
}

void RusStranger::SendMessage(QString &messageText) {
    QMap<QString, QString> messageParams;
    messageParams.insert("message", messageText);
    SendAction("send_message", messageParams);
}

void RusStranger::StartTyping() {
    SendAction("start_typing");
}

void RusStranger::StopTyping() {
    SendAction("stop_typing");
}


void RusStranger::SendAction(QString actionName, QMap<QString, QString> params) {
    QString requestString("action="+actionName);

    for(QMap<QString, QString>::Iterator i = params.begin(); i!=params.end(); i++) {
        requestString+="&"+QUrl::toPercentEncoding(i.key())+"="+QUrl::toPercentEncoding(i.value());
    }

    if(!uid.isEmpty())
        requestString.append("&uid="+uid);
    if(!cid.isEmpty())
        requestString.append("&cid="+cid);

    post("http://chatvdvoem.ru/send", requestString, RequestSendAction);
}

RusStranger::RusStranger(QObject *parent) {

}

void RusStranger::requestChatKey() {
    uint currentTimestamp = QDateTime::currentDateTime().toTime_t();
    get("http://chatvdvoem.ru/key?_="+QString::number(currentTimestamp), RequestChatKey);
}

void RusStranger::requestUid() {
    post("http://chatvdvoem.ru/send", "action=get_uid", RequestUid);
}

void RusStranger::waitOpponentPoll() {
    if(chatIsGoingOn) {
        qDebug() << "waitOpponentPoll: chatIsGoingOn==true, stop polling!";
        return;
    }
    /*if(!cid.isEmpty()) {
        qDebug() << "waitOpponentPoll: cid is not empty, stop polling!";
        return;
    }*/

    waitOpponentPollCounter++;

    post("http://chatvdvoem.ru/send", "action=wait_opponent&key="+chatKey+"&uid="+uid, RequestWaitOpponent);
}

void RusStranger::waitOpponentTimerHandler() {
    if(waitOpponentPollCounter >= 5) {
        qDebug("waitOpponentPollCounter is 5, giving up polling. Restarting chat");
        StartConversation();

    } else {
        waitOpponentPoll();
    }
}

void RusStranger::setReady() {
    //uid=13916278117fd2b3&cid=1391629390d08d64&key=k13916293891083602879b1c143-1-1-1-1-1-1-1&action=set_ready
    post("http://chatvdvoem.ru/send", "action=set_ready&key="+chatKey+"&uid="+uid+"&cid="+cid, RequestSetReady);
}

void RusStranger::getIdentifier() {
    //http://rp2.chatvdvoem.ru/?identifier=1391702235.98321740737:11391701805786a38&ncrnd=1391702241646
    //Referer: http://rp2.chatvdvoem.ru/?identifier=IFRAME&HOST=chatvdvoem.ru&version=1.32

    qDebug() << "Polling realplexor";
    if(uid.isEmpty())
        qDebug() << "getIdentifier: uid is empty";
    QString rpIdentifier = rpId.isEmpty() ? "1"+uid : rpId;

    get(QString("http://rp1.chatvdvoem.ru/?identifier=")+rpIdentifier+"&ncrnd="+QString::number(QDateTime::currentMSecsSinceEpoch()), RequestGetIdentifier);
}

QString decodeUnicode(QString unicodeCypher) {
    QString str = unicodeCypher;
    QRegExp rx("(\\\\u[0-9a-fA-F]{4})");
    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        str.replace(pos++, 6, QChar(rx.cap(1).right(4).toUShort(0, 16)));
    }
    return str;
}

void RusStranger::requestFinished(int requestIdentifier, const QString &responseString) {
    int requestType = requestIdentifier;
    const QString &replyText = responseString;
    if(requestType == RusStranger::RequestChatKey) {
            QScriptEngine jsInterpreter;
            QScriptValue returnVal = jsInterpreter.evaluate(replyText + "; chat_key");
            QString chatKeyString = returnVal.toString();

            qDebug() << chatKeyString;
            chatKey = chatKeyString;

            //requestUid();

    } else if(requestType == RusStranger::RequestUid) {
        //qDebug() << "Here's uid request reponse: " << replyText;

        QJsonParseError parseError;
        QJsonDocument document = QJsonDocument::fromJson(replyText.toUtf8(), &parseError);
        if(parseError.error != QJsonParseError::NoError)
            return; //error parsing json object


        if(document.isObject() && document.object().find("uid")!=document.object().end()) {
            //we received a clientID
            uid = document.object()["uid"].toString();
            qDebug() << "Got uid: " << uid;
            getIdentifier();

            emit WaitingForStranger();
            waitOpponentPollCounter = 0;
            waitOpponentPoll();
        }
    } else if(requestType == RusStranger::RequestWaitOpponent) {
        qDebug() << "Got wait opponent response: " << replyText;

        //if(cid.isEmpty()) {
        if(!chatIsGoingOn) {
            QTimer::singleShot(1000, this, SLOT(waitOpponentTimerHandler()));
        } else { //got cid, stop "waiting opponent", time to /send and /ping
            qDebug() << "Chat is already going on (we got start_chat somewhere). Stop polling";
            //qDebug() << "We have cid now: " << cid;
        }

    } else if(requestType == RusStranger::RequestGetIdentifier) {
        qDebug() << "Got get identifier response: " << replyText;

        QJsonParseError parseError;
        QJsonDocument document = QJsonDocument::fromJson(replyText.toUtf8(), &parseError);
        if(parseError.error != QJsonParseError::NoError) {
            qDebug() << "Error parsing json";
            return; //error parsing json object

        }

        //first saving new rpid
        if(document.isArray() && document.array()[0].isObject() && document.array()[0].toObject().find("ids")!=document.array()[0].toObject().end()) {
            //we received a realplexor ids. Do we need those?
            //qDebug() << "got RPlexor ids";
            QString idKey = document.array()[0].toObject()["ids"].toObject().keys()[0];
            rpId = document.array()[0].toObject()["ids"].toObject()[idKey].toString()+":"+idKey;
            //qDebug() << "rpid: " << rpId;
        }

        if(document.isArray() && document.array()[0].isObject() && document.array()[0].toObject().find("data")!=document.array()[0].toObject().end()) {
            QJsonValueRef actionValue = document.array()[0].toObject()["data"].toObject()["action"];
            QString actionString = actionValue.toString();

            if(actionString == "get_ready") {
                qDebug() << "This is get_ready";
                QJsonValueRef cidValue = document.array()[0].toObject()["data"].toObject()["cid"];
                if(cidValue.isString()) {
                    cid = cidValue.toString();
                    setReady();
                }
            } else if(actionString == "start_chat") {
                qDebug() << "This is start_chat";
                chatIsGoingOn = true;
                emit ConversationStarted();
            } else if(actionString == "start_typing") {
                qDebug() << "This is start_typing";
                emit StrangerStartsTyping();
            } else if(actionString == "stop_typing") {
                qDebug() << "This is stop_typing";
                emit StrangerStopsTyping();
            } else if(actionString == "new_message") {
                QString messageString = document.array()[0].toObject()["data"].toObject()["message"].toString();
                QString messageSender = document.array()[0].toObject()["data"].toObject()["user"].toString();

                qDebug() << "This is message: " << decodeUnicode(messageString);
                if(messageSender == "stranger") {
                    emit ReceivedMessage(decodeUnicode(messageString));
                }
            } else if(actionString == "stop_chat") {
                chatIsGoingOn = false;
                QString messageSender = document.array()[0].toObject()["data"].toObject()["user"].toString();
                if(messageSender == "stranger") {
                    qDebug() << "Stranger disconnected";
                    emit StrangerDisconnected();
                    return; //no need to getIdentifier() again. Coversation is over
                }
            } else if(actionString == "ping") {
                qDebug() << "Ping";
            } else {
                qDebug() << "Unknown action: " << actionString;
            }

            getIdentifier();
        }
    } else if(requestType == RusStranger::RequestSetReady) {
        qDebug () << "set_ready has been sent, got response: " << replyText;
    } else if(requestType == RusStranger::RequestSendAction) {
        qDebug() << "action sent: " << replyText;
    } else {
        qDebug() << "Unknown request type. Text: " << replyText;
    }
}

void RusStranger::requestFailed(int requestIdentifier, QNetworkReply::NetworkError errorCode) {
    //do nothing
}
