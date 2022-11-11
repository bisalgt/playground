#!/bin/sh
mysql -u root --password="$MYSQL_ROOT_PASSWORD"  << EOF
USE ${MYSQL_DATABASE};
# for testing 
# GRANT ALL PRIVILEGES ON  test_${MYSQL_DATABASE}.* TO '${MYSQL_USER}';
# for the cron job
GRANT PROCESS ON *.* TO '${MYSQL_USER}'@'%';
EOF
