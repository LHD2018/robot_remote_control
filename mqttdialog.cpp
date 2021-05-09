#include "mqttdialog.h"
#include "ui_mqttdialog.h"

MQTTDialog::MQTTDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MQTTDialog)
{
    ui->setupUi(this);

    mqtt_config = new QSettings(QCoreApplication::applicationDirPath() + "/other/config.ini", QSettings::IniFormat);

    // 读取mqtt配置

    ui->mqtt_ip_text->setText(mqtt_config->value("/MQTT/ip").toString());
    ui->mqtt_port_text->setText(mqtt_config->value("/MQTT/port").toString());
    ui->mqtt_id_text->setText(mqtt_config->value("/MQTT/id").toString());
    ui->mqtt_name_text->setText(mqtt_config->value(("/MQTT/username")).toString());
    ui->mqtt_pass_text->setText(mqtt_config->value("/MQTT/password").toString());

    connect(ui->apply_btn, &QPushButton::clicked, [=](){
        mqtt_config->setValue("/MQTT/ip", ui->mqtt_ip_text->text());
        mqtt_config->setValue("/MQTT/port", ui->mqtt_port_text->text());
        mqtt_config->setValue("/MQTT/id", ui->mqtt_id_text->text());
        mqtt_config->setValue("/MQTT/username", ui->mqtt_name_text->text());
        mqtt_config->setValue("/MQTT/password", ui->mqtt_pass_text->text());
    });

}

MQTTDialog::~MQTTDialog()
{
    delete mqtt_config;
    delete ui;
}
