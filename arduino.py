from sys import exit
from time import sleep
from os import strerror
from serial import Serial, SerialException
from serial.tools.list_ports import comports

class arduino:
    def __init__(self, COM = None, baudrate = 19200):
        if COM == None:
            try:
                COM = comports()[0][0]
                if len(comports()) > 1:
                    print('Multiple serial devices detected')
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
            'pinmode': lambda cc, dry: self.transcode(cc, 2, dry),
            'pinwrite': lambda cc, dry: self.transcode(cc, 3, dry),
            'pinread': lambda cc, dry: self.transcode(cc, 2, dry),
            'delay': lambda cc, dry: self.transcode(cc, 1, ry),
            'echo': lambda cc, dry: self.transcode(cc, dry = dry),
            'reset': lambda cc, dry: self.transcode(cc, 0, dry),
            'attachinterrupt': lambda cc, dry: self.transcode(cc, 3, dry),
            'detachInterrupt': lambda cc, dry: self.transcode(cc, 1, dry),
            'delaymicroseconds': lambda cc, dry: self.transcode(cc, 1, dry),
            'write': lambda cc, dry: self.transcode(cc, 1, dry),
            'setinterrupt': lambda cc, dry: self.transcode(cc, 1, dry),
            'runinterrupt': lambda cc, dry: self.transcode(cc, 2, dry),
            'read': lambda cc, dry: self.read(),
            'help': lambda cc, dry: self.helpf()
        }
        sleep(1)

    def transcode(self, temp, arr = None, dry = 0):
        if type(temp) != list:
            temp = [temp]
        temp[0] = list(self.switcher.keys()).index(temp[0])

        bcount = 0
        if temp[0] == 4:
            bcount = len(temp[1])
        elif temp[0] in [3, 8] and temp[1] < '256':
            temp.insert(1, 0)
            arr += 1
        elif temp[0] == 10:
            for j in range(1, len(self.ccs)):
                bcount += len(self.send(self.ccs[j], 1))

        if bcount:
            temp.insert(1, bcount)
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
                'digital': 0,
                'forever': 1,
                'once': 0
            }

            for i in range(0, len(temp)):
                for j in replacer.keys():
                    if temp[i] == j:
                        temp[i] = replacer[j]
                        break

                temp[i] = int(temp[i])
                if temp[i] < 256:
                    arr = 1
                else:
                    arr = 2
                temp[i] = temp[i].to_bytes(arr, 'big')

            temp = b''.join(temp)
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
        while self.conn.in_waiting:
            print(self.conn.readline()[:-1].decode('ascii'))

    def helpf(self):
        print('No help')

    def send(self, ccs, sync = 1, dry = 0):
        ccs = ccs.split(';')
        if not dry:
            self.ccs = ccs
        for i in ccs:
            if 'echo' in i:
                i = i.split(maxsplit = 1)
            else:
                i = i.lower()
                i = i.split()

            try:
                ccs = self.switcher[i[0]](i, dry)
                if dry:
                    return ccs

            except (KeyError, ValueError):
                print('Invalid command,', i)
                if 'setinterrupt' in i:
                    break
            except IndexError:
                print('Write a command')

if __name__ == '__main__':
    a = arduino()
    try:
        while 1:
            a.send(input(a.conn.port + '>'))
            a.read()
    except KeyboardInterrupt:
        a.conn.close()
