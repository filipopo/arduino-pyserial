from sys import exit
from time import sleep
from os import strerror
from serial import Serial, SerialException
from serial.tools.list_ports import comports

class arduino:
    def __init__(self, COM = None, baudrate = 19200):
        if COM == None:
            try:
                if len(comports()) < 2:
                    COM = comports()[0][0]
                else:
                    COM = input('Multiple serial devices detected, enter port:\n')
            except IndexError:
                print('No serial devices connected')
                sleep(1)
                exit()

        try:
            self.conn = Serial(COM, baudrate = baudrate, timeout = 1)
        except SerialException as ex:
            if ex.errno == None:
                err = 'No such device'
            else:
                err = strerror(ex.errno)
            print('Error on', COM + ':', err)
            sleep(1)
            exit()

        self.switcher = {
            'pinmode': 2,
            'pinwrite': 3,
            'pinread': 2,
            'delay': 1,
            'echo': None,
            'reset': 0,
            'attachinterrupt': 3,
            'detachInterrupt': 1,
            'delaymicroseconds': 1,
            'write': 1,
            'setinterrupt': 1,
            'runinterrupt': 2
        }
        sleep(1)

    def transcode(self, cc, dry = 0):
        bcount = 0
        if 'echo' in cc:
            cc = cc.split(maxsplit = 1)
            bcount = len(cc[1])
        else:
            cc = cc.lower()
            cc = cc.split()

        arr = self.switcher.get(cc[0], None)
        cc[0] = list(self.switcher.keys()).index(cc[0])
        if type(cc) == list:
            cc[1] = int(cc[1])
            if cc[0] in [3, 8] and cc[1] < 256:
                cc.insert(1, 0)
                arr += 1
            elif cc[0] == 10:
                bcount = len(b''.join(self.send(';'.join(self.ccs[1:]), 1)))
        else:
            cc = [cc]

        if bcount:
            cc.insert(1, bcount)
            if arr != None:
                arr += 1

        if arr != None:
            if arr != len(cc[1:]):
                raise AttributeError

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
                'digital': 0,
                'forever': 1,
                'once': 0
            }

            for i in range(0, len(cc)):
                for j in replacer.keys():
                    if cc[i] == j:
                        cc[i] = replacer[j]
                        break

                cc[i] = int(cc[i])
                if cc[i] < 256:
                    arr = 1
                else:
                    arr = 2
                cc[i] = cc[i].to_bytes(arr, 'big')

            cc = b''.join(cc)
        else:
            cc[0] = chr(cc[0])
            if bcount:
                cc[1] = chr(cc[1])
            cc = ''.join(cc).encode('ascii')

        if dry:
            return cc
        else:
            self.conn.write(cc)

    def read(self):
        sleep(0.1)
        while self.conn.in_waiting:
            print(self.conn.readline()[:-1].decode('ascii'))

    def help(self):
        print('No help')

    def send(self, ccs, dry = 0):
        ccs = ccs.split(';')
        self.ccs = ccs
        if dry:
            ccs = []
        for i in self.ccs:
            try:
                temp = i.split(maxsplit = 1)[0]
                if temp in self.switcher.keys():
                    temp = self.transcode(i, dry)
                    if dry:
                        ccs.append(temp)
                else:
                    getattr(self, temp)()
            except (AttributeError, ValueError):
                print('Invalid command:')
                if 'setinterrupt' == temp:
                    break
            except IndexError:
                print('Write a command')
        if dry:
            return ccs

if __name__ == '__main__':
    a = arduino()
    try:
        while 1:
            a.send(input(a.conn.port + '>'))
            a.read()
    except KeyboardInterrupt:
        a.conn.close()
