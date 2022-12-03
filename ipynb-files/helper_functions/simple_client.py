import socket


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((socket.gethostname(), 1245))

msg = s.recv(1024).decode('utf-8')
print(msg)

