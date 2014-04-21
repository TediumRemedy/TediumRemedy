#include "wavsound.h"

//total 8 bytes
struct chunk
{
    char        id[4];
    quint32     size;
};

//total 12 bytes
struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor; //8 bytes
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff; //12 bytes
    WAVEHeader  wave;
};




void WavSound::play() {
    QFile *file = new QFile(fileName);
    file->open(QIODevice::ReadOnly);
    QAudioFormat f;
    readHeader(&f, file);

    QAudioFormat format = f;

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    //seek(0x4e*4); //...avoiding clicks from the wav files

    file->seek(0x4e);

    QAudioOutput *audio = new QAudioOutput(format, NULL);
    if(file->size()<30000)
        audio->setBufferSize(10000);

    audio->setVolume(0.5);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    audio->start(file);
}

WavSound::WavSound(QString fileName, QObject *parent):fileName(fileName), QObject(parent)
{

}



bool WavSound::readHeader(QAudioFormat *theFormat, QFile *file)
{
    QAudioFormat m_fileFormat;
    CombinedHeader header;
    bool result = file->read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
    if (result) {
        if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
            || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
            && memcmp(&header.riff.type, "WAVE", 4) == 0
            && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
            && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)) {

            qDebug() << header.riff.descriptor.size << " " << header.wave.descriptor.size;

            // Read off remaining header information
            DATAHeader dataHeader;

            if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
                // Extended data available
                quint16 extraFormatBytes;
                if (file->peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                    return false;
                const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                if (file->read(throwAwayBytes).size() != throwAwayBytes)
                    return false;
            }

            if (file->read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                return false;

            // Establish format
            if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
            else
                m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
            m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
            m_fileFormat.setCodec("audio/pcm");
            m_fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
            m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
            m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        } else {
            result = false;
        }
    }
    m_headerLength = file->pos();

    *theFormat = m_fileFormat;

    return result;
}

void WavSound::handleStateChanged(QAudio::State newState)
{
    qDebug() << "handleStateChanged: " << newState;
    switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            audio->stop();
            //file->close();
            //delete audio;
            //delete file;
            //this->deleteLater();
            //delete this;
            break;

        case QAudio::StoppedState:
            // Stopped for other reasons
            if (audio->error() != QAudio::NoError) {
                // Error handling
            }
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}


