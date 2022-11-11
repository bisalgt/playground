#!/bin/sh
echo "--start the fastapi-server--"
uvicorn main:app --host 0.0.0.0 --port 8000
exec "$@"