#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDesktopWidget>
#include <QDebug>
#include <QtMultimedia/QSoundEffect>
#include <QLabel>
#include <QSound>
#include <QMediaPlayer>
#include <QLayout>
#include <QComboBox>
#include <QDockWidget>
#include <QPushButton>
#include <QAudioOutput>
#include <QRegExp>

#include "wavsound.h"
#include "russtranger.h"

#include "strangerprefswindow.h"

#include "cometclient.h"

const char SysMsgColor[] = "#888888";
const char StrangerColor[] = "#ff8888";
const char YouColor[] = "#8888ff";

#include <QAudioDeviceInfo>
#include <QAudio>

//For Linux Pulseaudio to play the wav files, they should be in Audio-CD format: 44.1 kHz, 16-bit, stereo
//for conversion use ffmpeg -i filename.wav -vn -acodec pcm_s16le -ar 44100 -f wav filename_cd.wav

void MainWindow::PlaySound() {


    //QSound *sound = new QSound("/home/mike/TediumRemedy/sample.wav", this);
    //sound->play();
    //return;

    sourceFile.setFileName("/home/mike/TediumRemedy/sample.wav");
    sourceFile.open(QIODevice::ReadOnly);

    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }



    return;


    /*QSound::play(":resources/connected.wav");
    return;
*/
    QList<QAudioDeviceInfo> l = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(QAudioDeviceInfo i, l) {
        qDebug() << i.supportedCodecs();
    }



    QTextStream out(stdout);
    out << "START-OUTPUT" << endl;
    QList<QAudioDeviceInfo> outputList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(QAudioDeviceInfo outputInfo, outputList) {
       out << outputInfo.deviceName() << endl;
    }
    out << "END-OUTPUT" << endl;
    return;

    /*QMediaPlayer *p  = new QMediaPlayer(0);
    p->setMedia(QMediaContent(QUrl::fromLocalFile(":/resources/connected.wav")));
    p->play();*/
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //    "([^"]+)"|(\S+)

    //    ""([^""]+)""|(\\S+)


    //WavSound *w = new WavSound(":/resources/connected.wav", 0);
    //w->play();
    //return;

    //PlaySound();
    //return;
    ui->setupUi(this);

    QApplication::setWindowIcon(QIcon(":/resources/Icon/RunningIcon.png"));

    strangerPrefsWindow = new StrangerPrefsWindow(this);

    //QDockWidget *drawdock = new QDockWidget(this,Qt::Dialog);
    //this->addDockWidget(Qt::RightDockWidgetArea,drawdock);

    /*CometClient *c = new CometClient(this);

    c->get("http://gggoogle.ru", 6);
    c->get("http://gggoogle.ru", 7);
    c->get("http://google.ru", 8);
    c->get("http://googleq.ru", 9);

    c->cancelAllRequests();
    return;
*/

    /*QHBoxLayout *dockLayout = new QHBoxLayout(this);
    QComboBox *b = new QComboBox(this);
    QTextEdit *t = new QTextEdit(this);
    dockLayout->addWidget(b);
    dockLayout->addWidget(t);
    dockLayout->setSizeConstraint(QLayout::SetMinimumSize);


    ui->dockWidgetContents_2->setLayout(dockLayout);
    */

    QWidget::setTabOrder(ui->typingBox, ui->chatlogBox);

    //do not allow Qt framework to intercept Tab keypress (we use it for switching modes)
    //ui->chatlogBox->setFocusPolicy(Qt::NoFocus);


    /*QFile stylesheetFile(":/resources/stylesheet.qss");
    if(!stylesheetFile.open(QFile::ReadOnly)) {
        qDebug() << "Error opening file " << stylesheetFile.error();
    }
    QString stylesheetString = QLatin1String(stylesheetFile.readAll());
    setStyleSheet(stylesheetString);
*/

    nightColoringMode = false;
    switchColoringMode(); //switch to night coloring mode, and apply it

    QStatusBar *sb = this->statusBar();
    chatModeLabel = new QLabel(this);
    //chatModeLabel = new QPushButton(this);
    //chatModeLabel->setFlat(true);

    sb->addPermanentWidget(chatModeLabel);
    typingLabel = new QLabel(this);
    sb->addPermanentWidget(typingLabel);

    //center the window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));


    receivedMessageSound = NULL;
    sentMessageSound = NULL;
    connectedSound = NULL;
    disconnectedSound = NULL;
    receivedMessageSound = new QSound(":/resources/sounds/shuffle.wav", this);
    //sentMessageSound = new QSound(":/resources/sounds/test.wav", this);
    connectedSound = new QSound(":/resources/sounds/click5.wav", this);
    disconnectedSound = new QSound(":/resources/sounds/phone_disc.wav", this);

    //receivedMessageSound->setObjectName();

//return;


    QObject::connect(ui->typingBox, SIGNAL(enterPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui->typingBox, SIGNAL(escapePressed()), this, SLOT(escapePressed()));    
    QObject::connect(ui->typingBox, SIGNAL(switchMode()), this, SLOT(SwitchMode()));
    QObject::connect(ui->typingBox, SIGNAL(typingStarted()), this, SLOT(TypingStarted()));
    QObject::connect(ui->typingBox, SIGNAL(typingStopped()), this, SLOT(TypingStopped()));
    QObject::connect(ui->typingBox, SIGNAL(showPreferences()), this, SLOT(displayStrangerPreferencesWindow()));
    QObject::connect(ui->typingBox, SIGNAL(switchColoringMode()), this, SLOT(switchColoringMode()));



    stranger = new Stranger(this);
    spy = new Spy(this);
    rusStranger = new RusStranger(this);

    //rusStranger->requestChatKey();
    //rusStranger->requestUid();





    QObject::connect(stranger, SIGNAL(ReceivedMessage(const QString &)), this, SLOT(ReceivedMessage(const QString &)));
    QObject::connect(stranger, SIGNAL(StrangerDisconnected()), this, SLOT(StrangerDisconnected()));
    QObject::connect(stranger, SIGNAL(ConversationStarted(QStringList, bool)), this, SLOT(StrangerConnected(QStringList, bool)));
    //QObject::connect(stranger, SIGNAL(ConversationStarted()), this, SLOT(StrangerConnected()));


    QObject::connect(stranger, SIGNAL(ConversationStartedWithQuestion(QString)), this, SLOT(StrangerConnectedWithQuestion(QString)));
    QObject::connect(stranger, SIGNAL(StrangerStartsTyping()), this, SLOT(StrangerStartsTyping()));
    QObject::connect(stranger, SIGNAL(StrangerStopsTyping()), this, SLOT(StrangerStopsTyping()));
    QObject::connect(stranger, SIGNAL(SystemMessage(const QString &)), this, SLOT(SystemMessage(const QString &)));
    QObject::connect(stranger, SIGNAL(WaitingForStranger()), this, SLOT(WaitingForStranger()));

    QObject::connect(spy, SIGNAL(ReceivedMessage(const QString &,const QString &)), this, SLOT(SpymodeReceivedMessage(const QString &,const QString &)));
    QObject::connect(spy, SIGNAL(StrangerDisconnected(const QString &)), this, SLOT(SpymodeStrangerDisconnected(const QString &)));
    QObject::connect(spy, SIGNAL(ConversationStarted()), this, SLOT(SpymodeStrangersConnected()));
    QObject::connect(spy, SIGNAL(ConversationStartedWithQuestion(QString)), this, SLOT(StrangerConnectedWithQuestion(QString)));
    QObject::connect(spy, SIGNAL(StrangerStartsTyping(QString)), this, SLOT(SpymodeStrangerStartsTyping(const QString &)));
    QObject::connect(spy, SIGNAL(StrangerStopsTyping()), this, SLOT(SpymodeStrangerStopsTyping(const QString &)));
    QObject::connect(spy, SIGNAL(SystemMessage(const QString &)), this, SLOT(SystemMessage(const QString &)));
    QObject::connect(spy, SIGNAL(WaitingForStranger()), this, SLOT(WaitingForStranger()));


    QObject::connect(rusStranger, SIGNAL(ReceivedMessage(const QString &)), this, SLOT(ReceivedMessage(const QString &)));
    QObject::connect(rusStranger, SIGNAL(StrangerDisconnected()), this, SLOT(StrangerDisconnected()));
    QObject::connect(rusStranger, SIGNAL(ConversationStarted()), this, SLOT(StrangerConnected()));
    QObject::connect(rusStranger, SIGNAL(StrangerStartsTyping()), this, SLOT(StrangerStartsTyping()));
    QObject::connect(rusStranger, SIGNAL(StrangerStopsTyping()), this, SLOT(StrangerStopsTyping()));
    QObject::connect(rusStranger, SIGNAL(WaitingForStranger()), this, SLOT(WaitingForStranger()));




    chatMode = Russian;
    SwitchMode(); //switch it to regular

    this->escapePressed();

    //strangerPrefsWindow->setModal(true);
    //strangerPrefsWindow->show();

}

void MainWindow::SystemMessage(const QString &message) {
     ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>"+message+"</font>");
}

void MainWindow::TypingStarted() {
    if(currentlyWorkingMode == Russian)
        rusStranger->StartTyping();
    else if(currentlyWorkingMode == AnsweringQuestions || chatMode == Regular)
        stranger->StartTyping();
}

void MainWindow::TypingStopped() {
    if(currentlyWorkingMode == Russian)
        rusStranger->StopTyping();
    else if(currentlyWorkingMode == AnsweringQuestions || chatMode == Regular)
        stranger->StopTyping();
}

void MainWindow::windowClosing() {
    qDebug() << "Closing";

    stranger->EndConversation();
    rusStranger->EndConversation();
    qDebug() << "Closed";
}

void MainWindow::WaitingForStranger() {
    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Waiting for stranger...</font>");
}

void MainWindow::displayStrangerPreferencesWindow()
{
    //if(chatMode == Regular) {
    strangerPrefsWindow->setParentCenterCoordinates(geometry().center());
    strangerPrefsWindow->show();
    //}
}

void MainWindow::SwitchMode() {
    if(chatMode==Regular) {
        chatMode = AnsweringQuestions;
        chatModeLabel->setText("Ans");
    } else if(chatMode==AnsweringQuestions) {
        chatMode = Spying;
        chatModeLabel->setText("Q");
    } else if(chatMode==Spying) {
        chatMode = Russian;
        chatModeLabel->setText("Russian");
    } else if(chatMode==Russian) {
        chatMode = Regular;
        chatModeLabel->setText("Regular");
    }
}

void MainWindow::switchColoringMode() {
    nightColoringMode = !nightColoringMode;
    QString stylesheetFilePath;

    if(nightColoringMode) {
        stylesheetFilePath = (":/resources/stylesheet.qss");
    } else {
        stylesheetFilePath = (":/resources/stylesheet_bright.qss");
    }

    QFile stylesheetFile(stylesheetFilePath);

    if(!stylesheetFile.open(QFile::ReadOnly)) {
        qDebug() << "Error opening file " << stylesheetFile.error();
    }
    QString stylesheetString = QLatin1String(stylesheetFile.readAll());
    setStyleSheet(stylesheetString);
    strangerPrefsWindow->setStyleSheet(stylesheetString);
}

void MainWindow::enterPressed() {
    //receivedMessageSound->play();
    QString messageText = ui->typingBox->toPlainText();
    ui->chatlogBox->append(QString("<font color='")+YouColor+"'><b>You: </b></font>"+messageText);
    ui->typingBox->clear();
    if(currentlyWorkingMode == Russian)
        rusStranger->SendMessage(messageText);
    else if(currentlyWorkingMode == Regular || currentlyWorkingMode == AnsweringQuestions)
        stranger->SendMessage(messageText);

    //sentMessageSound->play();
}

void MainWindow::escapePressed() {
    if(currentlyWorkingMode == Regular || currentlyWorkingMode==AnsweringQuestions)
        stranger->EndConversation();
    else if(currentlyWorkingMode == Spying)
        spy->EndConversation();
    else if(currentlyWorkingMode == Russian)
        rusStranger->EndConversation();

    ui->chatlogBox->clear();
    typingLabel->setText("");
    strangerTypingMask = 0;



    //spy->StartConversation(ui->typingBox->toPlainText());
    if(chatMode == Regular) {
        QString langSelector = "en";
        QString interestsString = strangerPrefsWindow->interestsString();
        qDebug() << interestsString;

        if(strangerPrefsWindow->languageSelector().length() > 0) {
            langSelector = strangerPrefsWindow->languageSelector();
        }

        if(langSelector!="en" || interestsString.length()>0) {
            chatModeLabel->setText("Regular*");
        } else {
            chatModeLabel->setText("Regular");
        }

        stranger->StartConversation(langSelector, interestsString, false);
    } else if(chatMode == Spying)
        spy->StartConversation(ui->typingBox->toPlainText());
    else if(chatMode == AnsweringQuestions)
        stranger->StartConversation("en", "", true);
    else if(chatMode == Russian)
        rusStranger->StartConversation();

    currentlyWorkingMode = chatMode;

}

void MainWindow::ReceivedMessage(const QString &messageText) {
    ui->chatlogBox->append(QString("<font color='")+StrangerColor+"'><b>Stranger: </b></font>"+messageText);
    if(receivedMessageSound && ShouldPlaySound())
        receivedMessageSound->play();

    typingLabel->setText("");
}

void MainWindow::StrangerDisconnected() {
    if(disconnectedSound && ShouldPlaySound())
        disconnectedSound->play();
    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Stranger disconnected</font>");
}

void MainWindow::StrangerConnected() {
    ui->chatlogBox->clear();

    if(connectedSound && ShouldPlaySound())
        connectedSound->play();

    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Stranger connected</font>");
    //connectedSound->play();
}

void MainWindow::StrangerConnected(QStringList interests, bool languageMatch) {
    ui->chatlogBox->clear();

    if(connectedSound && ShouldPlaySound())
        connectedSound->play();

    if(languageMatch) {
        ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Stranger connected</font>"+
                           "<font color='"+SysMsgColor+"'> ("+ strangerPrefsWindow->languageNameForSelector(strangerPrefsWindow->languageSelector()) +")</font>");
    } else if(interests.count() > 0) {
        ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Stranger connected</font>"+
                           "<font color='"+SysMsgColor+"'> ("+ interests.join(", ") +")</font>");

    } else {
        ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Stranger connected</font>");

    }
}

void MainWindow::StrangerConnectedWithQuestion(QString questionText) {
    ui->chatlogBox->clear();

    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>"+questionText+"</font>");
}

void MainWindow::StrangerStartsTyping() {
    typingLabel->setText("T");
}

void MainWindow::StrangerStopsTyping() {
    typingLabel->setText("");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QWidget *focusedWidget = QApplication::focusWidget();
    //let's not redirect Control/Alt modified key presses to the typing box (Ctrl+C for ex)
    //if(focusedWidget != ui->typingBox && ((event->key()==Qt::Key_Period && Qt::ControlModifier) || (!(event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier))))) {

    //qDebug() << event->nativeScanCode() << " and " << event->nativeVirtualKey();

    //66 and 65032 are capslock on my linux machine - i do not pass it to typing because its a language changing key in my system
    if(focusedWidget != ui->typingBox && event->nativeScanCode()!=66 && event->nativeVirtualKey()!=65032) {
        if((
                    (event->key()==Qt::Key_Period && (event->modifiers()&Qt::ControlModifier))
                  ||
                    (event->key()==Qt::Key_V && (event->modifiers()&Qt::ControlModifier))

                  ||(!(event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)))
          )) {

                ui->typingBox->setFocus();
                QKeyEvent *eventDuplicate = new QKeyEvent(event->type(), event->key(), event->modifiers(), event->text(), false, event->count());
                QCoreApplication::postEvent(ui->typingBox, eventDuplicate);
             }
    }
    //event->key();
    //qDebug() << "Inside MyWindow keypress";

}

//spy mode

void MainWindow::SpymodeReceivedMessage(const QString &strangerID, const QString &messageText){
    SpymodeStrangerStopsTyping(strangerID);
    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'><b>"+strangerID+": </b></font>"+messageText);
    //typingLabel->setText("");
}

void MainWindow::SpymodeStrangerDisconnected(const QString &strangerID) {
    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>"+strangerID+" disconnected</font>");
    strangerTypingMask = 0x00;
    typingLabel->setText("");
}

void MainWindow::SpymodeStrangersConnected() {
    ui->chatlogBox->clear();

    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Conversation started</font>");
}

void MainWindow::updateTypingLabelForSpymode() {
    if(strangerTypingMask == 0xF0)
        typingLabel->setText("1");
    else if(strangerTypingMask == 0x0F)
        typingLabel->setText("2");
    else if(strangerTypingMask == 0x00)
        typingLabel->setText("");
    else if(strangerTypingMask == 0xFF)
        typingLabel->setText("1&2");
    else
        typingLabel->setText("UnknownTyping"); //shouldnt be here
}

void MainWindow::SpymodeStrangerStartsTyping(const QString &strangerID) {
    //ui->chatlogBox->append("Stranger typing");
    if(strangerID == "Stranger 1") {
        strangerTypingMask |= 0xF0;
    } else if(strangerID == "Stranger 2") {
        strangerTypingMask |= 0x0F;
    }

    updateTypingLabelForSpymode();
    //typingLabel->setText("T");
}

void MainWindow::SpymodeStrangerStopsTyping(const QString &strangerID) {
    if(strangerID == "Stranger 1") {
        strangerTypingMask &= 0x0F;
    } else if(strangerID == "Stranger 2") {
        strangerTypingMask &= 0xF0;
    }

    updateTypingLabelForSpymode();


    //ui->chatlogBox->append("Stranger stopped typing");
}

bool MainWindow::ShouldPlaySound() {
    if(this->isActiveWindow()) {
        return false;
    } else {
        return true;
    }
}
