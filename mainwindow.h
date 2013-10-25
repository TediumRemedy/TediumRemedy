#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stranger.h"

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

public slots:
    void enterPressed();
    void escapePressed();

private:
    Stranger *stranger;

private slots:
    void ReceivedMessage(const QString &messageText);
    void StrangerDisconnected();
    void StrangerConnected();
    void StrangerStartsTyping();
    void StrangerStopsTyping();
};

#endif // MAINWINDOW_H
