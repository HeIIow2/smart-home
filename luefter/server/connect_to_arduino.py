import serial
import json
from flask import Flask, request, render_template

app = Flask(__name__)

RETRIES = 5

def open_port(ser_: serial.Serial):
    ser.close()
    if not ser_.isOpen():
        print("opening the port")
        ser_.open()
        error_code_ = b''
        while error_code_ == b'':
            error_code_ = ser.read(3)
        print(error_code_)
        print("port opened")

def send(ser_: serial.Serial, msg: bytes):
    print(msg)
    if not ser_.isOpen():
        print("opening the port")
        ser_.open()
        error_code_ = b''
        while error_code_ == b'':
            error_code_ = ser.read(3)
        print(error_code_)
        print("port opened")

    for retry in range(RETRIES):
        ser_.write(msg)
        error_code_ = ser_.read(3)
        if error_code_ != b'':
            return error_code_

        print(f"try {retry + 1}/{RETRIES}")

    print("message failed")

def set_luefter(ser_: serial.Serial, luefter_id: int, speed: int):
    msg = b'\x00' + luefter_id.to_bytes(1, "big") + speed.to_bytes(1, "big")
    error_code = send(ser_, msg)
    print(error_code)


ser = serial.Serial()
with open("port.json", "r") as port_file:
    port_data = json.loads(port_file.read())
    ser.port = port_data['port']
    ser.baudrate = port_data['baudrate']
    if port_data['timeout'] != -1:
        ser.timeout = port_data['timeout']

    # open_port(ser)

@app.route('/')
def my_form():
    return render_template('index.html')

@app.route('/', methods=['POST'])
def my_form_post():
    form = request.form
    for key in form:
        if key.isnumeric() and form[key].isnumeric():
            pass
            # set_luefter(ser, int(key), int(form[key]))

    return render_template('index.html')


if __name__ == '__main__':
    app.run(debug = True, host = "0.0.0.0", port = 3000)
    ser.close()