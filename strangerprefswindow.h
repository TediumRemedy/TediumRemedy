#ifndef STRANGERPREFSWINDOW_H
#define STRANGERPREFSWINDOW_H

#include <QDialog>

namespace Ui {
class StrangerPrefsWindow;
}

class StrangerPrefsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StrangerPrefsWindow(QWidget *parent = 0);

    QString interestsString();
    QString languageSelector();

    void setParentCenterCoordinates(QPoint theParentCenter);


    ~StrangerPrefsWindow();

    QString languageNameForSelector(QString selector);
    void resetToDefault();
public slots:
    void reject();
private:
    Ui::StrangerPrefsWindow *ui;
    QVector< QPair<QString, QString> > languages;

    void showEvent(QShowEvent *event);

    QPoint parentCenter;
private slots:
    void interestsTyped();
    void languageChanged(QString newLanguage);
};

#endif // STRANGERPREFSWINDOW_H
