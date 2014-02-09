#ifndef TYPINGBOX_H
#define TYPINGBOX_H

#include <QTextBrowser>
#include <QKeyEvent>
#include <QTimer>

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
    void switchMode();

    void typingStarted();
    void typingStopped();
public slots:

private:
    QTimer *typingTimer;
    bool typingGoingOn;

private slots:
    void typingTimerFireAction();
    void textChanged();

};

#endif // TYPINGBOX_H
