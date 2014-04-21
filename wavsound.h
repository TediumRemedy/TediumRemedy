#ifndef WAVINSTANCE_H
#define WAVINSTANCE_H

#include <QObject>

#include <QtCore/qendian.h>
#include <QVector>
#include <QDebug>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QFile>

class WavSound: QObject {
    Q_OBJECT

    int m_headerLength;
    //QAudioFormat m_fileFormat;


    QString fileName;
    QFile *file;
    QAudioOutput *audio;

    bool readHeader(QAudioFormat *theFormat, QFile *file);
public:
    void play();

    WavSound(QString fileName, QObject *parent);
signals:

public slots:

private slots:
    void handleStateChanged(QAudio::State newState);
};

#endif // WAVINSTANCE_H
