import serial
import json

RETRIES = 5
RESPONSE_CODE_LEN = 5

# width 50
# height 40


def send(ser_: serial.Serial, msg: bytes):
    print(msg)
    if not ser_.isOpen():
        print("opening the port")
        ser_.open()
        error_code_ = b''
        while error_code_ == b'':
            error_code_ = ser_.read(RESPONSE_CODE_LEN)
        print("port opened: " + str(error_code_))

    for retry in range(RETRIES):
        ser_.write(msg)
        error_code_ = ser_.read(RESPONSE_CODE_LEN)
        if error_code_ != b'':
            return error_code_

        print(f"try {retry + 1}/{RETRIES}")

    print("message failed")


def move_absolute(positions: list):
    mode = b'\x00'
    command = b'\x01'

    msg = mode + command
    for position in positions:
        msg = msg + abs(position).to_bytes(1, "big")

    print(f"got: " + str(send(ser, msg)))

def move_relative(positions: list):
    if positions[0] > 255 or positions[1] > 255:
        return

    mode = b'\x00'
    command = b'\x02'

    msg = mode + command
    for position in positions:
        if position < 0:
            msg = msg + b'\x00'
        else:
            msg = msg + b'\xff'
        msg = msg + abs(position).to_bytes(1, "big")

    print(f"got: " + str(send(ser, msg)))


ser = serial.Serial()
with open("port.json", "r") as port_file:
    port_data = json.loads(port_file.read())
    ser.port = port_data['port']
    ser.baudrate = port_data['baudrate']
    if port_data['timeout'] != -1:
        ser.timeout = port_data['timeout']

if __name__ == '__main__':
    import time

    move_absolute([50, 90])
    time.sleep(1)
    move_relative([0, -40])