import datetime
import logging
import os
from time import sleep

from pymodbus.client.sync import ModbusSerialClient as ModbusClient
import xively

logging.basicConfig()
log = logging.getLogger()
log.setLevel(logging.DEBUG)

FEED_ID = 750603513
API_KEY = os.environ['XIVELY_API_KEY']
DEBUG = True

REGISTERS = [
    'HUMIDITY',
    'TEMPERATURE_C',
    'TEMPERATURE_F',
]

if __name__ == '__main__':
    api = xively.XivelyAPIClient(API_KEY)
    feed = api.feeds.get(FEED_ID)
    client = ModbusClient(
        method='rtu',
        port='COM11',
        baudrate=9600,
        timeout=2
    )
    print("Client Connected: %s" % client.connect())
    try:
        while True:

            rr = client.read_holding_registers(0, len(REGISTERS), unit=100)
            if rr is None:
                continue
            print("Exception Response: %s" % (int(rr.function_code) > 0x80, ))
            for i in range(len(REGISTERS)):
                print("Register %s: %s" % (REGISTERS[i], rr.registers[i]))

            print("Updating Feed")
            feed.datastreams = [
                xively.Datastream(id='Humidity',
                    current_value = rr.registers[0] / 100.,
                    at = datetime.datetime.utcnow()),
                xively.Datastream(id='Temperature_C',
                    current_value = rr.registers[1] / 100.,
                    at = datetime.datetime.utcnow()),
                xively.Datastream(id='Temperature_F',
                    current_value = rr.registers[2] / 100.,
                    at = datetime.datetime.utcnow()),
            ]
            feed.update()
            sleep(60.0)
    finally:
        client.close()
