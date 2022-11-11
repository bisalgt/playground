FROM mysql:8.0-debian

COPY ./docker/mysql/init_db.sh /docker-entrypoint-initdb.d/
RUN chmod +x /docker-entrypoint-initdb.d/init_db.sh

