import sys
import time
import redpitaya_scpi as scpi

rp_s = scpi.scpi('192.168.178.25')

# print(rp_s)


# print(dir(rp_s))

try:
    print(rp_s.host)
    # print(rp_s)
except:
    print("Exception Occured!")


print(rp_s.close())