#!/usr/bin/python3

from socket import *

sock = socket()
sock.bind(('', 1752))
sock.listen(10)

while True:
    connection, addr = sock.accept()
    
    message = connection.recv(128)
    
    connection.send("ok".encode())
    
    print(addr[0] + ":" + str(addr[1]), message.decode())
                
    connection.close()
