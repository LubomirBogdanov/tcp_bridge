#include "tcp_client.h"

tcp_client::tcp_client(){
    client_sock = nullptr;
}

tcp_client::~tcp_client(){
    disconnect_signals();
}

void tcp_client::disconnect_signals(){
    if(client_sock){
        disconnect(client_sock, SIGNAL(connected()), this, SLOT(on_connected()));
        disconnect(client_sock, SIGNAL(disconnected()), client_sock, SLOT(deleteLater()));
        disconnect(client_sock, SIGNAL(disconnected()), this, SLOT(on_closed_connection()));
        disconnect(client_sock, SIGNAL(readyRead()), this, SLOT(on_ready_read()));
        disconnect(client_sock, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
        client_sock->disconnectFromHost();
        client_sock = nullptr;
    }
}

int tcp_client::start_client(QString &ip, QString &port, bool disconn_event){
    bool ok;

    if(!client_sock){
        client_sock = new QTcpSocket(this);

        connect(client_sock, SIGNAL(connected()), this, SLOT(on_connected()));
        connect(client_sock, SIGNAL(disconnected()), client_sock, SLOT(deleteLater()));
        connect(client_sock, SIGNAL(disconnected()), this, SLOT(on_closed_connection()));
        connect(client_sock, SIGNAL(readyRead()), this, SLOT(on_ready_read()));
        connect(client_sock, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));

        if(ip.isEmpty()){
            qDebug()<<"(tcp_client) No remote ip specified! Using localhost ...";
            ip = "127.0.0.1";
        }

        if(port.isEmpty()){
            qDebug()<<"(tcp_client) No remote port specified! Using 5533 ...";
            ip = "127.0.0.1";
        }

        client_sock->connectToHost(ip, port.toUInt(&ok, 10));
    }

    disconnect_event = disconn_event;

    return EXIT_SUCCESS;
}

void tcp_client::on_connected(){
    qDebug()<<"(tcp_client) Connected to remote server!";
}

void tcp_client::on_closed_connection(){
    qDebug()<<"(tcp_client) Connection closed!";
    if(disconnect_event){
        emit server_closed_the_connection();
    }
}

void tcp_client::on_ready_read(){
    QByteArray recv_msg;

    recv_msg = client_sock->readAll();

    qDebug()<<"(tcp_client) ";
    display_byte_array(recv_msg);

    emit recv_from_remote_server(recv_msg);
}

void tcp_client::on_error(QAbstractSocket::SocketError err){
    qDebug()<<"(tcp_client) Remote server err: "<<err;
}

void tcp_client::on_send_to_remote_server(QByteArray &msg){
    client_sock->write(msg);
}

void tcp_client::on_client_closed_the_connection(){
    disconnect_signals();
}
