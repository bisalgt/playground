#!/bin/sh
echo "--start the fastapi-server--"
cd ./app
python main.py
exec "$@"