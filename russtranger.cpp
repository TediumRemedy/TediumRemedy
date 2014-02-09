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
#include <QStringList>
#include <QMap>

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


void RusStranger::StartConversation() {
    uid="";
    cid="";
    rpId="";
    chatKey="";

    this->requestChatKey();
    this->requestUid();
}

void RusStranger::EndConversation() {
    SendAction("stop_chat");
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
    QUrl requestUrl("http://chatvdvoem.ru/send");
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestSendAction));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString requestString("action="+actionName);

    for(QMap<QString, QString>::Iterator i = params.begin(); i!=params.end(); i++) {
        requestString+="&"+QUrl::toPercentEncoding(i.key())+"="+QUrl::toPercentEncoding(i.value());
    }

    if(!uid.isEmpty())
        requestString.append("&uid="+uid);
    if(!cid.isEmpty())
        requestString.append("&cid="+cid);

    QByteArray data;
    data.append(requestString);
    QNetworkReply *reply = nam->post(request, data);
}

RusStranger::RusStranger(QObject *parent) {
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(urlRequestFinished(QNetworkReply*)));
}

void RusStranger::requestChatKey() {
    uint currentTimestamp = QDateTime::currentDateTime().toTime_t();
    QUrl requestUrl("http://chatvdvoem.ru/key?_="+QString::number(currentTimestamp));
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestChatKey));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    //qDebug() << request.attribute(QNetworkRequest::User);

    //const QByteArray data;
    QNetworkReply *reply = nam->get(request);
}

void RusStranger::requestUid() {
    QUrl requestUrl("http://chatvdvoem.ru/send");
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestUid));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString requestString("action=get_uid");
    QByteArray data;
    data.append(requestString);
    QNetworkReply *reply = nam->post(request, data);

}

void RusStranger::waitOpponentPoll() {
    if(!cid.isEmpty())
        return;

    QUrl requestUrl("http://chatvdvoem.ru/send");
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestWaitOpponent));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString requestString("action=wait_opponent&key="+chatKey+"&uid="+uid);
    QByteArray data;
    data.append(requestString);
    QNetworkReply *reply = nam->post(request, data);
}

void RusStranger::waitOpponentTimerHandler() {
    waitOpponentPoll(); //...delete timer!!!
}

void RusStranger::setReady() {
    //uid=13916278117fd2b3&cid=1391629390d08d64&key=k13916293891083602879b1c143-1-1-1-1-1-1-1&action=set_ready
    QUrl requestUrl("http://chatvdvoem.ru/send");
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestSetReady));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString requestString("action=set_ready&key="+chatKey+"&uid="+uid+"&cid="+cid);
    QByteArray data;
    data.append(requestString);
    QNetworkReply *reply = nam->post(request, data);
}

void RusStranger::getIdentifier() {
    //http://rp2.chatvdvoem.ru/?identifier=1391702235.98321740737:11391701805786a38&ncrnd=1391702241646
    //Referer: http://rp2.chatvdvoem.ru/?identifier=IFRAME&HOST=chatvdvoem.ru&version=1.32

    /*static bool started = false;
    if(started)
        return;
    started = true;*/

    qDebug() << "Polling realplexor";
    if(uid.isEmpty())
        qDebug() << "getIdentifier: uid is empty";
    QString rpIdentifier = rpId.isEmpty() ? "1"+uid : rpId;
    QUrl requestUrl(QString("http://rp1.chatvdvoem.ru/?identifier=")+rpIdentifier+"&ncrnd="+QString::number(QDateTime::currentMSecsSinceEpoch()));
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestGetIdentifier));
    //request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = nam->get(request);
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

void RusStranger::urlRequestFinished(QNetworkReply *reply) {
    QByteArray replyData = reply->readAll();
    RusStranger::RequestType requestType = (RusStranger::RequestType)(reply->request().attribute(QNetworkRequest::User).toInt());
    //qDebug() << reply->request().attribute(QNetworkRequest::User);
    delete reply;

    QString replyText(replyData);

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
        QJsonDocument document = QJsonDocument::fromJson(replyData, &parseError);
        if(parseError.error != QJsonParseError::NoError)
            return; //error parsing json object


        if(document.isObject() && document.object().find("uid")!=document.object().end()) {
            //we received a clientID
            uid = document.object()["uid"].toString();
            qDebug() << "Got uid: " << uid;
            getIdentifier();
            waitOpponentPoll();
        }
    } else if(requestType == RusStranger::RequestWaitOpponent) {
        qDebug() << "Got wait opponent response: " << replyText;
        if(cid.isEmpty()) {
            QTimer::singleShot(1000, this, SLOT(waitOpponentTimerHandler()));
        } else { //got cid, stop "waiting opponent", time to /send and /ping
            qDebug() << "We have cid now: " << cid;
        }

    } else if(requestType == RusStranger::RequestGetIdentifier) {
        qDebug() << "Got get identifier response: " << replyText;

        QJsonParseError parseError;
        QJsonDocument document = QJsonDocument::fromJson(replyData, &parseError);
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
            //qDebug() << "\"data\" field present";
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
                qDebug() << "Stranger disconnected";
                emit StrangerDisconnected();
                return; //no need to getIdentifier() again. Coversation is over
            } else if(actionString == "ping") {
                qDebug() << "Ping";
            } else {
                qDebug() << "Unknown action: " << actionString;
            }

            getIdentifier();
        }


            //clientID = document.object()["clientID"].toString();
            //qDebug() << "Got client id: " << clientID;
            //pollNewEvents();



    } else if(requestType == RusStranger::RequestSetReady) {
        qDebug () << "set_ready has been sent, got response: " << replyText;
        //getIdentifier();
    } else if(requestType == RusStranger::RequestSendAction) {
        qDebug() << "action sent: " << replyText;
    } else {
        qDebug() << "Unknown request type. Text: " << replyText;
    }
}
