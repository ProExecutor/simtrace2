#!/usr/bin/env python3 

import usb.core
import usb.util
import sys
import array

from constants import PHONE_RD

def find_dev():
    dev = usb.core.find(idVendor=0x03eb, idProduct=0x6004)
    if dev is None:
        raise ValueError("Device not found")
    else:
        print("Found device")
    return dev

def find_eps(dev):
    dev.set_configuration()

    cfg = dev.get_active_configuration()
    print("Active config: ")
    print(cfg)
    intf = cfg[(0,0)]

    ep_in = usb.util.find_descriptor(
        intf, 
        custom_match = \
        lambda e: \
            usb.util.endpoint_direction(e.bEndpointAddress) == \
            usb.util.ENDPOINT_IN)

    assert ep_in is not None

    ep_out = usb.util.find_descriptor(
        intf, 
        custom_match = \
        lambda e: \
            usb.util.endpoint_direction(e.bEndpointAddress) == \
            usb.util.ENDPOINT_OUT)

    assert ep_out is not None
    print("****")
    print(ep_in)
    print(ep_out)
    return (ep_in, ep_out)

# main code
def sniff():
    dev = find_dev()
    ans = array.array('B', [])

    while True:
        #ep_out.write("Hello")
        try:
            ans += dev.read(PHONE_RD, 64, 1000)
        except KeyboardInterrupt:
            print("Bye")
            sys.exit()
        except Exception as e:
            print e

        if len(ans) >= 1:
            print("".join("%02x " % b for b in ans))
            ans = array.array('B', [])
