FROM python:3.9-slim-bullseye

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY src/SerialToMqtt .

CMD [ "python", "app.py" ]