#ifndef PHONECALLVIEW_H
#define PHONECALLVIEW_H

#include <QDialog>
#include <QDBusInterface>
#include <QDebug>
#include <QThread>

namespace Ui {
class PhoneCallView;
}

class PhoneCallView : public QDialog
{
    Q_OBJECT

public:
    explicit PhoneCallView(QString bt, QWidget *parent = 0);
    ~PhoneCallView();
    void callRecieved(QDBusObjectPath path, QMap<QString, QVariant> map);
    void setAddress(QString address);
    void callDisconnected();

private slots:
    void on_endCall_clicked();

    void on_anwser_clicked();

    void on_decline_clicked();

private:
    Ui::PhoneCallView *ui;
    QString btAddress;
    QString currentNumber;
    QString state;
    QDBusInterface *callManager;
    QDBusInterface *call;
    bool connected;
    void connectToOfono();
};

#endif // PHONECALLVIEW_H
