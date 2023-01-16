import subprocess

username = 'rooot'
password = 'root'

host = '192.168.178.25'

shell_cmd = 'expect "rooot@192.168.178.25\'s password: \r;"send --"root\r";'

command = f"sshpass -p {password} ssh {username}@{host} {shell_cmd}"

# subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

test_cmd = f"ssh {username}@{host} {shell_cmd}"

print(subprocess.getoutput(test_cmd))