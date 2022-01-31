/*
    Copyright (C) 2021 Lubomir Bogdanov
    Contributor Lubomir Bogdanov <lbogdanov@tu-sofia.bg>
    This file is part of tcp_bridge.
    Powot Simulator is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    tcp_bridge is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with tcp_bridge.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "tcp_server.h"

tcp_server::tcp_server(){
    tcpsrv = nullptr;
    client_sock_0 = nullptr;
    client_sock_1 = nullptr;
    client_0_connected = 0;
    client_1_connected = 0;
    client_last_active = 0;
    client_mode = 0;
}

tcp_server::~tcp_server(){
    if(tcpsrv){
        delete tcpsrv;
        tcpsrv = nullptr;
    }

    close_client_socket_0();
    close_client_socket_1();
}

int tcp_server::start_server(QString &ip, QString &port, bool disconn_event, bool cli_mode){
    bool ok;
    bool addr_found = 0;
    QList<QHostAddress> ipAddressesList;
    QHostAddress ip_host(ip);

    disconnect_event = disconn_event;

    client_mode = cli_mode;

    if(!tcpsrv){
        tcpsrv = new QTcpServer;
    }

    if(ip.isEmpty()){
        qDebug()<<"(tcp_server) Host ip not specified. Trying to guess ip ...";
        ipAddressesList = QNetworkInterface::allAddresses();
        // use the first non-localhost IPv4 address
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
                ip_host.setAddress(ipAddressesList.at(i).toString());
                qDebug()<<"(tcp_server) IP found: "<<ipAddressesList.at(i).toString();
                addr_found = 1;
                break;
            }
        }
        // if we did not find one, use IPv4 localhost
        if (!addr_found){
            qDebug()<<"(tcp_server) IP found: localhost";
            ip_host.setAddress(QHostAddress::LocalHost);
        }
    }
    else{
        if(ip == "localhost"){
            ip_host.setAddress("127.0.0.1");
        }
    }

    if (!tcpsrv->listen(ip_host, port.toUInt(&ok, 10))) {
        qDebug()<<"(tcp_server) Unable to start server!";
        return EXIT_FAILURE;
    }
    connect(tcpsrv, &QTcpServer::newConnection, this, &tcp_server::on_new_connection);
    qDebug()<<"(tcp_server) running on IP: "<<tcpsrv->serverAddress().toString()<<"port: "<<tcpsrv->serverPort();
    qDebug()<<"(tcp_server) disconnect event bridging: "<<disconnect_event;

    return EXIT_SUCCESS;
}

void tcp_server::on_new_connection(void){
    QTcpSocket *temp_client_sock;

    if(client_0_connected && client_1_connected){
        qDebug()<<"(tcp_server) New connection! (x)";
        qDebug()<<"Aborting connection, 2 clients already connected!";
        temp_client_sock = tcpsrv->nextPendingConnection();
        temp_client_sock->disconnectFromHost();
        temp_client_sock = nullptr;
    }
    else{
        if(!client_0_connected){
            qDebug()<<"(tcp_server) New connection! (0)";

            client_0_connected = 1;

            client_sock_0 = tcpsrv->nextPendingConnection();

            if(client_sock_0){
                connect_signals(0);
            }
        }
        else if(!client_1_connected){
            qDebug()<<"(tcp_server) New connection! (1)";

            client_1_connected = 1;

            client_sock_1 = tcpsrv->nextPendingConnection();

            if(client_sock_1){
                connect_signals(1);
            }
        }
    }
}

void tcp_server::connect_signals(int client_number){
    switch(client_number){
    case 0:
        connect(client_sock_0, SIGNAL(disconnected()), client_sock_0, SLOT(deleteLater()));
        connect(client_sock_0, SIGNAL(disconnected()), this, SLOT(on_closed_connection_0()));
        connect(client_sock_0, SIGNAL(readyRead()), this, SLOT(on_ready_read_0()));
        break;
    case 1:
        connect(client_sock_1, SIGNAL(disconnected()), client_sock_1, SLOT(deleteLater()));
        connect(client_sock_1, SIGNAL(disconnected()), this, SLOT(on_closed_connection_1()));
        connect(client_sock_1, SIGNAL(readyRead()), this, SLOT(on_ready_read_1()));
        break;
    }
}

void tcp_server::disconnect_signals(int client_number){
    switch(client_number){
    case 0:
        if(client_sock_0){
            disconnect(client_sock_0, SIGNAL(disconnected()), client_sock_0, SLOT(deleteLater()));
            disconnect(client_sock_0, SIGNAL(disconnected()), this, SLOT(on_closed_connection_0()));
            disconnect(client_sock_0, SIGNAL(readyRead()), this, SLOT(on_ready_read_0()));
            client_sock_0->disconnectFromHost();
            client_sock_0 = nullptr;
        }
        break;
    case 1:
        if(client_sock_1){
            disconnect(client_sock_1, SIGNAL(disconnected()), client_sock_1, SLOT(deleteLater()));
            disconnect(client_sock_1, SIGNAL(disconnected()), this, SLOT(on_closed_connection_1()));
            disconnect(client_sock_1, SIGNAL(readyRead()), this, SLOT(on_ready_read_1()));
            client_sock_1->disconnectFromHost();
            client_sock_1 = nullptr;
        }
        break;
    }
}

void tcp_server::close_client_socket_0(){
    qDebug() << "(tcp_server) close_client_socket (0)";
    client_0_connected = 0;
    client_last_active = 1;
    disconnect_signals(0);
}

void tcp_server::on_closed_connection_0(){
    qDebug()<<"(tcp_server) Connection closed! (0)";
    close_client_socket_0();

    if(disconnect_event){
        close_client_socket_1();

        if(client_mode){
            emit client_closed_the_connection();
        }
    }
}

void tcp_server::on_ready_read_0(){
    QByteArray recv_msg;

    client_last_active = 0;

    recv_msg = client_sock_0->readAll();

    qDebug()<<"(tcp_server) (0)";
    display_byte_array(recv_msg);

    if(client_mode){
        emit send_to_remote_server(recv_msg);
    }else{
        send_to_client(recv_msg, 1);
    }
}

void tcp_server::close_client_socket_1(){
    qDebug() << "(tcp_server) close_client_socket (1)";
    client_1_connected = 0;
    client_last_active = 0;
    disconnect_signals(1);
}

void tcp_server::on_closed_connection_1(){
    qDebug()<<"(tcp_server) Connection closed! (1)";
    close_client_socket_1();

    if(disconnect_event){
        close_client_socket_0();

        if(client_mode){
            emit client_closed_the_connection();
        }
    }
}

void tcp_server::on_ready_read_1(){
    QByteArray recv_msg;

    client_last_active = 1;

    recv_msg = client_sock_1->readAll();

    qDebug()<<"(tcp_server) (1)";
    display_byte_array(recv_msg);

    if(client_mode){
        emit send_to_remote_server(recv_msg);
    }else{
        send_to_client(recv_msg, 0);
    }
}

void tcp_server::send_to_client(QByteArray &msg, int client_number){
    switch(client_number){
    case 0:
        if(client_sock_0){
            client_sock_0->write(msg);
        }
        break;
    case 1:
        if(client_sock_1){
            client_sock_1->write(msg);
        }
        break;
    }
}

void tcp_server::on_recv_from_remote_server(QByteArray &msg){
    send_to_client(msg, client_last_active);
}

void tcp_server::on_server_closed_the_connection(){
    if(disconnect_event && client_mode){
        close_client_socket_0();
        close_client_socket_1();
    }
}


