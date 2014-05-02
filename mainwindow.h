#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*

  Convenient qtcreator shortcuts:

  Ctrl+K - quick project file search
  Alt+Enter - generate implementation/declaration
  F4 - switch between h/cpp
  F2 - jump to declaration

*/

#include "strangerprefswindow.h"

#include <QMainWindow>
#include "stranger.h"
#include "spy.h"
#include "russtranger.h"
#include <QtMultimedia/QSoundEffect>
#include <QLabel>
#include <QSound>
#include <QAudio>
#include <QFile>
#include <QAudioOutput>

#include "wavsound.h"

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

    StrangerPrefsWindow *strangerPrefsWindow;

    enum ChatMode {Regular, Spying, AnsweringQuestions, Russian};
    ChatMode chatMode;
    ChatMode currentlyWorkingMode;

    QLabel *typingImage;
    QLabel *typingLabel;
    //QPushButton *chatModeLabel;
    QLabel *chatModeLabel;

    bool nightColoringMode;

public slots:
    void enterPressed();
    void escapePressed();
    void SwitchMode();
    void switchColoringMode();
    void TypingStarted();
    void TypingStopped();

    void windowClosing();
    void displayStrangerPreferencesWindow();

    void WaitingForStranger();
private:
    void updateTypingLabelForSpymode();

    Stranger *stranger;
    Spy *spy;
    RusStranger *rusStranger;


    //QSound has problem with pulseaudio
    //PulseAudio should be in a "non flat-volumes" mode to work with normal volume (not setting system volume to 100% everytime)
    //https://wiki.archlinux.org/index.php/PulseAudio#Clients_alter_master_output_volume_.28a.k.a._volume_jumps_to_100.25_after_running_application.29
    QSound *receivedMessageSound;
    QSound *sentMessageSound;
    QSound *disconnectedSound;
    QSound *connectedSound;


    unsigned char strangerTypingMask; //00 - no one typing, f0 - first typing, 0f - second typing, ff - both typing

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void ReceivedMessage(const QString &messageText);
    void StrangerDisconnected();
    void StrangerConnected();
    void StrangerConnected(QStringList interests, bool languageMatch);
    void StrangerConnectedWithQuestion(QString questionText);
    void StrangerStartsTyping();
    void StrangerStopsTyping();
    void SystemMessage(const QString &message);

    //spy
    void SpymodeReceivedMessage(const QString &strangerID, const QString &messageText);
    void SpymodeStrangerDisconnected(const QString &strangerID);
    void SpymodeStrangersConnected();
    void SpymodeStrangerStartsTyping(const QString &strangerID);
    void SpymodeStrangerStopsTyping(const QString &strangerID);

private:
    void PlaySoundFile(QString filename);

    QFile sourceFile;   // class member.
    QAudioOutput* audio; // class member.
    void PlaySound();
};

#endif // MAINWINDOW_H
