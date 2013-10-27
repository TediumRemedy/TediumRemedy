#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDesktopWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile stylesheetFile("/home/mike/TediumRemedy/stylesheet.qss");
    if(!stylesheetFile.open(QFile::ReadOnly)) {

    }
    QString stylesheetString = QLatin1String(stylesheetFile.readAll());
    setStyleSheet(stylesheetString);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    QObject::connect(ui->typingBox, SIGNAL(enterPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui->typingBox, SIGNAL(escapePressed()), this, SLOT(escapePressed()));

    stranger = new Stranger(this);
    spy = new Spy(this);

    QObject::connect(stranger, SIGNAL(ReceivedMessage(const QString &)), this, SLOT(ReceivedMessage(const QString &)));
    QObject::connect(stranger, SIGNAL(StrangerDisconnected()), this, SLOT(StrangerDisconnected()));
    QObject::connect(stranger, SIGNAL(ConversationStarted()), this, SLOT(StrangerConnected()));
    QObject::connect(stranger, SIGNAL(StrangerStartsTyping()), this, SLOT(StrangerStartsTyping()));
    QObject::connect(stranger, SIGNAL(StrangerStopsTyping()), this, SLOT(StrangerStopsTyping()));

    QObject::connect(spy, SIGNAL(ReceivedMessage(const QString &,const QString &)), this, SLOT(SpymodeReceivedMessage(const QString &,const QString &)));
    QObject::connect(spy, SIGNAL(StrangerDisconnected(const QString &)), this, SLOT(SpymodeStrangerDisconnected(const QString &)));
    QObject::connect(spy, SIGNAL(ConversationStarted()), this, SLOT(SpymodeStrangersConnected()));
    QObject::connect(spy, SIGNAL(StrangerStartsTyping()), this, SLOT(SpymodeStrangerStartsTyping(const QString &)));
    QObject::connect(spy, SIGNAL(StrangerStopsTyping()), this, SLOT(SpymodeStrangerStopsTyping(const QString &)));


    //stranger->StartConversation();
    spy->StartConversation("I don't like you");
}

void MainWindow::enterPressed() {
    QString messageText = ui->typingBox->toPlainText();
    ui->chatlogBox->append("You: "+messageText);
    ui->typingBox->clear();
    stranger->SendMessage(messageText);
}

void MainWindow::escapePressed() {
    ui->chatlogBox->clear();
    spy->StartConversation("grrrrrrrrrrrrrrrrrr");
    //stranger->StartConversation();
}

void MainWindow::ReceivedMessage(const QString &messageText) {
    ui->chatlogBox->append("Stranger: "+messageText);
}

void MainWindow::StrangerDisconnected() {
    ui->chatlogBox->append("Stranger disconnected");
}

void MainWindow::StrangerConnected() {

    ui->chatlogBox->append("Stranger connected");
}

void MainWindow::StrangerStartsTyping() {
    ui->chatlogBox->append("Stranger typing");
}

void MainWindow::StrangerStopsTyping() {
    ui->chatlogBox->append("Stranger stopped typing");
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
    qDebug() << "Inside MyWindow keypress";

}

//spy mode

void MainWindow::SpymodeReceivedMessage(const QString &strangerID, const QString &messageText){
    ui->chatlogBox->append(strangerID+": "+messageText);
}

void MainWindow::SpymodeStrangerDisconnected(const QString &strangerID) {
    ui->chatlogBox->append(strangerID+" disconnected");
}

void MainWindow::SpymodeStrangersConnected() {
    ui->chatlogBox->append("Conversation started");
}

void MainWindow::SpymodeStrangerStartsTyping(const QString &strangerID) {
    ui->chatlogBox->append(strangerID+" types");
}

void MainWindow::SpymodeStrangerStopsTyping(const QString &strangerID) {

}
