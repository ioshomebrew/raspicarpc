#include "audiosettings.h"
#include "ui_audiosettings.h"

AudioSettings::AudioSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioSettings)
{
    ui->setupUi(this);

    settings = new QSettings(CONFIG_FILE, QSettings::NativeFormat);

    // load current volume
    currentVolume = settings->value("Volume", 0).toInt();

    // find current sources and sinks
    process = new QProcess(this);
    process->start("pactl list sources short", QIODevice::ReadWrite);
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(processSourcesFinished()));
    process2 = new QProcess(this);
    process2->start("pactl list sinks short", QIODevice::ReadWrite);
    connect(process2, SIGNAL(readyReadStandardError()), this, SLOT(processSinksFinished()));
}

AudioSettings::~AudioSettings()
{
    delete ui;
}

void AudioSettings::processSinksFinished()
{
    QList<QString> sinks2;
    qDebug() << "Process sink finished";
    QString *data = new QString(process2->readAllStandardOutput());
    QStringList lines = data->split("\n", QString::SkipEmptyParts);
    foreach( QString line, lines ) {
       sinks2.append(line);
    }

    // get data from pulseaudio
    for(int i = 0; i < sinks2.size(); i++)
    {
        QString line = sinks2.at(i);
        int index = line.indexOf('a', Qt::CaseSensitive);
        QString newLine = line.mid(index);
        int index2 = newLine.indexOf('\t', Qt::CaseSensitive);
        QString newLine2 = newLine.left(index2);
        sinks.insert(i, newLine2);
        ui->audioOutput->insertItem(i, sinks.at(i));
    }
}

void AudioSettings::processSourcesFinished()
{
    QList<QString> sources2;
    qDebug() << "Process source finished";
    QString *data = new QString(process->readAllStandardOutput());
    QStringList lines = data->split("\n", QString::SkipEmptyParts);
    foreach( QString line, lines ) {
       sources2.append(line);
    }

    // get data from pulseaudio
    for(int i = 0; i < sources2.size(); i++)
    {
        QString line = sources2.at(i);
        int index = line.indexOf('a', Qt::CaseSensitive);
        QString newLine = line.mid(index);
        int index2 = newLine.indexOf('\t', Qt::CaseSensitive);
        QString newLine2 = newLine.left(index2);
        sources.insert(i, newLine2);
        ui->auxSource->insertItem(i, sources.at(i));
        ui->micIn->insertItem(i, sources.at(i));
    }
}

void AudioSettings::setVol(int vol)
{
    QString volCommand;
    volCommand.sprintf("pactl set-sink-volume \"alsa_output.0.analog-stereo\" %i\%", vol);
    qDebug() << volCommand;

    QProcess *process3 = new QProcess();
    process3->start(volCommand);

    // update volume value
    currentVolume = vol;

    // set new volume
    settings->setValue("Volume", vol);
}
