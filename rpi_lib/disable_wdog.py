#!/usr/bin/env python3

import pidog
import json

if __name__ == '__main__':

    import time

    pd = pidog.PiDog()
    ok = pd.reset()

    if ok:

        pd.set('status', ~pd.mask('wdog_en'), 2)
        rv = pd.get('status')
        print(json.dumps(rv,indent=2,sort_keys=True))

    else:
        print('Uh-oh. Missing PiDog?')


