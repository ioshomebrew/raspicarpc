#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qbluetoothglobal.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothservicediscoveryagent.h>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include "nowplaying.h"
#include "bluesettings.h"
#include "phone.h"
#include "settings.h"

#define CONFIG_FILE "/root/settings.ini"
#define DEBUG 1

typedef QMap<QString, QVariantMap> InterfaceList;
typedef QMap<QDBusObjectPath, InterfaceList> ManagedObjectList;
Q_DECLARE_METATYPE(InterfaceList)
Q_DECLARE_METATYPE(ManagedObjectList)
class NowPlaying;
class BlueSettings;
class Settings;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setAddress(QString address);
    ~MainWindow();

public slots:
    void callFinishedSlot(QDBusPendingCallWatcher*);

private slots:
    void on_settings_clicked();

    void on_phone_clicked();
    void on_maps_clicked();

    void on_nowplaying_clicked();

    void on_quit_clicked();

private:
    void connectToBluez();
     void connectDevice(QString address);
    QBluetoothLocalDevice *localDevice;
    QDBusInterface *interface;
    Settings *settings;
    NowPlaying *nowplaying;
    Phone *phone;
    QString btAddress;
    QString waitingbtAddress;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
