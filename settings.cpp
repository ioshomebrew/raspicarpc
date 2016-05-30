#include "settings.h"
#include "ui_settings.h"

Settings::Settings(MainWindow *main, QString address, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    blueSettings = new BlueSettings(main, address, this);
    audioSettings = new AudioSettings(this);
}

void Settings::setAddress(QString address)
{
    blueSettings->setAddress(address);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_back_clicked()
{
    close();
}

void Settings::on_bluetooth_clicked()
{
    blueSettings->show();
}

void Settings::on_audio_clicked()
{
    audioSettings->show();
}
