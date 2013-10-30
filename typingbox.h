#ifndef TYPINGBOX_H
#define TYPINGBOX_H

#include <QTextBrowser>
#include <QKeyEvent>

class TypingBox : public QTextBrowser
{
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event);

public:
    explicit TypingBox(QWidget *parent = 0);

signals:
    void enterPressed();
    void escapePressed();
    void SwitchMode();
public slots:

};

#endif // TYPINGBOX_H
