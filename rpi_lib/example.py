#!/usr/bin/env python3

import pidog
import json

if __name__ == '__main__':

    import time

    pd = pidog.PiDog()
    ok = pd.reset()

    if ok:
        # modes: 0 = read, 1 = logical_OR (set bits), 2 = logical_AND (for clearing bits), 0x3 = set
        pd.set(name='vsensa_on_threshold', val=13000, mode=3)
        pd.set(name='on_rem_resetval', val=60, mode=3)
        pd.set(name='off_rem_resetval', val=30, mode=3)
        pd.set(name='on_remaining', val=60, mode=3)
        pd.set(name='off_remaining', val=30, mode=3)
    
        while True:
            rv = pd.get('status')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('v5_v5swtch')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('on_remaining')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('off_remaining')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('on_rem_resetval')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('off_rem_resetval')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('temp_v33')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('vsensa_vsensb')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('firecounts')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('hw_rev')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('vsensa_on_threshold')
            print(json.dumps(rv,indent=2,sort_keys=True))            
            time.sleep(5)
            #pd.feed()    

    else:
        print('Uh-oh. Missing PiDog?')


