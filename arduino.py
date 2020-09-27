from sys import argv, exit
from time import sleep
from os import strerror
from serial import Serial, SerialException
from serial.tools.list_ports import comports

class arduino:
    def __init__(self, COM = None, baudrate = 9600):
        if COM == None:
            if len(argv) > 1:
                COM = argv[1]
            else:
                if len(comports()) < 2:
                    try:
                        COM = comports()[0][0]
                    except IndexError:
                        print('No serial devices connected')
                        sleep(1)
                        exit()
                else:
                    for i in comports():
                        print(i)
                    COM = input('Enter port:\n')

        try:
            self.conn = Serial(COM, baudrate = baudrate, timeout = 1)
        except SerialException as ex:
            if ex.errno == None:
                err = 'No such device available'
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
            'runinterrupt': 2,
            'pinchange': 1,
            'pinclick': 2
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

        arr = self.switcher[cc[0]]
        cc[0] = list(self.switcher.keys()).index(cc[0])
        if cc[0] in [3, 8] and int(cc[1]) < 256:
            cc.insert(1, 0)
            arr += 1
        elif cc[0] == 10:
            bcount = len(b''.join(self.send(';'.join(self.ccs[1:]), 1)))
        elif cc[0] == 13 and int(cc[2]) < 256:
            cc.insert(2, 0)
            arr += 1

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
                'a0': 14,
                'a1': 15,
                'a2': 16,
                'a3': 17,
                'a4': 18,
                'a5': 19,
                'analog': 1,
                'digital': 0
            }

            for i in range(0, len(cc)):
                for j in replacer.keys():
                    if cc[i] == j:
                        cc[i] = replacer[j]
                        break

                cc[i] = int(cc[i])
                arr = 1 + int(cc[i] > 255)
                cc[i] = cc[i].to_bytes(arr, 'big')

            cc = b''.join(cc)
        else:
            for i in range(1 + int(bcount > 0)):
                cc[i] = chr(cc[i])
            cc = ''.join(cc).encode('ascii')

        if dry:
            return cc
        else:
            self.conn.write(cc)
            return 0

    def read(self):
        sleep(0.1)
        while self.conn.in_waiting:
            print(self.conn.readline()[:-1].decode('ascii'))
            sleep(0.1)

    def help(self):
        print('No help')

    def send(self, ccs, dry = 0):
        ccs = ccs.split(';')
        self.ccs = ccs
        ccs = []
        for cc in self.ccs:
            try:
                if cc.split(maxsplit = 1)[0] in self.switcher.keys():
                    ccs.append(self.transcode(cc, dry))
                else:
                    getattr(self, cc)()
            except (AttributeError, ValueError, TypeError):
                print('Invalid command')
                ccs.append(1)
                if 'setinterrupt' in cc:
                    break
            except IndexError:
                print('Write a command')

        return ccs

if __name__ == '__main__':
    a = arduino()
    try:
        while 1:
            a.send(input(a.conn.port + '>'))
            a.read()
    except KeyboardInterrupt:
        a.conn.close()
