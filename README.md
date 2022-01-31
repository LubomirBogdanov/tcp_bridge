Description  
===================================================================   
A server application that allows two clients to connect to it. The  
packets sent from the first client are sent to the second and vice  
 versa.  

INSTALL  
===================================================================
The project can be built with Qt Creator. Just import it and 
press build.  
  
  
<pre>  
----------------        -----------------        -----------------
               |        |  HOST SERVER  |        |               |
               |        |  (tcp_bridge) |        |               |
               |        |               |        |               |
    CLIENT 1   |------->|-------------->|------->|  CLIENT 2     |
               |<-------|<--------------|<-------|               |
               |        |               |        |               |
               |        |               |        |               |
----------------        -----------------        -----------------
</pre>  

<pre>  
----------------        -----------------        -----------------
               |        |  HOST SERVER  |        | REMOTE SERVER |
               |        |  (tcp_bridge) |        |               |
               |        |  ----------   |        |               |
    CLIENT 1   |------->|--|        |-->|------->|               |
               |<-------|<-|        |---|<-------|               |
               |        |  ----------   |        |               |
               |        |    CLIENT     |        |               |
----------------        -----------------        -----------------
</pre>  

<pre>  
----------------        -----------------        -----------------
               |        |  HOST SERVER  |        | REMOTE SERVER |
               |        |  (tcp_bridge) |        |               |
               |        |  ----------   |        |               |
    CLIENT 1   |------->|--|        |-->|------->|               |
               |<-------|<-|        |---|<-------|               |
               |        |  |        |   |        |               |
               |        |  |        |   |        |               |
----------------        |  |        |   |        -----------------
                        |  |  LAST  |   |
----------------        |  |  ONE   |   |
               |        |  |  USED  |   |        
               |        |  |  MUX   |   |        
               |        |  |        |   |        
    CLIENT 1   |------->|--|        |   |
               |<-------|<-|        |   |
               |        |  ----------   |        
               |        |    CLIENT     |        
----------------        -----------------


</pre>  



Example usage  
===================================================================
**EXAMPLE 1:**  
==============  
Go to the tcp_bridge build directory and type:  

./tcp_bridge -i localhost -p 1234  

!!!Note that there is a space after the -i and -p argument!!!  
    
Then open two separate terminals and try to connect to the server.  
You may use netcat or any other client. For example, using netcat  
the command would look like:  

nc localhost 1234  
  
in the first terminal and again:  
  
nc localhost 1234  
  
in the second terminal. Type text in terminal 1 and it should be  
received in terminal 2. Type text in terminal 2 and it should be  
received in terminal 1. When one of the clients disconnects,  
the other remains connected to the server. If you want to disconnect  
both clients when one of them closes the connection issue the command  

./tcp_bridge -i localhost -p 1234 -d  
  
**EXAMPLE 2:**  
==============  
Start a "remote" server:  

nc 127.0.0.1 5533 -l  

then the tcp_bridge in client mode (version >= 1.4) with the -c argument:  

./tcp_bridge -i 127.0.0.1 -p 1212 -c -r 127.0.0.1 -o 5533 -d  

then connect two clients from two separate terminals:  

nc 127.0.0.1 1212
nc 127.0.0.1 1212

The messages from the server will be re-routed (multiplexed, MUX) to the  
last client that send a message to the server.  




