#include "strangerprefswindow.h"
#include "ui_strangerprefswindow.h"
#include <QDebug>
#include <QFile>
#include <QPushButton>

StrangerPrefsWindow::StrangerPrefsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StrangerPrefsWindow)
{
    ui->setupUi(this);

    //languages.push_back(QPair<QString, QString>("1", "2"));
    languages.push_back(QPair<QString, QString>("Afrikaans", "af"));
    languages.push_back(QPair<QString, QString>("Albanian", "sq"));
    languages.push_back(QPair<QString, QString>("Arabic", "ar"));
    languages.push_back(QPair<QString, QString>("Azerbaijani", "az"));
    languages.push_back(QPair<QString, QString>("Basque", "eu"));
    languages.push_back(QPair<QString, QString>("Bengali", "bn"));
    languages.push_back(QPair<QString, QString>("Belarusian", "be"));
    languages.push_back(QPair<QString, QString>("Bulgarian", "bg"));
    languages.push_back(QPair<QString, QString>("Catalan", "ca"));
    languages.push_back(QPair<QString, QString>("Chinese Simplified", "zh-CN"));
    languages.push_back(QPair<QString, QString>("Chinese Traditional", "zh-TW"));
    languages.push_back(QPair<QString, QString>("Croatian", "hr"));
    languages.push_back(QPair<QString, QString>("Czech", "cs"));
    languages.push_back(QPair<QString, QString>("Danish", "da"));
    languages.push_back(QPair<QString, QString>("Dutch", "nl"));
    languages.push_back(QPair<QString, QString>("English", "en"));
    languages.push_back(QPair<QString, QString>("Esperanto", "eo"));
    languages.push_back(QPair<QString, QString>("Estonian", "et"));
    languages.push_back(QPair<QString, QString>("Filipino", "tl"));
    languages.push_back(QPair<QString, QString>("Finnish", "fi"));
    languages.push_back(QPair<QString, QString>("French", "fr"));
    languages.push_back(QPair<QString, QString>("Galician", "gl"));
    languages.push_back(QPair<QString, QString>("Georgian", "ka"));
    languages.push_back(QPair<QString, QString>("German", "de"));
    languages.push_back(QPair<QString, QString>("Greek", "el"));
    languages.push_back(QPair<QString, QString>("Gujarati", "gu"));
    languages.push_back(QPair<QString, QString>("Haitian Creole", "ht"));
    languages.push_back(QPair<QString, QString>("Hebrew", "iw"));
    languages.push_back(QPair<QString, QString>("Hindi", "hi"));
    languages.push_back(QPair<QString, QString>("Hungarian", "hu"));
    languages.push_back(QPair<QString, QString>("Icelandic", "is"));
    languages.push_back(QPair<QString, QString>("Indonesian", "id"));
    languages.push_back(QPair<QString, QString>("Irish", "ga"));
    languages.push_back(QPair<QString, QString>("Italian", "it"));
    languages.push_back(QPair<QString, QString>("Japanese", "ja"));
    languages.push_back(QPair<QString, QString>("Kannada", "kn"));
    languages.push_back(QPair<QString, QString>("Korean", "ko"));
    languages.push_back(QPair<QString, QString>("Latin", "la"));
    languages.push_back(QPair<QString, QString>("Latvian", "lv"));
    languages.push_back(QPair<QString, QString>("Lithuanian", "lt"));
    languages.push_back(QPair<QString, QString>("Macedonian", "mk"));
    languages.push_back(QPair<QString, QString>("Malay", "ms"));
    languages.push_back(QPair<QString, QString>("Maltese", "mt"));
    languages.push_back(QPair<QString, QString>("Norwegian", "no"));
    languages.push_back(QPair<QString, QString>("Persian", "fa"));
    languages.push_back(QPair<QString, QString>("Polish", "pl"));
    languages.push_back(QPair<QString, QString>("Portuguese", "pt"));
    languages.push_back(QPair<QString, QString>("Romanian", "ro"));
    languages.push_back(QPair<QString, QString>("Russian", "ru"));
    languages.push_back(QPair<QString, QString>("Serbian", "sr"));
    languages.push_back(QPair<QString, QString>("Slovak", "sk"));
    languages.push_back(QPair<QString, QString>("Slovenian", "sl"));
    languages.push_back(QPair<QString, QString>("Spanish", "es"));
    languages.push_back(QPair<QString, QString>("Swahili", "sw"));
    languages.push_back(QPair<QString, QString>("Swedish", "sv"));
    languages.push_back(QPair<QString, QString>("Tamil", "ta"));
    languages.push_back(QPair<QString, QString>("Telugu", "te"));
    languages.push_back(QPair<QString, QString>("Thai", "th"));
    languages.push_back(QPair<QString, QString>("Turkish", "tr"));
    languages.push_back(QPair<QString, QString>("Ukrainian", "uk"));
    languages.push_back(QPair<QString, QString>("Urdu", "ur"));
    languages.push_back(QPair<QString, QString>("Vietnamese", "vi"));
    languages.push_back(QPair<QString, QString>("Welsh", "cy"));
    languages.push_back(QPair<QString, QString>("Yiddish", "yi"));

    ui->languagePicker->clear();
    typedef QPair<QString, QString> StringPair;
    foreach(StringPair pair, languages) {
        ui->languagePicker->addItem(pair.first, pair.second);
    }

    ui->languagePicker->setCurrentIndex(ui->languagePicker->findData("en"));

    QObject::connect(ui->interestsTextbox, SIGNAL(textChanged()), this, SLOT(interestsTyped()));
    QObject::connect(ui->languagePicker, SIGNAL(currentIndexChanged(QString)), this, SLOT(languageChanged(QString)));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Ok");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Default english chat");



}

QString StrangerPrefsWindow::interestsString()
{
    QString interestsString = ui->interestsTextbox->toPlainText();

    //the following regexp is used to turn a string of interests into a list of interests-tokens
    // "[^"]*"|[^\s,"]+

    //QString testString = " , \"one"    \"two\",   \" three and more \" ,four,   \"five\" ,, six  seven ";
    //qDebug() << interestsString;

    QStringList interestsList;
    QRegExp rx = QRegExp("\"[^\"]*\"|[^\\s,\"]+");
    //qDebug() << rx;
    int pos = 0;
    while((pos = rx.indexIn(interestsString, pos)) !=-1) {
        QStringList tokens = rx.capturedTexts();
        foreach(QString t, tokens) {
            QString trimmedToken = t.remove(QRegExp("^\"")).remove(QRegExp("\"$")); //trim quotation marks
            interestsList.append("\""+trimmedToken+"\"");
        }
        pos+=rx.matchedLength();
    }

    return interestsList.join(", ");
}

QString StrangerPrefsWindow::languageSelector()
{
    return ui->languagePicker->currentData().toString();
}

QString StrangerPrefsWindow::languageNameForSelector(QString selector) {
    typedef QPair<QString, QString> StringPair;
    foreach(StringPair pair, languages) {
        if(pair.second == selector)
            return pair.first;
    }

    return "<Unknown language>"; //shouldn't be here

}

StrangerPrefsWindow::~StrangerPrefsWindow()
{
    delete ui;
}

void StrangerPrefsWindow::interestsTyped()
{
    //qDebug("interstsTyped");
    if(!ui->interestsTextbox->toPlainText().isEmpty()) {
        ui->languagePicker->setCurrentIndex(ui->languagePicker->findData("en"));
    }
}

void StrangerPrefsWindow::languageChanged(QString newLanguage)
{
    if(ui->languagePicker->currentData().toString() != "en") {
        ui->interestsTextbox->setPlainText("");
    }
}

void StrangerPrefsWindow::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    move(0,0);
    move((parentCenter.x() - geometry().center().x()), (parentCenter.y() - geometry().center().y()));
}

void StrangerPrefsWindow::setParentCenterCoordinates(QPoint theParentCenter) {
    parentCenter = theParentCenter;
}

void StrangerPrefsWindow::resetToDefault() {
    ui->interestsTextbox->setPlainText("");
    ui->languagePicker->setCurrentIndex(ui->languagePicker->findData("en"));
}

void StrangerPrefsWindow::reject() {
    this->resetToDefault();
    QDialog::reject();
}
