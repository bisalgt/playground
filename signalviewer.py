import numpy as np
import matplotlib.pyplot as plt
import socket
import struct

# get the signal from redpitaya
buffer_size = 65536
msg_from_client = "-a 1"
bytes_to_send = str.encode(msg_from_client)
server_address_port = ("192.168.128.1", 61231)
# Create a UDP socket at client side
udp_client_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
# Send to server using created UDP socket
udp_client_socket.sendto(bytes_to_send, server_address_port)

def get_data_from_sensor(udp_client_socket, buffer_size):
    packet = udp_client_socket.recv(buffer_size)
    print(f"Total Received : {len(packet)} Bytes.")
    header_length = int(struct.unpack('@f', packet[:4])[0])
    ultrasonic_data_length = int(struct.unpack('@f', packet[4:8])[0])
    header_data = []
    for i in struct.iter_unpack('@f', packet[:header_length]):
        header_data.append(i[0])
    print("Header_Length : ", header_length)
    print("Header_Data : ", header_data)
    ultrasonic_data = []
    for i in struct.iter_unpack('@h', packet[header_length:]):
        ultrasonic_data.append(i[0])
    print("Ultrasonic Data Length : ", ultrasonic_data_length)
    # print("Ultrasonic Data : ", ultrasonic_data)
    print(len(ultrasonic_data) + len(header_data))
    print(f"Length of Header : {len(header_data)}")
    print(f"Length of Ultrasonic Data : {len(ultrasonic_data)}")
    # returning the ultrasonic raw adc data from the sensor.
    return ultrasonic_data

def plot_signal(x, y, fig_size = (10, 6), title="ADC Signal", extracted_signal=False):
    # Plot the entire ADC signal
    plt.figure(figsize=fig_size)
    plt.plot(x, y, label=title)
    plt.title(title)
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.grid(True)
    if extracted_signal:
        plt.show()

# Use ginput to interactively select a region
print("Click on the plot to select start and end points.")
while True:
    # plt.close()
    raw_adc_data = get_data_from_sensor(udp_client_socket, buffer_size)
    t = [i for i in range(len(raw_adc_data))]
    plot_signal(x=t, y=raw_adc_data)

    #waiting for selecting two points in the graph
    points = plt.ginput(n=2, timeout=0, show_clicks=True)
    # Extract the selected region
    if len(points) == 2:
        start_index, end_index = sorted([int(point[0]) for point in points])
        print(start_index, end_index)
        extracted_signal = raw_adc_data[start_index:end_index]
        xaxis_extracted_sig = [i for i in range(len(extracted_signal))]

        plot_signal(x=xaxis_extracted_sig, y=extracted_signal, fig_size=(8,4), title="Extracted ADC Signal", extracted_signal=True)
    else:
        print("Selection canceled.")

plt.show()


udp_client_socket.close()