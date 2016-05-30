#include "phone.h"
#include "ui_phone.h"

Phone::Phone(QString bt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Phone)
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

    phoneCallView = new PhoneCallView(btAddress);
}

Phone::~Phone()
{
    delete ui;
}

void Phone::setAddress(QString address)
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

void Phone::connectToOfono()
{
    QString objectPath;
    objectPath.sprintf("/hfp/org/bluez/hci0/dev_%s", btAddress.toStdString().c_str());
    objectPath.replace(":", "_");

    qDebug() << "Phone: " << objectPath;

    // create interface
    QDBusConnection conn = QDBusConnection::systemBus();
    callManager = new QDBusInterface("org.ofono", objectPath, "org.ofono.VoiceCallManager", conn, this);

    // turn on modem
    QDBusInterface *modem = new QDBusInterface("org.ofono", objectPath, "org.ofono.Modem", conn, this);
    modem->setProperty("Powered", true);

    // connect signals
    QDBusConnection::systemBus().connect("org.ofono", objectPath, "org.ofono.VoiceCallManager", "CallAdded", this, SLOT(callRecieved(QDBusObjectPath,QMap<QString,QVariant>)));
    QDBusConnection::systemBus().connect("org.ofono", objectPath, "org.ofono.VoiceCallManager", "CallRemoved", this, SLOT(callDisconnected(QDBusObjectPath)));
}

void Phone::callRecieved(QDBusObjectPath path, QMap<QString, QVariant> map)
{
    qDebug() << "Call recieved signal";
    phoneCallView->callRecieved(path, map);
}

void Phone::callDisconnected(QDBusObjectPath path)
{
    qDebug() << "Call disconnected";
    phoneCallView->callDisconnected();
}

void Phone::on_one_clicked()
{
    phoneNumber += "1";
    ui->display->setText(phoneNumber);
}

void Phone::on_two_clicked()
{
    phoneNumber += "2";
    ui->display->setText(phoneNumber);
}

void Phone::on_three_clicked()
{
    phoneNumber += "3";
    ui->display->setText(phoneNumber);
}

void Phone::on_four_clicked()
{
    phoneNumber += "4";
    ui->display->setText(phoneNumber);
}

void Phone::on_five_clicked()
{
    phoneNumber += "5";
    ui->display->setText(phoneNumber);
}

void Phone::on_six_clicked()
{
    phoneNumber += "6";
    ui->display->setText(phoneNumber);
}

void Phone::on_seven_clicked()
{
    phoneNumber += "7";
    ui->display->setText(phoneNumber);
}

void Phone::on_eight_clicked()
{
    phoneNumber += "8";
    ui->display->setText(phoneNumber);
}

void Phone::on_nine_clicked()
{
    phoneNumber += "9";
    ui->display->setText(phoneNumber);
}

void Phone::on_apostrophe_clicked()
{
    phoneNumber += "*";
    ui->display->setText(phoneNumber);
}

void Phone::on_zero_clicked()
{
    phoneNumber += "0";
    ui->display->setText(phoneNumber);
}

void Phone::on_hashtag_clicked()
{
    phoneNumber += "#";
    ui->display->setText(phoneNumber);
}

void Phone::on_backspace_clicked()
{
    phoneNumber = phoneNumber.left(phoneNumber.length()-1);
    ui->display->setText(phoneNumber);
}

void Phone::on_clear_clicked()
{
    phoneNumber = "";
    ui->display->setText(phoneNumber);
}

void Phone::on_call_clicked()
{
    if(connected)
    {
        // call dial
        callManager->call("Dial", phoneNumber, "default"); // disable hide caller id
    }
    else
    {
        QMessageBox box;
        box.setText("No Phone Connected");
        box.exec();
    }
}

void Phone::on_back_clicked()
{
    close();
}
