#!/usr/bin/env python3

import time
# import spidev
import random
import json
import RPi.GPIO as GPIO

PDELAY = 0.0005

RESISTORS = {
    'v5swtch' : [2.4, 9.1],
    'v33'     : [3.9, 9.1],
    'vsensa'  : [6.8, 91.0],
    'vsensb'  : [6.8, 91.0], # not fitted
}

def top16(v):
    return ((v >> 16) & 0xffff) + 0
def bot16(v):
    return (v & 0xffff) + 0

def mulRatio(name, value):
    # internal attiny reference is 1.1V
    # ADC is 10b (0-1023)
    # we want result in mV
    return (1000 * 1.1 * value) / (1024 * RESISTORS[name][0] / sum(RESISTORS[name]))

class PiDog:
    def __init__(self, bus = 0, device = 0):
        self.bus = bus
        self.device = device

        spi = bbSPI()
        #spi = spidev.SpiDev()
        #spi.open(self.bus,self.device)
        #spi.max_speed_hz = 500000
        #spi.mode = 0x3
        self.spi = spi

        self.masks = {
            'wdog_en'     : 0x01,
            'wdog_fired'  : 0x02,
            'wdog_soon'   : 0x04,
            'wake_en'     : 0x08,
            'wake_fired'  : 0x10,
            'power_on'    : 0x20,
            'led_led_warn': 0x40,
        }

        self.regs_by_name = {
            'status'           : {
                'addr': 0,
                'decode': {
                    'wdog_en'     : lambda v: True if v & 0x1  else False,
                    'wdog_fired'  : lambda v: True if v & 0x2  else False,
                    'wdog_soon'   : lambda v: True if v & 0x4  else False,
                    'wake_en  '   : lambda v: True if v & 0x8  else False,
                    'wake_fired'  : lambda v: True if v & 0x10 else False,
                    'power_on  '  : lambda v: True if v & 0x20 else False,
                    'led_warn'    : lambda v: True if v & 0x40 else False,
                },
            },
            'on_remaining'     : {
                'addr': 1,
                'decode': {
                    'on_remaining': lambda v: v + 0,
                },
            },
            'off_remaining'    : {
                'addr': 2,
                'decode': {
                    'off_remaining': lambda v: v + 0,
                },
            },
            'on_rem_resetval'  : {
                'addr': 3,
                'decode': {
                    'on_rem_resetval': lambda v: v + 0,
                },
            },
            'off_rem_resetval' : {
                'addr': 4,
                'decode': {
                    'off_rem_resetval': lambda v: v + 0,
                },
            },
            'temp_v33'             : {
                'addr': 5,
                'decode': {
                    # needs formula
                    'temp_C': lambda v: top16(v) + 0,
                    'v33': lambda v: mulRatio('v33',bot16(v)),
                },
            },
            'vsensa_vsensb'          : {
                'addr': 6,
                'decode': {
                    # Vcc is measured internally relative to 1.1V 
                    # reference. No divider
                    'vsensa': lambda v: mulRatio('vsensa',top16(v)),
                    'vsensb': lambda v: mulRatio('vsensa',bot16(v)),
                },
            },
            'v5_v5swtch'        : {
                'addr': 7,
                'decode': {
                    'v5'     : lambda v: top16(v),
                    'v5swtch': lambda v: mulRatio('v5swtch',bot16(v)),
                },
            },

            'firecounts'       : {
                'addr': 8,
                'decode': {
                    'wdog_events': lambda v: bot16(v),
                    'wake_events': lambda v: top16(v),
                },
            },
            'hw_rev'           : {
                'addr': 9,
                'decode': {
                    'device_id': lambda v: [ (v >> 24) & 0xff, (v >> 16) & 0xff ],
                    'version_minor': lambda v: (v & 0xff) + 0,
                    'version_major': lambda v: ((v >> 8)& 0xff) + 0,
                },
            },
        }

    def __del__(self):
        try:
            self.deinit()
        except Exception as e:
            pass


    def _inout(self,x,y):
        i5 = [ x, 
             (y >> 24) & 0xff,
             (y >> 16) & 0xff,
             (y >>  8) & 0xff,
             y & 0xff ]

        o5 = self.spi.xfer2(i5)
        oc = o5.pop(0)
        ov = 0
        while len(o5):
            ov <<= 8
            ov |= o5.pop(0) & 0xff
        return (oc, ov)

    def _read_reg(self,idx):
        r = self._inout(idx & 0xf, 0)
        # print(' [read from {0:x}] r: {1:x}, c: {2:x}'.format(idx,r[1],r[0]))
        r = self._inout(0, 0)
        # print(' [read from {0:x}] r: {1:x}, c: {2:x}'.format(idx,r[1],r[0]))
        return r[1]

    def _half_read_reg(self,idx):
        r = self._inout(idx & 0xf, 0)
        print(' [read from {0:x}] r: {1:x}, c: {2:x}'.format(idx,r[1],r[0]))
        return r[1]

   
    def _write_reg(self,idx,v,mode = 0x3):
       
        mode = (mode & 0x3) << 6
        r = self._inout(mode | (idx & 0xf), v)
        print(' [write {0:x} to {1:x} mode {2} ] result: {3:x}, result_cmd: {4:x}'.format(v,idx,mode,r[1],r[0]))
        return r[1]

    def fastGetList(self,l):
        prev_i = None
        o = []
        for i in l:
            val = self._half_read_reg(i)
            if prev_i:
                o.append(val)
            prev_i = i
        val = self._half_read_reg(i)
        o.append(val)
        return o

    def _make_result(self, name, val):
        reg = self.regs_by_name[name]

        o = {
            '__raw': val,
            '__rawhex': '{0:x}_{1:x}'.format((val >> 16), val & 0xffff),
        }
        for n in reg['decode']:
            f = reg['decode'][n]
            o[n] = f(val)
        return o

    def getAll(self):
        res = {}
        prev_rname = None
        val = None
        rnames = list(self.regs_by_name.keys())
        rnames.sort()
        for rname in rnames:
            addr = self.regs_by_name[rname]['addr'];
            val = self._half_read_reg(addr)
            if prev_rname is not None:
                res[prev_rname] = self._make_result(prev_rname,val)
            prev_rname = rname
        val = self._half_read_reg(0)
        res[rnames[-1]] = self._make_result(rnames[-1],val)
        return res

    def get(self, name):
        name = name.lower()
        reg = self.regs_by_name.get(name,None)
        if reg is not None:
            addr = reg['addr']
            return self._make_result(name, self._read_reg(addr))
        return None

    def mask(self,mname,invert = False):
        m = self.masks.get(mname.lower(),None)
        if m is None:
            print('Warning: unknown bitmask: "' + mname + '"')
        elif invert:
            m = ~m
        return m


    def setBits(self, name, pattern):
        return self.set(name,pattern, 1)
       
    def clearBits(self, name, pattern):
        return self.set(name,~pattern, 2)

    def feed(self):
        self.set('on_remaining',self.get('on_rem_resetval')['__raw'])

    def reset(self):
        hw = self.get('hw_rev')
        if hw['device_id'][0] == 0x70 and hw['device_id'][1]  == 0x64 and hw['version_major'] == 2:
            self.set('on_remaining',self.get('on_rem_resetval')['__raw'])
            self.set('off_remaining',self.get('off_rem_resetval')['__raw'])
            self.set('firecounts',0)
            self.set('status',
                self.mask('wdog_en') | 
                self.mask('wake_en') | 
                self.mask('power_on')
            )
            return True
        print('PiDog2 initialization failed or device not present.')
        print(hw)
        return None

    # modes: 0 = read, 1 = logical_OR (set bits), 2 = logical_AND (for clearing bits), 0x3 = set
    def set(self, name, val, mode = 3):
        reg = None
        addr = None
        if type(name) == int:
            addr = name & 0xf
        else:
            name = name.lower()
            reg = self.regs_by_name.get(name,None)
            if reg is not None:
                addr = reg['addr']
        if addr is not None:
            v = self._write_reg(addr,val,mode)
            if reg is not None:
                return self._make_result(name, v)
            else:
                return v
        return None

    def deinit(self):
        self.spi.close()

        
def crazy_testing_stuff():
    pd = PiDog()

    myregs = [0 for x in range(5)]

    def setOne():
        victim = random.randrange(5,10)
        value = random.randint(0,256)
        value <<= 8
        value |= random.randint(0,256)
        value <<= 8
        value |= random.randint(0,256)
        value <<= 8
        value |= random.randint(0,256)
        myregs[victim-5] = value
        pd.set(victim,value)
       
    def compareArrays(a,b):
        for i in range(len(a)):
            av = a[i]
            bv = b[i]
            if av != bv:
                print('ERR r{0:x} {1:x} != {2:x}'.format(i,av,bv))

    if False:
        while True:
            for x in range(5):
                setOne()
            realregs = pd.fastGetList(list(range(5,10)))
            compareArrays(realregs,myregs)

 
    if False:
        for x in range(10):
            pd.set(x, x | x << 16)

        count = 0
        while count < 10:
            x = pd.getAll()
            print('\n'.join([ '{0}: {1:x}'.format(n,x[n]) for n in x]))
            count += 1

    if True:
        def jsd(x):
            print(json.dumps(x,indent=2,sort_keys=True))

        x = pd.getAll()
        jsd(x)

        jsd(pd.get('status'))
        time.sleep(2)

        jsd(pd.setBits('status',pd.mask('wdog_en')))
        time.sleep(2)

        jsd(pd.get('status'))
        time.sleep(2)

        jsd(pd.clearBits('status',pd.mask('wdog_en')))
        time.sleep(2)

        jsd(pd.get('status'))
        time.sleep(2)



class bbSPI:
    def __init__(self):
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(19,GPIO.OUT) # MOSI
        GPIO.setup(21,GPIO.IN)  # MISO
        GPIO.setup(23,GPIO.OUT) # CK
        GPIO.setup(24,GPIO.OUT) # SS

    def xfer2(self, oblist):
        iblist = []
        GPIO.output(24,GPIO.LOW)
        time.sleep(PDELAY)
        for obyte in oblist:
            ibyte = self._xfer8(obyte)
            if ibyte > 0xff:
                print('ERROR byte is not a byte!')
            iblist.append(ibyte)
        GPIO.output(24,GPIO.HIGH)
        time.sleep(5*PDELAY)
        # print('==> ' + ','.join([ '{0:x}'.format(x) for x in oblist]))
        # print('<== ' + ','.join([ '{0:x}'.format(x) for x in iblist]))
        return iblist

    def _xfer8(self,obyte):
        ibyte = 0
        for i in range(8):
            obit = obyte & 0x80
            GPIO.output(19, GPIO.HIGH if obit else GPIO.LOW)
            time.sleep(PDELAY)
            ibit = 1 if GPIO.input(21) else 0
            GPIO.output(23, GPIO.HIGH)
            GPIO.output(23, GPIO.LOW)
            time.sleep(PDELAY)
            ibyte |= ibit
            if i < 7:
                obyte <<= 1
                ibyte <<= 1
        time.sleep(4*PDELAY)
        return ibyte


    def close(self):
        print('closing and setting as inputs');
        GPIO.setup(19,GPIO.IN)
        GPIO.setup(21,GPIO.IN)
        GPIO.setup(23,GPIO.IN)
        GPIO.setup(24,GPIO.IN)


if __name__ == '__main__':
    crazy_testing_stuff()

