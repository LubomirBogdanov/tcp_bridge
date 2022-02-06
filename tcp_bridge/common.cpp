#include "common.h"

using namespace std;

void display_byte_array(QByteArray &hex_data){
    qDebug()<<"======================================";
    qDebug()<<hex_data.toHex(' ');
    qDebug() <<"\n--------------------------------------\n";
    qDebug()<<hex_data;
    qDebug()<<"\n======================================\n";
}
