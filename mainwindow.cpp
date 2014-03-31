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

#include "russtranger.h"

#include "cometclient.h"

#include <phonon4qt5/phonon/AudioOutput>
#include <phonon4qt5/phonon/MediaObject>
#include <phonon4qt5/phonon/MediaSource>

const char SysMsgColor[] = "#888888";
const char StrangerColor[] = "#ff8888";
const char YouColor[] = "#8888ff";

#include <QAudioDeviceInfo>
#include <QAudio>

void PlaySound() {
    QSound::play("/home/mike/TediumRemedy/connected.wav");
return;

    Phonon::MediaObject mo;
    Phonon::AudioOutput ao;
    createPath(&mo,&ao);
    mo.setCurrentSource(Phonon::MediaSource("/home/mike/TediumRemedy/connected1.wav")); // insert your multimedia file here
    mo.play();
    //return;

    QTextStream out(stdout);
    out << "START-OUTPUT" << endl;
    QList<QAudioDeviceInfo> outputList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(QAudioDeviceInfo outputInfo, outputList) {
       out << outputInfo.deviceName() << endl;
    }
    out << "END-OUTPUT" << endl;

    QMediaPlayer *p = new QMediaPlayer(0);
    p->setMedia(QMediaContent(QUrl::fromLocalFile("/home/mike/TediumRemedy/connected.wav")));
    p->play();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //PlaySound();
    //return;
    ui->setupUi(this);


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


    QFile stylesheetFile(":/resources/stylesheet.qss");
    if(!stylesheetFile.open(QFile::ReadOnly)) {
        qDebug() << "Error opening file " << stylesheetFile.error();
    }
    QString stylesheetString = QLatin1String(stylesheetFile.readAll());
    setStyleSheet(stylesheetString);

    QStatusBar *sb = this->statusBar();
    chatModeLabel = new QLabel(this);
    sb->addPermanentWidget(chatModeLabel);
    typingLabel = new QLabel(this);
    sb->addPermanentWidget(typingLabel);

    //center the window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));



    QObject::connect(ui->typingBox, SIGNAL(enterPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui->typingBox, SIGNAL(escapePressed()), this, SLOT(escapePressed()));    
    QObject::connect(ui->typingBox, SIGNAL(switchMode()), this, SLOT(SwitchMode()));
    QObject::connect(ui->typingBox, SIGNAL(typingStarted()), this, SLOT(TypingStarted()));
    QObject::connect(ui->typingBox, SIGNAL(typingStopped()), this, SLOT(TypingStopped()));



    stranger = new Stranger(this);
    spy = new Spy(this);
    rusStranger = new RusStranger(this);

    //rusStranger->requestChatKey();
    //rusStranger->requestUid();


    //receivedMessageSound = new QSound(":/resources/received_message.wav", this);
    //sentMessageSound = new QSound(":/resources/sent_message.wav", this);
    //connectedSound = new QSound(":/resources/connected.wav", this);
    //disconnectedSound = new QSound(":/resources/disconnected.wav", this);

    //QSound *testSound = new QSound("/usr/share/sounds/KDE_Logout_new.wav");
    //testSound->play();

    QObject::connect(stranger, SIGNAL(ReceivedMessage(const QString &)), this, SLOT(ReceivedMessage(const QString &)));
    QObject::connect(stranger, SIGNAL(StrangerDisconnected()), this, SLOT(StrangerDisconnected()));
    QObject::connect(stranger, SIGNAL(ConversationStarted()), this, SLOT(StrangerConnected()));
    QObject::connect(stranger, SIGNAL(ConversationStartedWithQuestion(QString)), this, SLOT(StrangerConnectedWithQuestion(QString)));
    QObject::connect(stranger, SIGNAL(StrangerStartsTyping()), this, SLOT(StrangerStartsTyping()));
    QObject::connect(stranger, SIGNAL(StrangerStopsTyping()), this, SLOT(StrangerStopsTyping()));
    QObject::connect(stranger, SIGNAL(SystemMessage(const QString &)), this, SLOT(SystemMessage(const QString &)));


    QObject::connect(spy, SIGNAL(ReceivedMessage(const QString &,const QString &)), this, SLOT(SpymodeReceivedMessage(const QString &,const QString &)));
    QObject::connect(spy, SIGNAL(StrangerDisconnected(const QString &)), this, SLOT(SpymodeStrangerDisconnected(const QString &)));
    QObject::connect(spy, SIGNAL(ConversationStarted()), this, SLOT(SpymodeStrangersConnected()));
    QObject::connect(spy, SIGNAL(ConversationStartedWithQuestion(QString)), this, SLOT(StrangerConnectedWithQuestion(QString)));
    QObject::connect(spy, SIGNAL(StrangerStartsTyping(QString)), this, SLOT(SpymodeStrangerStartsTyping(const QString &)));
    QObject::connect(spy, SIGNAL(StrangerStopsTyping()), this, SLOT(SpymodeStrangerStopsTyping(const QString &)));
    QObject::connect(spy, SIGNAL(SystemMessage(const QString &)), this, SLOT(SystemMessage(const QString &)));

    QObject::connect(rusStranger, SIGNAL(ReceivedMessage(const QString &)), this, SLOT(ReceivedMessage(const QString &)));
    QObject::connect(rusStranger, SIGNAL(StrangerDisconnected()), this, SLOT(StrangerDisconnected()));
    QObject::connect(rusStranger, SIGNAL(ConversationStarted()), this, SLOT(StrangerConnected()));
    QObject::connect(rusStranger, SIGNAL(StrangerStartsTyping()), this, SLOT(StrangerStartsTyping()));
    QObject::connect(rusStranger, SIGNAL(StrangerStopsTyping()), this, SLOT(StrangerStopsTyping()));




    chatMode = Russian;
    SwitchMode(); //switch it to regular

    this->escapePressed();
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

void MainWindow::enterPressed() {
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
    ui->chatlogBox->clear();
    typingLabel->setText("");
    strangerTypingMask = 0;

    stranger->EndConversation();
    spy->EndConversation();
    rusStranger->EndConversation();

    //spy->StartConversation(ui->typingBox->toPlainText());
    if(chatMode == Regular)
        stranger->StartConversation("en", "", false);
    else if(chatMode == Spying)
        spy->StartConversation(ui->typingBox->toPlainText());
    else if(chatMode == AnsweringQuestions)
        stranger->StartConversation("en", "", true);
    else if(chatMode == Russian)
        rusStranger->StartConversation();

    currentlyWorkingMode = chatMode;

}

void MainWindow::ReceivedMessage(const QString &messageText) {
    ui->chatlogBox->append(QString("<font color='")+StrangerColor+"'><b>Stranger: </b></font>"+messageText);
    //receivedMessageSound->play();
    typingLabel->setText("");
}

void MainWindow::StrangerDisconnected() {
    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Stranger disconnected</font>");
    //disconnectedSound->play();
}

void MainWindow::StrangerConnected() {

    ui->chatlogBox->append(QString("<font color='")+SysMsgColor+"'>Stranger connected</font>");
    //connectedSound->play();
}

void MainWindow::StrangerConnectedWithQuestion(QString questionText) {
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
    if(focusedWidget != ui->typingBox && !(event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier))) {
        ui->typingBox->setFocus();
        QKeyEvent *eventDuplicate = new QKeyEvent(event->type(), event->key(), event->modifiers(), event->text(), false, event->count());
        QCoreApplication::postEvent(ui->typingBox, eventDuplicate);
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

