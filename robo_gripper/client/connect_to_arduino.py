import serial
import json

RETRIES = 5

LOOK_AT_ME = [60, 50, 180, 50]


def send(ser_: serial.Serial, msg: bytes):
    print(msg)
    if not ser_.isOpen():
        print("opening the port")
        ser_.open()
        error_code_ = b''
        while error_code_ == b'':
            error_code_ = ser.read(3)
        print("port opened")

    for retry in range(RETRIES):
        ser_.write(msg)
        error_code_ = ser_.read(3)
        if error_code_ != b'':
            return error_code_

        print(f"try {retry + 1}/{RETRIES}")

    print("message failed")


def up(ser_: serial.Serial, n: bytes):
    return send(ser_, b'u' + n)


def down(ser_: serial.Serial, n: bytes):
    return send(ser_, b'd' + n)

def set_height(ser_: serial.Serial, n: bytes):
    return send(ser_, b'h' + n)

def set_angle(ser_: serial.Serial, n: bytes):
    return send(ser_, b'a' + n)

def set_motor_angle(ser_: serial.Serial, motor: bytes, angle: bytes):
    return send(ser_, b's' + motor + angle)

def set_position(ser_: serial.Serial, angles: tuple):
    pass

def get_position(ser_: serial.Serial):
    error_code = send(ser_, b'g')
    if error_code != b'200':
        return error_code

    length_bytes = ser_.read(1)
    length = int.from_bytes(length_bytes, 'big')
    positions = list(ser_.read(length))

    return positions


ser = serial.Serial()
with open("port.json", "r") as port_file:
    port_data = json.loads(port_file.read())
    ser.port = port_data['port']
    ser.baudrate = port_data['baudrate']
    if port_data['timeout'] != -1:
        ser.timeout = port_data['timeout']

if __name__ == '__main__':
    while True:

        for i in reversed(list(range(15))):
            print("up: ", i*12, set_angle(ser, (i*12).to_bytes(1, "big")))
            print(get_position(ser))

        for i in range(15):
            print("up: ", i*12, set_angle(ser, (i*12).to_bytes(1, "big")))
            print(get_position(ser))


        # input_ = input("--")
        # set_angle(ser, motor=b'\x03', angle=int(input_).to_bytes(1, 'big'))

        print(get_position(ser))
