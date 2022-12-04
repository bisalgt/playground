import socket


# s.connect((socket.gethostname(), 1255))

# msg = s.recv(1024)

for i in range(17):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((socket.gethostname(), 1444))
    msg = s.recv(10)
    print(msg.decode("utf-8"))
    s.close()

# while True:
#     full_msg = ''
#     new_msg = True
#     while True:
#         msg = s.recv(16)
#         if new_msg:
#             print(f"new message length : {msg[:HEADERSIZE]}")
#             msglen = int(msg[:HEADERSIZE])
#             new_msg = False
        
#         full_msg += msg.decode("utf-8")
        
#         if len(full_msg) - HEADERSIZE == msglen:
#             print("full msg received")
#             print(full_msg[HEADERSIZE:])
#             new_msg = True
#             full_msg = ""
    
#     print(full_msg)

