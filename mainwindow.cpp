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
}

void MainWindow::enterPressed() {
    ui->textBrowser->append(ui->textBrowser_2->toPlainText());
    ui->textBrowser_2->clear();
}

void MainWindow::escapePressed() {
    stranger->StartConversation();

    //ui->textBrowser->append(ui->textBrowser_2->toPlainText());
    //ui->textBrowser_2->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}
