"""dpi.py contains routines to help support high DPI displays."""

import ctypes
import platform

# default system dpi
default_system_dpi = 96

# current system dpi if known, or None if not known
system_dpi = None


def get_system_dpi():
    """Return the DPI currently in use."""
    global system_dpi
    dc = ctypes.windll.user32.GetDC(0)
    system_dpi = ctypes.windll.gdi32.GetDeviceCaps(dc, 88)
    ctypes.windll.user32.ReleaseDC(0, dc)


def set_dpi_aware():
    """Make the application DPI aware."""
    if platform.release() == "7":
        ctypes.windll.user32.SetProcessDPIAware()
    elif platform.release() == "8" or platform.release() == "10":
        ctypes.windll.shcore.SetProcessDpiAwareness(1)
    else:
        ctypes.windll.shcore.SetProcessDpiAwareness(1)
    get_system_dpi()


def asize(size):
    """Return the size adjusted for the current DPI setting."""
    if system_dpi is None:
        get_system_dpi()
    if isinstance(size, int) or isinstance(size, float):
        return round(size * system_dpi / default_system_dpi + 0.5)
    return [asize(x) for x in size]
