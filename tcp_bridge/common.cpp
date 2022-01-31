#include "common.h"

using namespace std;

void display_byte_array(QByteArray &hex_data){
    QChar hex_value;

    qDebug()<<"======================================";
    for(int i = 0; i < hex_data.size(); i++){
        hex_value = hex_data.at(i);
        cout << QString::asprintf("%02X ", hex_value.toLatin1()).toStdString();
    }
    cout <<"\n--------------------------------------\n";
    for(int i = 0; i < hex_data.size(); i++){
        hex_value = hex_data.at(i);
        cout << QString::asprintf("%c", hex_value.toLatin1()).toStdString();
    }
    qDebug()<<"\n======================================\n";
}
