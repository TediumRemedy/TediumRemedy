#ifndef WAVFILE_H
#define WAVFILE_H

#include <QObject>
#include <QAudioFormat>
#include <QAudio>
#include <QAudioOutput>
#include <QFile>

#include "wavinstance.h"

class WavInstance;

class WavFile : public QObject
{
    Q_OBJECT

    int m_headerLength;
    //QAudioFormat m_fileFormat;


    QString fileName;
    WavInstance *wavInstance;

public:
    void play();

    WavFile(QString fileName, QObject *parent);
signals:

public slots:


};

#endif // WAVFILE_H
