#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stranger.h"
#include "spy.h"
#include <QtMultimedia/QSoundEffect>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    enum ChatMode {Regular, Spying, AnsweringQuestions};
    ChatMode chatMode;

    QLabel *typingImage;
    QLabel *typingLabel;
    QLabel *chatModeLabel;

public slots:
    void enterPressed();
    void escapePressed();
void SwitchMode();

private:
    void updateTypingLabelForSpymode();

    Stranger *stranger;
    Spy *spy;
    QSoundEffect *incomingMessageSound;

    unsigned char strangerTypingMask; //00 - no one typing, f0 - first typing, 0f - second typing, ff - both typing

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void ReceivedMessage(const QString &messageText);
    void StrangerDisconnected();
    void StrangerConnected();
    void StrangerConnectedWithQuestion(QString questionText);
    void StrangerStartsTyping();
    void StrangerStopsTyping();

    //spy
    void SpymodeReceivedMessage(const QString &strangerID, const QString &messageText);
    void SpymodeStrangerDisconnected(const QString &strangerID);
    void SpymodeStrangersConnected();
    void SpymodeStrangerStartsTyping(const QString &strangerID);
    void SpymodeStrangerStopsTyping(const QString &strangerID);

private:
    void PlaySoundFile(QString filename);

};

#endif // MAINWINDOW_H
