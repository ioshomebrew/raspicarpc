#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "bluesettings.h"
#include "audiosettings.h"

class BlueSettings;
class MainWindow;
class AudioSettings;

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(MainWindow *main, QString address, QWidget *parent = 0);
    ~Settings();
    void setAddress(QString address);

private slots:
    void on_back_clicked();

    void on_bluetooth_clicked();

    void on_audio_clicked();

private:
    Ui::Settings *ui;
    BlueSettings *blueSettings;
    AudioSettings *audioSettings;
};

#endif // SETTINGS_H
