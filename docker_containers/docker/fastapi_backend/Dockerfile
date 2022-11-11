# syntax=docker/dockerfile:1

FROM python:3.9
ENV PYTHONUNBUFFERED 1
WORKDIR /app
COPY ./app/requirements.txt /app/requirements.txt
COPY ./docker/fastapi_backend/entrypoint.sh /app/entrypoint.sh
RUN pip install --upgrade pip
RUN pip install -r requirements.txt

COPY ./app /app

RUN sed -i 's/\r$//g' /app/entrypoint.sh
RUN chmod +x /app/entrypoint.sh

ENTRYPOINT ["/app/entrypoint.sh"]
