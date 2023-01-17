import socket

msgFromClient       = "-v"

bytesToSend         = str.encode(msgFromClient)

serverAddressPort   = ("192.168.178.25", 61231)

bufferSize          = 1024

# Create a UDP socket at client side
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Send to server using created UDP socket

UDPClientSocket.sendto(bytesToSend, serverAddressPort)

 

msgFromServer, addressFromServer = UDPClientSocket.recvfrom(bufferSize)

print(addressFromServer)

message = msgFromServer.decode()

msg = "Message from Server {}".format(message)

print(msg)



UDPClientSocket.close()