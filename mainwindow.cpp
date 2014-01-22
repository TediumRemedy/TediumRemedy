#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDesktopWidget>
#include <QDebug>
#include <QtMultimedia/QSoundEffect>
#include <QLabel>

#include "russtranger.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //RusStranger *rusStranger = new RusStranger;

    //rusStranger->requestChatKey();
    //rusStranger->requestUid();

    //return;

    ui->setupUi(this);
    QFile stylesheetFile("/home/mike/TediumRemedy/stylesheet.qss");
    if(!stylesheetFile.open(QFile::ReadOnly)) {

    }


    QStatusBar *sb = this->statusBar();

    chatModeLabel = new QLabel(this);
    //chatModeLabel->setText("R");
    sb->addPermanentWidget(chatModeLabel);


    /*QImage typingImagePng("/home/mike/TediumRemedy/typing.png");
    typingImagePng = typingImagePng.scaled(20,20,Qt::KeepAspectRatio);
    typingImage = new QLabel(this);
    typingImage->setPixmap(QPixmap::fromImage(typingImagePng));
    sb->addPermanentWidget(typingImage);*/

    typingLabel = new QLabel(this);
    //typingLabel->setText("1 & 2");
    sb->addPermanentWidget(typingLabel);



    //return;

    QString stylesheetString = QLatin1String(stylesheetFile.readAll());
    setStyleSheet(stylesheetString);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    QObject::connect(ui->typingBox, SIGNAL(enterPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui->typingBox, SIGNAL(escapePressed()), this, SLOT(escapePressed()));    
    QObject::connect(ui->typingBox, SIGNAL(SwitchMode()), this, SLOT(SwitchMode()));

    stranger = new Stranger(this);
    spy = new Spy(this);
    incomingMessageSound = new QSoundEffect(this);
    incomingMessageSound->setSource(QUrl("/home/mike/TediumRemedy/bell2.wav"));
    incomingMessageSound->play();

    QObject::connect(stranger, SIGNAL(ReceivedMessage(const QString &)), this, SLOT(ReceivedMessage(const QString &)));
    QObject::connect(stranger, SIGNAL(StrangerDisconnected()), this, SLOT(StrangerDisconnected()));
    QObject::connect(stranger, SIGNAL(ConversationStarted()), this, SLOT(StrangerConnected()));
    QObject::connect(stranger, SIGNAL(ConversationStartedWithQuestion(QString)), this, SLOT(StrangerConnectedWithQuestion(QString)));

    QObject::connect(stranger, SIGNAL(StrangerStartsTyping()), this, SLOT(StrangerStartsTyping()));
    QObject::connect(stranger, SIGNAL(StrangerStopsTyping()), this, SLOT(StrangerStopsTyping()));

    QObject::connect(spy, SIGNAL(ReceivedMessage(const QString &,const QString &)), this, SLOT(SpymodeReceivedMessage(const QString &,const QString &)));
    QObject::connect(spy, SIGNAL(StrangerDisconnected(const QString &)), this, SLOT(SpymodeStrangerDisconnected(const QString &)));
    QObject::connect(spy, SIGNAL(ConversationStarted()), this, SLOT(SpymodeStrangersConnected()));
    QObject::connect(spy, SIGNAL(ConversationStartedWithQuestion(QString)), this, SLOT(StrangerConnectedWithQuestion(QString)));
    QObject::connect(spy, SIGNAL(StrangerStartsTyping(QString)), this, SLOT(SpymodeStrangerStartsTyping(const QString &)));
    QObject::connect(spy, SIGNAL(StrangerStopsTyping()), this, SLOT(SpymodeStrangerStopsTyping(const QString &)));

    chatMode = Spying;
    SwitchMode(); //switch it to regular

    this->escapePressed();
    //stranger->StartConversation("en", "");
    //spy->StartConversation("This is a test question");

    //ui->statusBar->hide();

}

void MainWindow::SwitchMode() {
    if(chatMode==Regular) {
        chatMode = AnsweringQuestions;
        chatModeLabel->setText("Ans");
    }
    else if(chatMode==AnsweringQuestions) {
            chatMode = Spying;
        chatModeLabel->setText("Q");
    }
    else if(chatMode==Spying) {
            chatMode = Regular;
        chatModeLabel->setText("Regular");
}
}

void MainWindow::enterPressed() {
    QString messageText = ui->typingBox->toPlainText();
    ui->chatlogBox->append("<font color='#aaaacc'>You: </font>"+messageText);
    ui->typingBox->clear();
    stranger->SendMessage(messageText);
}

void MainWindow::escapePressed() {
    ui->chatlogBox->clear();
    typingLabel->setText("");
    //spy->StartConversation(ui->typingBox->toPlainText());
    if(chatMode == Regular)
        stranger->StartConversation("ru", "", false);
    else if(chatMode == Spying)
        spy->StartConversation(ui->typingBox->toPlainText());
    else if(chatMode == AnsweringQuestions)
            stranger->StartConversation("en", "", true);
}

void MainWindow::ReceivedMessage(const QString &messageText) {
    ui->chatlogBox->append("<font color='#ccaaaa'>Stranger: </font>"+messageText);
    incomingMessageSound->play();
    typingLabel->setText("");
}

void MainWindow::StrangerDisconnected() {
    ui->chatlogBox->append("<font color='#aaaaaa'>Stranger disconnected</font>");
}

void MainWindow::StrangerConnected() {

    ui->chatlogBox->append("<font color='#aaaaaa'>Stranger connected</font>");
}

void MainWindow::StrangerConnectedWithQuestion(QString questionText) {
    ui->chatlogBox->append("<font color='#aaaaaa'>"+questionText+"</font>");
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
    ui->chatlogBox->append(strangerID+": "+messageText);
    //typingLabel->setText("");
}

void MainWindow::SpymodeStrangerDisconnected(const QString &strangerID) {
    ui->chatlogBox->append(strangerID+" disconnected");
    strangerTypingMask = 0x00;
    typingLabel->setText("");
}

void MainWindow::SpymodeStrangersConnected() {
    ui->chatlogBox->append("Conversation started");
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


void MainWindow::PlaySoundFile(QString filename) {
    //QSoundEffect s()
    /*QFile inputFile;     // class member.
    static QAudioOutput* audio = NULL; // class member.
    inputFile.setFileName("/home/mike/TediumRemedy/bell.wav");
    inputFile.open(QIODevice::ReadOnly);

    QAudioFormat format;
    // Set up the format, eg.
    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        return;
    }

    if(!audio)
        audio = new QAudioOutput(format, this);

    //connect(audio,SIGNAL(stateChanged(QAudio::State)),SLOT(finishedPlaying(QAudio::State)));
    audio->start(&inputFile);*/
}
