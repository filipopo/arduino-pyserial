from sys import exit
from os import strerror
from time import sleep
from serial import Serial, SerialException
from serial.tools.list_ports import comports

class arduino:
    def __init__(self, COM = None, baudrate = 19200):
        if COM == None:
            try:
                COM = comports()[0][0]
                if len(comports()) > 1:
                    print('Multiple serial devices detected, using', COM)
            except IndexError:
                print('No serial devices connected')
                sleep(1)
                exit()

        try:
            self.conn = Serial(COM, baudrate = baudrate, timeout = 1)
        except SerialException as ex:
            print('Error on', COM + ':', strerror(ex.errno))
            sleep(1)
            exit()

        self.switcher = {
            'pinmode': lambda dry: self.transcode(2, dry = dry),
            'pinwrite': lambda dry: self.transcode(3, dry = dry),
            'pinread': lambda dry: self.transcode(2, dry = dry),
            'delay': lambda dry: self.transcode(1, dry = dry),
            'echo': lambda dry: self.transcode(bcount = 1, dry = dry),
            'reset': lambda dry: self.transcode(0, dry = dry),
            'attachinterrupt': lambda dry: self.transcode(2, 1, dry),
            'detachInterrupt': lambda dry: self.transcode(1, dry = dry),
            'delaymicroseconds': lambda dry: self.transcode(1, dry = dry),
            'read': lambda dry: self.read(),
            'help': lambda dry: self.helpf()
        }

        sleep(1)

    def transcode(self, arr = None, bcount = 0, dry = 0):
        temp = self.cc
        temp[0] = list(self.switcher.keys()).index(temp[0])

        if bcount:
            temp.insert(1, self.bcount)
            if arr != None:
                arr += 1

        if arr != None:
            if arr != len(temp[1:]):
                raise KeyError

            replacer = {
                'input_pullup': 2,
                'output': 1,
                'input': 0,
                'high': 1,
                'low': 0,
                'change': 1,
                'falling': 2,
                'rising': 3,
                'led_builtin': 13,
                'analog': 1,
                'digital': 0
            }

            for i in range(1, len(temp)):
                for j in replacer.keys():
                    if temp[i] == j:
                        temp[i] = replacer[j]
                        break
                temp[i] = int(temp[i])

            temp = bytes(temp)
        else:
            temp[0] = chr(temp[0])
            if bcount:
                temp[1] = chr(temp[1])
            temp = ''.join(temp).encode('ascii')

        if dry:
            return temp
        else:
            self.conn.write(temp)

    def read(self):
        sleep(0.1)
        if self.conn.in_waiting:
            temp = self.conn.readline().decode('ascii')
            print(temp)

    def helpf(self):
        print('No help')

    def send(self, temp):
        temp = temp.lower()
        temp = temp.split(';')

        self.cc = temp
        for i in temp:
            self.bcount = 0
            if i.startswith('echo'):
                self.cc = i.split(maxsplit = 1)
                self.bcount = len(self.cc[1])
            else:
                self.cc = i.split()

            try:
                if i.startswith('attachinterrupt'):
                    for j in range(temp.index(i) + 1, len(temp)):
                        bk = self.cc
                        self.cc = temp[j].split()
                        self.bcount += len(self.switcher[self.cc[0]](1))
                        self.cc = bk

                self.switcher[self.cc[0]](0)
            except (KeyError, ValueError):
                print('Invalid command,', i)
            except IndexError:
                print('Write a command')
        self.read()

if __name__ == '__main__':
    a = arduino()
    try:
        while 1:
            temp = input('Input command:\n')
            a.send(temp)
    except KeyboardInterrupt:
        pass
