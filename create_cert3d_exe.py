"""
This file creates a pyinstaller EXE file.

"""

import os

####################
# START OF OPTIONS #
####################

##################
# END OF OPTIONS #
##################

# path of the EXE to create
exe_path = os.path.abspath("./dist/cert3d_gui.exe")

# delete existing EXE
if os.path.isfile(exe_path):
    os.remove(exe_path)

# create the EXE
arguments = [
    "pyinstaller",
    "-n cert3d_gui",
    "--clean",
    "--noconsole",
    "--onefile",
    "cert3d.py",
]
os.system(" ".join(arguments))

# if file wasn't created, something went wrong
if not os.path.isfile(exe_path):
    print("\nERROR: could not create EXE file")
    exit(1)

# report success
print("\nEXE created at %s" % exe_path)
