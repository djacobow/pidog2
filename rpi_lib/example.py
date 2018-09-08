#!/usr/bin/env python3

import pidog

if __name__ == '__main__':

    import time

    pd = pidog.PiDog()
    ok = pd.reset()

    if ok:
        while True:
            pd.feed()    
            time.sleep(10)
    else:
        print('Uh-oh. Missing PiDog?')


