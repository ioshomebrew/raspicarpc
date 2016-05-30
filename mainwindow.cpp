#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), localDevice(new QBluetoothLocalDevice),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDBusRegisterMetaType<InterfaceList>();
    qDBusRegisterMetaType<ManagedObjectList>();

    connectToBluez();

    // load config
    QSettings config(CONFIG_FILE, QSettings::NativeFormat);
    btAddress = config.value("Device").toString();

    // check if autoconnect is enabled
    bool autoConnect = config.value("AutoConnect", true).toBool();
    if(autoConnect && btAddress.compare("") != 0)
    {
        connectDevice(btAddress);
    }
    else
    {
        // set btAddress to null if autoconnect not enabled
        btAddress = "";
    }

    // set btaddress to nothing
    waitingbtAddress = btAddress;
    btAddress = "";

    if(!DEBUG)
    {
        ui->quit->setVisible(false);
    }

    // load window classes
    nowplaying = new NowPlaying(btAddress, this);
    settings = new Settings(this, btAddress, this);
    phone = new Phone(btAddress, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectDevice(QString address)
{
    // connect device
    QString objectPath;
    objectPath.sprintf("/org/bluez/hci0/dev_%s", address.toStdString().c_str());
    objectPath.replace(":", "_");
    qDebug() << objectPath;
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusInterface *interface = new QDBusInterface("org.bluez",
                                                   objectPath,
                                                   "org.bluez.Device1",
                                                   bus,
                                                   this);

    // connect device asynchoncsly
    QDBusPendingCall pcall = interface->asyncCall("Connect");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(callFinishedSlot(QDBusPendingCallWatcher*)));
}

void MainWindow::callFinishedSlot(QDBusPendingCallWatcher *test)
{
    qDebug() << "Call finished";
    QDBusReply<void> reply = test->reply();
    if(reply.isValid())
    {
        setAddress(waitingbtAddress);
    }
    else
    {
        qDebug() << reply.error();
    }
}

void MainWindow::connectToBluez()
{
    // connect to bluez
    interface = new QDBusInterface("org.bluez", "/org/bluez/hci0", "org.bluez.Adapter1", QDBusConnection::systemBus(), this);
}

void MainWindow::setAddress(QString address)
{
    btAddress = address;

    // notify classes of address updates
    nowplaying->setAddress(address);
    phone->setAddress(address);
    settings->setAddress(address);
}

void MainWindow::on_settings_clicked()
{
    settings->show();
}

void MainWindow::on_phone_clicked()
{
    phone->show();
}

void MainWindow::on_maps_clicked()
{
    QMessageBox box;
    box.setText("TODO");
    box.exec();
}

void MainWindow::on_nowplaying_clicked()
{
    nowplaying->show();
}

void MainWindow::on_quit_clicked()
{
    if(DEBUG)
    {
        QApplication::quit();
    }
}
