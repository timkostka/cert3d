import usb1
with usb1.USBContext() as context:
    handle = context.openByVendorIDAndProductID(
        0x0483,
        0x5740,
        skip_on_error=True,
    )
    if handle is None:
        print('Unable to access device!')
        # Device not present, or user is not allowed to access device.
    with handle.claimInterface(INTERFACE):
        pass
        # Do stuff with endpoints on claimed interface.
exit(0)


import usb.core
import usb.util

# find our device
dev = usb.core.find(idVendor=0x0483, idProduct=0x5740)

# was it found?
if dev is None:
    raise ValueError('Device not found')

# set the active configuration. With no arguments, the first
# configuration will be the active one
dev.set_configuration()

# get an endpoint instance
cfg = dev.get_active_configuration()
intf = cfg[(0,0)]

ep = usb.util.find_descriptor(
    intf,
    # match the first OUT endpoint
    custom_match = \
    lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_OUT)

assert ep is not None

# write the data
ep.write('test')
