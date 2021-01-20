#!/usr/bin/env python3

import pidog
import sys
import json

usage = """

    dogcmd.py set|get|reset <regname> <value>

    The first argument is set, get, or reset.

    Unless the first argument is reset, the second argument is 
    the name of the register you want to affect. One of:


    Regname is a register name, and must be one of:

        status
        on_remaining
        off_remaining
        on_rem_resetval
        off_rem_resetval
        temp_v33
        vsensa_vsensb
        v5_v5swtch
        firecounts
        hw_rev
        vsensa_on_threshold

    If the command was "set", then a value to write must also
    be specified.
     
"""

def getArgs():

    if len(sys.argv)<2:
        print('Err -- no args')
        return None

    cmd = sys.argv[1]

    if cmd == 'reset':
        return { 'cmd': 'reset' }
    elif cmd == 'get':
        if len(sys.argv)<3:
            print('Err -- no register named')
            return None
        rname = sys.argv[2]
        return { 'cmd': 'get', 'rname': rname }
    elif cmd == 'set':
        if len(sys.argv)<4:
            print('Err -- insufficient argument')
            return None
        rname = sys.argv[2]
        value = int(sys.argv[3],0)
        return {'cmd': 'set', 'rname': rname, 'value': value }

    print('Err -- unknown command')
    return None


def showRes(r):
    print(json.dumps(r,sort_keys=True,indent=4))


if __name__ == '__main__':
    args = getArgs()
    if not args:
        print(usage)
        sys.exit()

    pd = pidog.PiDog()

    cmd = args.get('cmd',None)

    rv = None

    if cmd == 'reset':
        rv = pd.reset()
    elif cmd == 'get':
        rv = pd.get(args.get('rname',None))
    elif cmd == 'set':
        rv = pd.set(args.get('rname',None), args.get('value',None))

    if rv:
        showRes(rv)
    else:
        print('Err -- command didn\'t run')


   
