#!/usr/bin/python3

from socket import *
from datetime import datetime
from time import sleep


while True:
    sock = socket()
    sock.connect(('127.0.0.1', 1752))
    
    current_time = datetime.now().strftime("%A, %d %B %Y, %X")

    while True:
        sock.send(current_time.encode())
        responce = sock.recv(128).decode()
        
        if responce and responce == "ok":
            print("sended \'{}\' to 127.0.0.1:1752".format(current_time))
            break;
        
        else:
            print("sending error: \'{}\'".format(current_time))
        
        
    sock.close()
    
    sleep(60)
