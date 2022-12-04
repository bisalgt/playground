import socket
import threading
import select
import time

max_clients = 3
port_number = 1444


def handle_client(client, address, msg):
    print("++++++++++inside handle client ++++++++++++++++++++++++++++++++++")
    print("++++++++++++++++++++++++++++++++++++++++++++")
    print("++++++++++++++++++++++++++++++++++++++++++++")
    client.send(bytes(msg, "utf-8"))
    # request_bytes = b"" + client.recv(1024)

    # if not request_bytes:
    #     print("Connection closed")
    client.close()
    # request_str = request_bytes.decode()
    # print(request_str)

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("0.0.0.0", port_number))
server_socket.listen(max_clients)

inputs = [server_socket]
outputs = []

t1 = time.perf_counter()
count = 0

while True:
    t2 = time.perf_counter()
    if t2 - t1 >= 1:
        count += 1
        t1 = t2
        print("increase counter --------------------------------")
    print("inside while : ", count)
    try:
        readable, writable, exceptional = select.select(
        inputs, outputs, inputs, 0)
    except Exception as e:
        print(e)
        print(inputs.pop())
    print("inputs  ", inputs)
    print("outputs  ", outputs)
    print()
    print()
    print()
    for s in readable:
            if s is server_socket:
                connection, client_address = s.accept()
                connection.setblocking(0)
                inputs.append(connection)
                threading.Thread(target=handle_client, args=(connection, client_address, str(count))).start()