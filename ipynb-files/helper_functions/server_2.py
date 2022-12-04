import socket
import time
import threading

HEADERSIZE = 10

def accept_and_send_msg(server_socket, count):
    clientsocket, address = server_socket.accept()
    clientsocket.send(bytes(msg, "utf-8"))
    clientsocket.close()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((socket.gethostname(), 1255))
s.listen(5)

print("starting server!")



t1 = time.perf_counter()
count = 0
try:
    while True:
        
        
        t2 = time.perf_counter()

        if t2 - t1 >= 1:
            print("increasing counter ------", count)
            count += 1
            t1 = t2
        
        
        clientsocket, address = s.accept()
        print(f"Connection from {address} has been established!")
        msg = str(count)
except FileNotFoundError as e:
    print(e)
    s.close()
