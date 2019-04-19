"""
This file examines threading capabilities.

"""
import time
from threading import Thread


exit_slave = False


def slave_thread_entry():
    print('Slave thread started!')
    #raise ValueError
    while not exit_slave:
        time.sleep(0.001)
    print('Exiting slave thread!')


# create slave thread
child_thread = Thread(target=slave_thread_entry)
child_thread.start()
time.sleep(0.500)
print('child_thread.is_alive() =', child_thread.is_alive())
exit_slave = True
child_thread.join()
