#ifndef MQTTDIALOG_H
#define MQTTDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class MQTTDialog;
}

class MQTTDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MQTTDialog(QWidget *parent = nullptr);
    ~MQTTDialog();

private:
    Ui::MQTTDialog *ui;

    QSettings *mqtt_config;

};

#endif // MQTTDIALOG_H
