#!/usr/bin/expect -f
spawn ssh "root@192.168.178.25"
expect "*assword: "
send "root\r"
expect "$ "
send "ls\r"
expect "$ "

send "systemctl start redpitaya_scpi &\r"
expect "$ "
send "systemctl status redpitaya_scpi \r"
expect "$ "
send "exit\r"