#!/usr/bin/env python3

import pidog

if __name__ == '__main__':

    import time

    pd = pidog.PiDog()
    pd.reset()

    while True:
        pd.feed()    
        time.sleep(10)

