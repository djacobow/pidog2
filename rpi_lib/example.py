#!/usr/bin/env python3

import pidog
import json

if __name__ == '__main__':

    import time

    pd = pidog.PiDog()
    ok = pd.reset()

    if ok:
        pd.set(name='vsense_on_threshold', val=(pd.getAdcValue('vsensa', 13000) << 16 | 0x0), mode=1)
        ##pd.set(name='vsense_on_threshold', val=(pd.getAdcValue('vsensb', 13000) & 0xffff), mode=1)
        pd.set(name='vsense_off_threshold', val=(pd.getAdcValue('vsensa', 10000) << 16 | 0x0), mode=1)
        ##pd.set(name='vsense_off_threshold', val=(pd.getAdcValue('vsensb', 10000) & 0xffff), mode=1)
        pd.set(name='on_rem_resetval', val=60, mode=3)
        pd.set(name='off_rem_resetval', val=60, mode=3)
        pd.set(name='on_remaining', val=60, mode=3)
        pd.set(name='off_remaining', val=60, mode=3)
    
        rv = pd.get('hw_rev')
        print(json.dumps(rv,indent=2,sort_keys=True))
        skip_feed = 0
        while True:
            rv = pd.get('status')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('v5_v5swtch')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('on_remaining')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('off_remaining')
            print(json.dumps(rv,indent=2,sort_keys=True))
            """
            rv = pd.get('on_rem_resetval')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('off_rem_resetval')
            print(json.dumps(rv,indent=2,sort_keys=True))
            """
            rv = pd.get('temp_v33')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('vsensa_vsensb')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('firecounts')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('vsense_on_threshold')
            print(json.dumps(rv,indent=2,sort_keys=True))
            rv = pd.get('vsense_off_threshold')
            print(json.dumps(rv,indent=2,sort_keys=True)) 

            if skip_feed == 8:
                pd.feed()
                skip_feed = 0
            else:
                skip_feed += 1
            
            time.sleep(5)

    else:
        print('Uh-oh. Missing PiDog?')


