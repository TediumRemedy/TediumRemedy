#include "intereststextedit.h"

InterestsTextEdit::InterestsTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
}

void InterestsTextEdit::keyPressEvent(QKeyEvent *event) {
  if(event->key() == Qt::Key_Return) {
    // optional: if the QPlainTextEdit should do its normal action
    // even when the return button is pressed, uncomment the following line
    // QPlainTextEdit::keyPressEvent( event )

    event->ignore();
  } else {
    QPlainTextEdit::keyPressEvent(event);
  }
}
