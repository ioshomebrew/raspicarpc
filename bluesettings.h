#ifndef BLUESETTINGS_H
#define BLUESETTINGS_H

#include <qbluetoothglobal.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothservicediscoveryagent.h>
#include <QDialog>
#include <QPushButton>
#include <QDebug>
#include <QMenu>
#include <QProcess>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QFile>
#include <QSettings>
#include <QTimer>
#include "nowplaying.h"
#include "mainwindow.h"
#include <QtDBus/QDBusInterface>
#include <QtDBus/qdbusmetatype.h>
#include <QtDBus/QDBusReply>
#include <unistd.h>

class MainWindow;

typedef QMap<QString, QVariantMap> InterfaceList;

namespace Ui
{
    class BlueSettings;
}

class BlueSettings : public QDialog
{
    Q_OBJECT

public:
    explicit BlueSettings(MainWindow *main, QString address, QWidget *parent = 0);
    void setAddress(QString address);
    ~BlueSettings();

public slots:
    void addDevice(const QString);
    void interfaceAdded(QDBusObjectPath path, InterfaceList unused);
    void interfaceRemoved(QDBusObjectPath path, QStringList unused);
    void connectionChanged(QString interface, QVariantMap map, QList<QString> unused);

private slots:
    void on_close_clicked();
    void update();
    void on_autoConnect_clicked();

    void on_connect_clicked();

    void on_remove_clicked();

private:
    QString btAddress;
    QDBusInterface *adapter;
    MainWindow *mainwindow;
    Ui::BlueSettings *ui;
    QTimer *timer;
    void connectDevice(QString address);
    QList<QString> devices;
};

#endif // BLUESETTINGS_H
