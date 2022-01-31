#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include "common.h"

class tcp_client: public QObject{
    Q_OBJECT
private:
    QTcpSocket *client_sock;
    bool disconnect_event;

    void disconnect_signals();

    friend void display_byte_array(QByteArray &hex_data);

public:
    tcp_client();
    ~tcp_client();
    int start_client(QString &ip, QString &port, bool disconn_event);

signals:
    void recv_from_remote_server(QByteArray &msg);
    void server_closed_the_connection();

private slots:
    void on_connected();
    void on_closed_connection();
    void on_ready_read();
    void on_error(QAbstractSocket::SocketError err);
    void on_send_to_remote_server(QByteArray &msg);
    void on_client_closed_the_connection();
};

#endif // TCP_CLIENT_H
