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
#include <QTimer>

/*
chatvdvoem.ru protocol
======================

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


RusStranger::RusStranger() {
    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(urlRequestFinished(QNetworkReply*)));


}

void RusStranger::requestChatKey() {
    uint currentTimestamp = QDateTime::currentDateTime().toTime_t();
    QUrl requestUrl("http://chatvdvoem.ru/key?_="+QString::number(currentTimestamp));
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestChatKey));
    //qDebug() << request.attribute(QNetworkRequest::User);

    //const QByteArray data;
    QNetworkReply *reply = nam->get(request);
}

void RusStranger::requestUid() {
    QUrl requestUrl("http://chatvdvoem.ru/send");
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestUid));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    QString requestString("action=get_uid");
    QByteArray data;
    data.append(requestString);
    QNetworkReply *reply = nam->post(request, data);

}

void RusStranger::waitOpponentPoll() {
    QUrl requestUrl("http://chatvdvoem.ru/send");
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant((int)RusStranger::RequestWaitOpponent));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    QString requestString("action=wait_opponent&key="+chatKey+"&uid="+uid);
    QByteArray data;
    data.append(requestString);
    QNetworkReply *reply = nam->post(request, data);
}

void RusStranger::waitOpponentTimerHandler() {
    waitOpponentPoll(); //...delete timer!!!

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
            waitOpponentPoll();
        }
    } else if(requestType == RusStranger::RequestWaitOpponent) {
        qDebug() << "Got wait opponent response: " << replyText;
        QTimer *waitOpponentTimer = new QTimer(this);
        waitOpponentTimer->setInterval(1000);
        waitOpponentTimer->setSingleShot(true);
        QObject::connect(waitOpponentTimer, SIGNAL(timeout()), this, SLOT(waitOpponentTimerHandler()));
        waitOpponentTimer->start();
    }

    else {
            qDebug() << "Unknown request type";
    }
}
