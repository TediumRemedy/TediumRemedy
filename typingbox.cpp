#include "typingbox.h"
#include <QDebug>

TypingBox::TypingBox(QWidget *parent) :
    QTextBrowser(parent)
{
    typingTimer = new QTimer(this);
    typingTimer->setSingleShot(true);
    typingTimer->setInterval(1000);
    typingGoingOn = false;
    QObject::connect(typingTimer, SIGNAL(timeout()), this, SLOT(typingTimerFireAction()));
    QObject::connect(this, SIGNAL(textChanged()), this, SLOT(textChanged()));
}

void TypingBox::typingTimerFireAction() {
    typingGoingOn = false;
    emit typingStopped();
    //qDebug() << "Typing stopped";
}

void TypingBox::textChanged() {
    //maybe check if the key is alphanumeric...
    if(!typingGoingOn) {
        typingGoingOn = true;
        emit typingStarted();
        //qDebug() << "Typing started";
    } else {
        //do nothing
    }

    typingTimer->start(); //restart or start the timer
}

void TypingBox::keyPressEvent(QKeyEvent *event) {

    if(event->key()==Qt::Key_Return){
        emit enterPressed();
    } else if(event->key()==Qt::Key_Escape) {
        emit escapePressed();
    } else if(event->key()==Qt::Key_Tab) {
        emit switchMode();
    } else if(event->key()==Qt::Key_Period && (event->modifiers() & Qt::ControlModifier)) {
        emit showPreferences();
    } else if(event->key()==Qt::Key_Z && (event->modifiers() & Qt::ControlModifier)) {
        emit switchColoringMode();
    } else {
        QTextBrowser::keyPressEvent(event);
    }

}
