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
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QTcpServer>
#include <QNetworkInterface>
#include <QObject>
#include <QTcpSocket>
#include <QXmlStreamReader>
#include "common.h"

class tcp_server : public QObject{
    Q_OBJECT
private:
    QTcpServer *tcpsrv;
    QTcpSocket *client_sock_0;
    QTcpSocket *client_sock_1;
    int client_0_connected;
    int client_1_connected;
    int client_last_active;
    bool disconnect_event;
    bool client_mode;

    void connect_signals(int client_number);
    void disconnect_signals(int client_number);
    friend void display_byte_array(QByteArray &hex_data);

public:
    tcp_server();
    ~tcp_server();
    int start_server(QString &ip, QString &port, bool disconn_event, bool cli_mode);
    void close_client_socket_0();
    void close_client_socket_1();
    void send_to_client(QByteArray &msg, int client_number);

signals:
    void user_client_connected_0();
    void user_client_connected_1();
    void send_to_remote_server(QByteArray &msg);
    void client_closed_the_connection();

private slots:
    void on_new_connection();
    void on_closed_connection_0();
    void on_ready_read_0();
    void on_closed_connection_1();
    void on_ready_read_1();
    void on_recv_from_remote_server(QByteArray &msg);
    void on_server_closed_the_connection();
};

#endif // TCP_SERVER_H
