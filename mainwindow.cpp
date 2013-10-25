#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDesktopWidget>

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
    QObject::connect(ui->textBrowser_2, SIGNAL(enterPressed()), this, SLOT(enterPressed()));
    QObject::connect(ui->textBrowser_2, SIGNAL(escapePressed()), this, SLOT(escapePressed()));

    stranger = new Stranger(this);

    QObject::connect(stranger, SIGNAL(ReceivedMessage(const QString &)), this, SLOT(ReceivedMessage(const QString &)));
    QObject::connect(stranger, SIGNAL(StrangerDisconnected()), this, SLOT(StrangerDisconnected()));
    QObject::connect(stranger, SIGNAL(ConversationStarted()), this, SLOT(StrangerConnected()));
    QObject::connect(stranger, SIGNAL(StrangerStartsTyping()), this, SLOT(StrangerStartsTyping()));
    QObject::connect(stranger, SIGNAL(StrangerStopsTyping()), this, SLOT(StrangerStopsTyping()));

}

void MainWindow::enterPressed() {
    QString messageText = ui->textBrowser_2->toPlainText();
    ui->textBrowser->append("You: "+messageText);
    ui->textBrowser_2->clear();
    stranger->SendMessage(messageText);
}

void MainWindow::escapePressed() {
    stranger->StartConversation();
}

void MainWindow::ReceivedMessage(const QString &messageText) {
    ui->textBrowser->append("Stranger: "+messageText);
}

void MainWindow::StrangerDisconnected() {
    ui->textBrowser->append("Stranger disconnected");
}

void MainWindow::StrangerConnected() {
    ui->textBrowser->append("Stranger connected");
}

void MainWindow::StrangerStartsTyping() {
    ui->textBrowser->append("Stranger typing");
}

void MainWindow::StrangerStopsTyping() {
    ui->textBrowser->append("Stranger stopped typing");
}

MainWindow::~MainWindow()
{
    delete ui;
}
