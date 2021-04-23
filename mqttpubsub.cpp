#include "mqttpubsub.h"

void MQTTPubSub::mqttConnect(){
    if(isConnectedToHost()){
        return;
    }
    connectToHost();
}

void MQTTPubSub::mqttPublish(QString topic, QString content){
    QMQTT::Message message(_number, topic, content.toUtf8());
    publish(message);
    _number++;
}

void MQTTPubSub::mqttSubcribe(QString topic){
    subscribe(topic, 1);
}

void MQTTPubSub::mqttDisconnect(){
    if(isConnectedToHost()){
        disconnectFromHost();
    }
}

MQTTPubSub::~MQTTPubSub(){
    mqttDisconnect();
}
