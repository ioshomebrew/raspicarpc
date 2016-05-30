#ifndef NOWPLAYING_H
#define NOWPLAYING_H

#include <QDialog>
#include <QDebug>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusMessage>
#include <QTimer>
#include <QtCore/QMap>
#include <QProcess>
#include "mainwindow.h"
#define STATE_OFF 3
#define STATE_AUX 2
#define STATE_FM 1
#define STATE_BLUETOOTH 0

typedef QMap<QString, QVariantMap> InterfaceList;

namespace Ui {
class NowPlaying;
}

class NowPlaying : public QDialog
{
    Q_OBJECT

public:
    explicit NowPlaying(QString address, QWidget *parent = 0);
    ~NowPlaying();
    void setAddress(QString address);
    void setMute(bool val);
    void setVol(int val);
    void volUp();
    void volDown();

public slots:
    void updateStats();
    void updateReady();
    void interfacesAdded(QDBusObjectPath path, InterfaceList unused);
    void interfacesRemoved(QDBusObjectPath path, QStringList unused);

private slots:
    void on_play_clicked();

    void on_next_clicked();

    void on_previous_clicked();

    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    void connectToBlueZ();
    void updateUI();
    static void getMetadata(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    QDBusInterface *interface;
    QDBusInterface *interface1;
    QString btAddress;
    Ui::NowPlaying *ui;
    QTimer *timer;
    QTimer *timer2;
    bool connected;
    bool readyToUpdate;
    int currentSource;
    int currentVolume;

    // bluez properties
    QString status;
    QString title;
    QString artist;
    QString album;
    QString player;
};

#endif // NOWPLAYING_H
