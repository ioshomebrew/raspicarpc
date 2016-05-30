#include "bluesettings.h"
#include "ui_bluesettings.h"
#include <qbluetoothaddress.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothuuid.h>

BlueSettings::BlueSettings(MainWindow *main, QString address, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlueSettings)
{
    ui->setupUi(this);

    btAddress = address;

    // get mainwindow
    mainwindow = main;
    
    // setup interface
    adapter = new QDBusInterface("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", QDBusConnection::systemBus(), this);
    adapter->call("StartDiscovery");

    // get list of devices from dbus
    QDBusInterface *interface = new QDBusInterface("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", QDBusConnection::systemBus());
    QDBusReply<ManagedObjectList> reply = interface->call("GetManagedObjects");

    ManagedObjectList list = reply.value();
    ManagedObjectList::iterator i;
    int x = 0;
    for (i = list.begin(); i != list.end(); ++i, x++)
    {
        if(i.key().path().contains("dev", Qt::CaseInsensitive))
        {
            // add device to list
            addDevice(i.key().path());
        }
    }

    // get name
    const char *name = adapter->property("Alias").toString().toStdString().c_str();
    ui->name->setText(name);

    // get is autoconnect enabled
    QSettings settings(CONFIG_FILE, QSettings::NativeFormat);
    bool autoConnect = settings.value("AutoConnect", true).toBool();
    ui->autoConnect->setChecked(autoConnect);

    // setup signal
    QDBusConnection::systemBus().connect("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this, SLOT(interfaceAdded(QDBusObjectPath, InterfaceList)));
    QDBusConnection::systemBus().connect("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", this, SLOT(interfaceRemoved(QDBusObjectPath, QStringList)));
}

BlueSettings::~BlueSettings()
{
    delete ui;
}

void BlueSettings::setAddress(QString address)
{
    btAddress = address;

    QString objectPath;
    objectPath.sprintf("/org/bluez/hci0/dev_%s", address.toStdString().c_str());
    objectPath.replace(":", "_");

    // setup connectchanged signal
    QDBusConnection::systemBus().connect("org.bluez", objectPath, "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(connectionChanged(QString, QVariantMap, QList<QString>)));
}

void BlueSettings::interfaceAdded(QDBusObjectPath path, InterfaceList unused)
{
    qDebug() << path.path();
    qDebug() << "Interface added";
    if(path.path().contains("dev", Qt::CaseInsensitive))
    {
        // add device to list
        addDevice(path.path());
    }
}

void BlueSettings::interfaceRemoved(QDBusObjectPath path, QStringList unused)
{
    qDebug() << "Interface removed";

    if(path.path().contains("dev", Qt::CaseInsensitive))
    {
        // remove device
        for(int i = 0; i < devices.size(); i++)
        {
            QString temp = devices.at(i);
            temp.replace(":", "_");
            if(path.path().contains(temp, Qt::CaseInsensitive))
            {
                delete ui->list->item(i);
                devices.removeAt(i);
            }
        }
    }
}

void BlueSettings::connectionChanged(QString interface, QVariantMap map, QList<QString> unused)
{
    if(interface.compare("org.bluez.Device1", Qt::CaseSensitive) == 0)
    {
        QVariantMap::iterator i;
        for(i = map.begin(); i != map.end(); ++i)
        {
            if(i.key().compare("Connected") == 0)
            {
                if(i.value().toBool() == true)
                {
                    // inform program device connected
                    mainwindow->setAddress(btAddress);
                }
                else
                {
                    // inform program device disconnected
                    mainwindow->setAddress("");
                }
            }
        }
    }
}

void BlueSettings::update()
{

}

// takes DBus path
void BlueSettings::addDevice(const QString device)
{
    qDebug() << "adding device: " << device;
    QDBusInterface *interface = new QDBusInterface("org.bluez", device, "org.bluez.Device1", QDBusConnection::systemBus(), this);
    QString label = interface->property("Alias").toString();
    QList<QListWidgetItem *> items = ui->list->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(label);
        bool paired = interface->property("Paired").toBool();
        if (paired)
        {
            item->setSizeHint(QSize(50, 50));
            item->setFont(QFont("Courier", 20));
            item->setTextColor(QColor(Qt::green));
        }
        else
        {
            item->setSizeHint(QSize(50, 50));
            item->setFont(QFont("Courier", 20));
            item->setTextColor(QColor(Qt::black));
        }

        ui->list->addItem(item);

        // add to array
        if(!devices.contains(interface->property("Address").toString()))
        {
            devices.append(interface->property("Address").toString());
        }
    }
}

// accepts dbus path
void BlueSettings::connectDevice(QString address)
{
    // connect device
    QString objectPath;
    objectPath.sprintf("/org/bluez/hci0/dev_%s", address.toStdString().c_str());
    objectPath.replace(":", "_");
    qDebug() << "Settings: " << objectPath;
    QDBusInterface *interface = new QDBusInterface("org.bluez",
                                                   objectPath,
                                                   "org.bluez.Device1",
                                                   QDBusConnection::systemBus(),
                                                   this);
    // set device to trusted
    interface->setProperty("Trusted", true);
    QDBusReply<void> reply = interface->call("Connect");
    if(reply.isValid())
    {
        QMessageBox box;
        box.setText("Device connected sucessfully");
        box.exec();

        // change phone address
        btAddress = address;
        mainwindow->setAddress(address);

        // write to config paired device
        QSettings settings(CONFIG_FILE, QSettings::NativeFormat);
        settings.setValue("Device", address);

        // setup connectchanged signal
        QDBusConnection::systemBus().connect("org.bluez", objectPath, "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(connectionChanged(QString, QVariantMap, QList<QString>)));
    }
    else
    {
        qDebug() << reply.error();
    }
}

void BlueSettings::on_close_clicked()
{
    // set name
    adapter->setProperty("Alias", ui->name->text());

    close();
}

void BlueSettings::on_autoConnect_clicked()
{
    if(ui->autoConnect->isChecked())
    {
        QSettings settings(CONFIG_FILE, QSettings::NativeFormat);
        settings.setValue("AutoConnect", true);
    }
    else
    {
        QSettings settings(CONFIG_FILE, QSettings::NativeFormat);
        settings.setValue("AutoConnect", false);
    }
}

void BlueSettings::on_connect_clicked()
{
    // get mac address
    QString item = devices.at(ui->list->currentIndex().row());

    // start pairing
    QString objectPath;
    objectPath.sprintf("/org/bluez/hci0/dev_%s", item.toStdString().c_str());
    objectPath.replace(":", "_");
    QDBusInterface *interface = new QDBusInterface("org.bluez",
                                                   objectPath,
                                                   "org.bluez.Device1",
                                                   QDBusConnection::systemBus(),
                                                   this);
    qDebug() << "Settings: " << objectPath;
    QDBusReply<void> reply = interface->asyncCall("Pair");
    if(reply.isValid())
    {
        connectDevice(item);
    }
    else
    {
        if(reply.error().name().compare("org.bluez.Error.AlreadyExists", Qt::CaseSensitive) == 0)
        {
            connectDevice(item);
        }
        else
        {
            QMessageBox box;
            box.setText(reply.error().name());
            box.exec();
        }
    }
}

void BlueSettings::on_remove_clicked()
{
    // get address from table
    int indexSelected = ui->list->currentIndex().row();
    QString address = devices.at(indexSelected);
    QString objectPath;
    objectPath.sprintf("/org/bluez/hci0/dev_%s", address.toStdString().c_str());
    objectPath.replace(":", "_");
    QDBusObjectPath path(objectPath);

    // check if it's paired
    QDBusInterface *interface = new QDBusInterface("org.bluez", objectPath, "org.bluez.Device1", QDBusConnection::systemBus(), this);
    if(interface->property("Paired").toBool())
    {
        QDBusReply<void> reply = adapter->call("RemoveDevice", QVariant::fromValue(path));
        if(!reply.isValid())
        {
            QMessageBox box;
            box.setText(reply.error().name());
            box.exec();
        }
    }

    // update settings and mainwindow
    QSettings settings(CONFIG_FILE, QSettings::NativeFormat);
    settings.setValue("Device", "");
    mainwindow->setAddress("");
}
