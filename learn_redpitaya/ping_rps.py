import subprocess

hostname = '192.168.178.25'

# output = subprocess.run(f'ping -c 1 {hostname}')


# print(output)
# print(dir(output))


process = subprocess.call(['ping', '-c', '1', f'{hostname}'])

print(process)
# print(dir(process))