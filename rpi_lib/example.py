#!/usr/bin/env python3

import pidog
import json

if __name__ == '__main__':

    import time

    pd = pidog.PiDog()
    ok = pd.reset()

    if ok:
        while True:
            pd.feed()    
            rv = pd.get('vsensa_vsensb')
            print(json.dumps(rv,indent=2,sort_keys=True))
            time.sleep(10)
    else:
        print('Uh-oh. Missing PiDog?')


