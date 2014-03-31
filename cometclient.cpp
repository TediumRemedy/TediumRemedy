#include "cometclient.h"
#include <QTimer>
#include <QEventLoop>

QString CometClient::decodeUnicode(const QString &unicodeCypher) {
    QString str = unicodeCypher;
    QRegExp rx("(\\\\u[0-9a-fA-F]{4})");
    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        str.replace(pos++, 6, QChar(rx.cap(1).right(4).toUShort(0, 16)));
    }
    return str;
}

CometClient::CometClient(QObject *parent) :
    QObject(parent) {

    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(urlRequestFinished(QNetworkReply*)));
}

void CometClient::post(const QString &requestUrlString, const QString &postDataString, int requestIdentifier) {
    QUrl requestUrl(requestUrlString);
    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setAttribute(QNetworkRequest::User, QVariant(requestIdentifier));
    QByteArray data;
    data.append(postDataString);
    QNetworkReply *reply = nam->post(request, data);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(urlRequestError(QNetworkReply::NetworkError)));
    requestsMade.insert(reply);
}

void CometClient::postSynchronously(const QString &requestUrlString, const QString &postDataString, int requestIdentifier, int timeoutMilliseconds) {
    QUrl requestUrl(requestUrlString);
    QNetworkRequest request(requestUrl);

    request.setAttribute(QNetworkRequest::User, QVariant(requestIdentifier));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray data;
    data.append(postDataString);
    QNetworkReply *reply = nam->post(request, data);
    requestsMade.insert(reply);

    QTimer timer; //the timer handles timeout event
    timer.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(timeoutMilliseconds);
    loop.exec();
    if(timer.isActive()) {
        timer.stop();
    } else {
        qDebug() << "postSynchronously() timeout occured";
        reply->abort();
    }
}

void CometClient::get(const QString &requestUrlString, int requestIdentifier) {
    QUrl requestUrl(requestUrlString);
    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setAttribute(QNetworkRequest::User, QVariant(requestIdentifier));
    QNetworkReply *reply = nam->get(request);
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(urlRequestError(QNetworkReply::NetworkError)));
    requestsMade.insert(reply);
}

void CometClient::cancelAllRequests() {
    foreach(QNetworkReply *reply, requestsMade) {
        qDebug("Aborting %d", reply->request().attribute(QNetworkRequest::User).toInt());
        reply->abort();
    }
}



void CometClient::urlRequestFinished(QNetworkReply *reply) {
    //this method isn't called for every url request made, some requests that have been aborted, do net emit finished() signal (bug), so there's a memory leak here.
    //Bugreport has been submitted: https://bugreports.qt-project.org/browse/QTBUG-37473

    int requestIdentifier = reply->request().attribute(QNetworkRequest::User).toInt();
    requestsMade.remove(reply);

    if(reply->error()) {
        qDebug("Error (Class: %s, requestIdentifier: %d): %s, errcode: %d", metaObject()->className(), requestIdentifier, reply->errorString().toStdString().c_str(), reply->error());
        reply->deleteLater();
        requestFailed(requestIdentifier, reply->error());
    } else { //no errors
        QByteArray replyData = reply->readAll();
        reply->deleteLater();

        QString replyText(replyData);
        requestFinished(requestIdentifier, replyText);
    }
}

void CometClient::urlRequestError(QNetworkReply::NetworkError code) {
    QNetworkReply *reply = dynamic_cast<QNetworkReply *>(QObject::sender());
    if(reply) { //just to be safe that the sender is actually QNetworkReply object
        qDebug("urlRequestError %d", code);
    }
}

void CometClient::requestFinished(int requestIdentifier, const QString &responseString) {
    qDebug() << "requestFinished - should be overridden in a subclass";
}

void CometClient::requestFailed(int requestIdentifier, QNetworkReply::NetworkError errorCode) {
    qDebug() << "requestFailed - should be overridden in a subclass";
}
