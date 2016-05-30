#include "phonecallview.h"
#include "ui_phonecallview.h"

PhoneCallView::PhoneCallView(QString bt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhoneCallView)
{
    ui->setupUi(this);
    btAddress = bt;

    // connect to ofono service
    if(btAddress.compare("") != 0)
    {
        connectToOfono();
        qDebug() << "Device found";
        connected = true;
    }
    else
    {
        qDebug() << "Device not found";
        connected = false;
    }

    ui->anwser->setVisible(false);
    ui->decline->setVisible(false);
}

PhoneCallView::~PhoneCallView()
{
    delete ui;
}

void PhoneCallView::setAddress(QString address)
{
    btAddress = address;

    // reconnect to ofono service
    if(btAddress.compare("") != 0)
    {
        connectToOfono();
        connected = true;
    }
    else
    {
        connected = false;
    }
}

void PhoneCallView::callRecieved(QDBusObjectPath path, QMap<QString, QVariant> map)
{
    // setup interface
    call = new QDBusInterface("org.ofono", path.path(), "org.ofono.VoiceCall", QDBusConnection::systemBus(), this);

    // get data
    state = map.value("State").toString();
    currentNumber = map.value("LineIdentification").toString();

    ui->currentNum->setText(currentNumber);

    if(state.compare("incoming") == 0)
    {
        // change ui
        ui->anwser->setVisible(true);
        ui->decline->setVisible(true);
        ui->endCall->setVisible(false);
        ui->keypad->setVisible(false);
        ui->mute->setVisible(false);
    }

    show();
}

void PhoneCallView::callDisconnected()
{
    close();
}

void PhoneCallView::on_endCall_clicked()
{
    call->call("Hangup");
    close();
}

void PhoneCallView::connectToOfono()
{
    QString objectPath;
    objectPath.sprintf("/hfp/org/bluez/hci0/dev_%s", btAddress.toStdString().c_str());
    objectPath.replace(":", "_");

    qDebug() << "Phone call View: " << objectPath;

    // create interface
    QDBusConnection conn = QDBusConnection::systemBus();
    callManager = new QDBusInterface("org.ofono", objectPath, "org.ofono.VoiceCallManager", conn, this);
}

void PhoneCallView::on_anwser_clicked()
{
    call->call("Answer");

    // change ui
    ui->anwser->setVisible(false);
    ui->decline->setVisible(false);
    ui->endCall->setVisible(true);
    ui->keypad->setVisible(true);
    ui->mute->setVisible(true);
}

void PhoneCallView::on_decline_clicked()
{
    call->call("Hangup");
    close();
}
