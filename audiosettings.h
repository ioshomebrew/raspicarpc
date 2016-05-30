#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class AudioSettings;
}

class AudioSettings : public QDialog
{
    Q_OBJECT

public:
    explicit AudioSettings(QWidget *parent = 0);
    ~AudioSettings();

private slots:
    void processSourcesFinished();
    void processSinksFinished();

private:
    Ui::AudioSettings *ui;
    int currentVolume;
    QSettings *settings;
    void setVol(int vol);
    QList<QString> sinks;
    QList<QString> sources;
    QProcess *process;
    QProcess *process2;
};

#endif // AUDIOSETTINGS_H
