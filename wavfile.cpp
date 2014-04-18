#include "wavfile.h"


#include <QtCore/qendian.h>
#include <QVector>
#include <QDebug>
//#include "utils.h"
#include "wavfile.h"
#include <QDebug>



void WavFile::play() {
    wavInstance = new WavInstance(fileName, NULL);
    wavInstance->play();
}

WavFile::WavFile(QString fileName, QObject *parent):fileName(fileName), QObject(parent) {

}
