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
            rv = pd.get('status')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('v5_v5swtch')
            print(json.dumps(rv,indent=2,sort_keys=True))
            time.sleep(2)
    else:
        print('Uh-oh. Missing PiDog?')


