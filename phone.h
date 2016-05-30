#ifndef PHONE_H
#define PHONE_H

#include <QDialog>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QMessageBox>
#include "phonecallview.h"

namespace Ui {
class Phone;
}

class Phone : public QDialog
{
    Q_OBJECT

public:
    explicit Phone(QString bt, QWidget *parent = 0);
    void setAddress(QString address);
    ~Phone();

public slots:
    void callRecieved(QDBusObjectPath path, QMap<QString, QVariant> map);
    void callDisconnected(QDBusObjectPath path);

private slots:
    void on_one_clicked();

    void on_two_clicked();

    void on_three_clicked();

    void on_four_clicked();

    void on_five_clicked();

    void on_six_clicked();

    void on_seven_clicked();

    void on_eight_clicked();

    void on_nine_clicked();

    void on_apostrophe_clicked();

    void on_zero_clicked();

    void on_hashtag_clicked();

    void on_backspace_clicked();

    void on_clear_clicked();
    
    void on_call_clicked();

    void on_back_clicked();

private:
    Ui::Phone *ui;
    QString phoneNumber;
    QString btAddress;
    QDBusInterface *callManager;
    PhoneCallView *phoneCallView;
    bool connected;
    void connectToOfono();
};

#endif // PHONE_H
