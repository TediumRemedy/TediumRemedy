#include "typingbox.h"

TypingBox::TypingBox(QWidget *parent) :
    QTextBrowser(parent)
{
}

void TypingBox::keyPressEvent(QKeyEvent *event) {

    if(event->key()==Qt::Key_Return){
        emit enterPressed();
    } else if(event->key()==Qt::Key_Escape) {
        emit escapePressed();
    } else if(event->key()==Qt::Key_Z && event->modifiers()|Qt::Key_Control) {
        emit SwitchMode();
    } else {
       QTextBrowser::keyPressEvent(event);
    }

}
