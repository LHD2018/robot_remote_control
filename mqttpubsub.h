#ifndef MQTTPUBSUB_H
#define MQTTPUBSUB_H

/*
 *  自定义mqtt 类
 */

#include <qmqtt.h>
#include <QObject>

const QHostAddress EXAMPLE_HOST = QHostAddress::LocalHost;
const quint16 EXAMPLE_PORT = 1883;

class MQTTPubSub : public QMQTT::Client
{
    Q_OBJECT
public:
    explicit MQTTPubSub(const QHostAddress& host = EXAMPLE_HOST, const quint16 port = EXAMPLE_PORT, QObject* parent = NULL): QMQTT::Client(host, port, parent){
        _number = 0;
    }

    ~MQTTPubSub();

    void mqttConnect();
    void mqttPublish(QString topic, QString content);
    void mqttSubcribe(QString topic);
    void mqttDisconnect();

signals:

private:
    qint16 _number;

};

#endif // MQTTPUBSUB_H
