#include "nowplaying.h"
#include "ui_nowplaying.h"

QString titleGlobal;
QString artistGlobal;
QString albumGlobal;

NowPlaying::NowPlaying(QString address, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NowPlaying)
{
    ui->setupUi(this);

    btAddress = address;

    // connect to media player service
    if(btAddress.compare("") != 0)
    {
        connectToBlueZ();
        qDebug() << "Device found";
        connected = true;
    }
    else
    {
        qDebug() << "Device not found";
        connected = false;
    }

    // setup timers
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStats()));
    timer->start(1000);
    timer2 = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateReady()));
    timer2->start(5000);
    readyToUpdate = false;

    // setup signal
    QDBusConnection::systemBus().connect("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this, SLOT(interfacesAdded(QDBusObjectPath, InterfaceList)));
    QDBusConnection::systemBus().connect("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", this, SLOT(interfacesRemoved(QDBusObjectPath, QStringList)));
}

NowPlaying::~NowPlaying()
{
    delete ui;
}

void NowPlaying::interfacesAdded(QDBusObjectPath path, InterfaceList unused)
{
    // reconnect to bluez
    connectToBlueZ();
}

void NowPlaying::interfacesRemoved(QDBusObjectPath path, QStringList unused)
{
    // reconnect to bluez
    connectToBlueZ();
}

void NowPlaying::updateStats()
{
    if(readyToUpdate && connected)
    {
        bool changed = false;

        // update status
        if(QString::compare(status, interface->property("Status").toString(), Qt::CaseInsensitive) != 0)
        {
            status = interface->property("Status").toString();
            changed = true;
        }

        // update song info
        QDBusMessage reply = interface1->call("Get", "org.bluez.MediaPlayer1", "Track");
        qInstallMessageHandler(getMetadata);
        qDebug() << reply;
        qInstallMessageHandler(0);

        if(QString::compare(title, titleGlobal, Qt::CaseInsensitive) != 0)
        {
            title = titleGlobal;
            changed = true;
        }
        if(QString::compare(artist, artistGlobal, Qt::CaseInsensitive) != 0)
        {
            artist = artistGlobal;
            changed = true;
        }
        if(QString::compare(album, albumGlobal, Qt::CaseInsensitive) != 0)
        {
            album = albumGlobal;
            changed = true;
        }

        // update ui
        if(changed)
        {
            updateUI();
        }
    }
}

void NowPlaying::updateReady()
{
    if(connected)
    {
        readyToUpdate = true;
    }
    timer2->stop();

    // load media source
    QSettings config(CONFIG_FILE, QSettings::NativeFormat);
    currentSource = config.value("Source", STATE_OFF).toInt();

    // if state is set to bluetooth, but no device available
    if(!connected && currentSource == STATE_BLUETOOTH)
    {
        currentSource = STATE_OFF;
    }

    // set current state
    ui->comboBox->setCurrentIndex(currentSource);

    ui->title->setAlignment(Qt::AlignVCenter);
    ui->artist->setAlignment(Qt::AlignVCenter);
    ui->album->setAlignment(Qt::AlignVCenter);

    // setup UI
    switch(currentSource)
    {
        case STATE_BLUETOOTH:
        {
            // set track info visible
            ui->titleLabel->setVisible(true);
            ui->artistLabel->setVisible(true);
            ui->albumLabel->setVisible(true);
            ui->artist->setVisible(true);
            ui->title->setVisible(true);
            ui->album->setVisible(true);

            // enable audio
            setMute(false);
        }
        break;

        case STATE_FM:
        {
            // set track info invisible
            ui->titleLabel->setVisible(false);
            ui->artistLabel->setVisible(false);
            ui->albumLabel->setVisible(false);
            ui->artist->setVisible(false);
            ui->title->setVisible(false);
            ui->album->setVisible(false);

            // enable audio
            setMute(false);
        }
        break;

        case STATE_AUX:
        {
            // set track info invisible
            ui->titleLabel->setVisible(false);
            ui->artistLabel->setVisible(false);
            ui->albumLabel->setVisible(false);
            ui->artist->setVisible(false);
            ui->title->setVisible(false);
            ui->album->setVisible(false);

            // enable audio
            setMute(false);
        }
        break;

        case STATE_OFF:
        {
            // disable audio
            setMute(true);

            // set track info invisible
            ui->titleLabel->setVisible(false);
            ui->artistLabel->setVisible(false);
            ui->albumLabel->setVisible(false);
            ui->artist->setVisible(false);
            ui->title->setVisible(false);
            ui->album->setVisible(false);
        }
        break;
    }
}

void NowPlaying::getMetadata(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // get title
    QString substring = msg.mid(msg.indexOf("Title"));
    int stop = substring.indexOf(']');
    titleGlobal = substring.mid(29, (stop-1)-29);

    // get album
    int stop2 = msg.indexOf("Album");
    QString substring2 = msg.mid(stop2);
    int stop3 = substring2.indexOf(']', Qt::CaseInsensitive);
    QString substring3 = substring2.mid(29, stop3);
    albumGlobal = substring3.mid(0, substring3.indexOf(']')-1);

    // get artist
    int stop4 = msg.indexOf("Artist");
    QString substring4 = msg.mid(stop4);
    int stop5 = substring4.indexOf(']');
    QString substring5 = substring4.mid(29, stop5);
    artistGlobal = substring5.mid(1, substring5.indexOf(']')-2);
}

void NowPlaying::setAddress(QString address)
{
    btAddress = address;

    // connect to media player service
    if(btAddress.compare("") != 0)
    {
        // reconnect to media server
        connectToBlueZ();
        connected = true;
        timer2->start(5000);
    }
    else
    {
        connected = false;
        readyToUpdate = false;
    }
}

void NowPlaying::connectToBlueZ()
{
    // bluez
    QString objectPath;

    // get player num
    QDBusInterface *temp = new QDBusInterface("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", QDBusConnection::systemBus());
    QDBusReply<ManagedObjectList> reply = temp->call("GetManagedObjects");

    ManagedObjectList list = reply.value();
    ManagedObjectList::iterator i;
    for (i = list.begin(); i != list.end(); ++i)
    {
        if(i.key().path().contains("player", Qt::CaseInsensitive))
        {   
            objectPath = i.key().path();
            break;
        }
    }

    // if player not found
    if(objectPath.compare("", Qt::CaseInsensitive) == 0)
    {
        connected = false;
        return;
    }

    // setup interface
    QDBusConnection bus = QDBusConnection::systemBus();
    interface = new QDBusInterface("org.bluez",
                                                   objectPath,
                                                   "org.bluez.MediaPlayer1",
                                                   bus,
                                                   this);
    interface1 = new QDBusInterface("org.bluez", objectPath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());

    // get initial properties
    status = interface->property("Status").toString();

    // update ui
    updateUI();

    // if no error set connected to true
    connected = true;
}

void NowPlaying::updateUI()
{
    // update play button
    if(QString::compare(status, "playing") == 0)
    {
        ui->play->setText("Pause");
    }
    else
    {
        ui->play->setText("Play");
    }

    // update music info
    ui->title->setText(title);
    ui->artist->setText(artist);
    ui->album->setText(album);
}

void NowPlaying::on_play_clicked()
{
    if(connected)
    {
        QDBusReply<QString> reply;
        // call play or pause
        if(QString::compare(ui->play->text(), "Pause", Qt::CaseInsensitive) != 0)
        {
            reply = interface->call("Play");
        }
        else
        {
            reply = interface->call("Pause");
        }
    }
}

void NowPlaying::on_next_clicked()
{
    if(connected)
    {
        // call next
        interface->call("Next");
    }
}

void NowPlaying::on_previous_clicked()
{
    if(connected)
    {
        // call previous
        interface->call("Previous");
    }
}

void NowPlaying::on_pushButton_clicked()
{
    // set track info invisible
    ui->titleLabel->setVisible(false);
    ui->artistLabel->setVisible(false);
    ui->albumLabel->setVisible(false);
    ui->artist->setVisible(false);
    ui->title->setVisible(false);
    ui->album->setVisible(false);
    close();
}

void NowPlaying::setVol(int val)
{
    QString volCommand;
    volCommand.sprintf("pactl set-sink-volume \"alsa_output.0.analog-stereo\" %i\%", val);
    qDebug() << volCommand;

    QProcess *process = new QProcess();
    process->start(volCommand);

    // update volume value
    currentVolume = val;

    // set new volume
    QSettings config(CONFIG_FILE, QSettings::NativeFormat);
    config.setValue("Volume", val);
}

void NowPlaying::setMute(bool val)
{
    QProcess *process2 = new QProcess();
    if(val)
    {
        process2->start("pactl set-sink-mute \"alsa_output.0.analog-stereo\" true");
    }
    else
    {
        process2->start("pactl set-sink-mute \"alsa_output.0.analog-stereo\" false");
    }
}

void NowPlaying::volUp()
{
    if(currentVolume == 100)
    {
        return;
    }
    else
    {
        currentVolume++;
        setVol(currentVolume);
    }
}

void NowPlaying::volDown()
{
    if(currentVolume == 0)
    {
        return;
    }
    else
    {
        currentVolume--;
        setVol(currentVolume);
    }
}

void NowPlaying::on_comboBox_currentIndexChanged(int index)
{
    switch(index)
    {
        case STATE_BLUETOOTH:
        {
            if(!connected)
            {
                QMessageBox box;
                box.setText("No Bluetooth Audio Device connected");
                box.exec();
            }
            else
            {
                // set track info visible
                ui->titleLabel->setVisible(true);
                ui->artistLabel->setVisible(true);
                ui->albumLabel->setVisible(true);
                ui->artist->setVisible(true);
                ui->title->setVisible(true);
                ui->album->setVisible(true);
            }

            // enable audio
            setMute(false);
        }
        break;

        case STATE_FM:
        {
            QMessageBox box;
            box.setText("FM Not Implemented");
            box.exec();
        }
        break;

        case STATE_AUX:
        {
            QMessageBox box;
            box.setText("AUX Not Implemented");
            box.exec();
        }
        break;

        case STATE_OFF:
        {
            // set track info invisible
            ui->titleLabel->setVisible(false);
            ui->artistLabel->setVisible(false);
            ui->albumLabel->setVisible(false);
            ui->artist->setVisible(false);
            ui->title->setVisible(false);
            ui->album->setVisible(false);
        }
        break;
    }
    currentSource = index;

    // save current state
    QSettings config(CONFIG_FILE, QSettings::NativeFormat);
    config.setValue("Source", currentSource);
    config.sync();
}
