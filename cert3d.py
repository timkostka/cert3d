"""
Cert3D.py is a GUI for showing and interpreting results.

"""

import wx
import platform
import ctypes

# from ScopeData import ScopeData
# from ScopePanel import ScopePanel
from ScopeResultsWindow import ScopeResultsWindow

system_dpi = 96


def get_system_dpi():
    """Return the DPI currently in use."""
    global system_dpi
    dc = ctypes.windll.user32.GetDC(0)
    system_dpi = ctypes.windll.gdi32.GetDeviceCaps(dc, 88)
    ctypes.windll.user32.ReleaseDC(0, dc)


def set_dpi_aware():
    """Make the application DPI aware."""
    print("Registering DPI awareness.")
    if platform.release() == "7":
        ctypes.windll.user32.SetProcessDPIAware()
    elif platform.release() == "8" or platform.release() == "10":
        ctypes.windll.shcore.SetProcessDpiAwareness(1)
    else:
        ctypes.windll.shcore.SetProcessDpiAwareness(1)


def adjusted_size(size):
    """Return a size adjusted for the current DPI setting."""
    if isinstance(size, int):
        return int(size * system_dpi / 96.0 + 0.5)
    if isinstance(size, wx.Size):
        return wx.Size(adjusted_size(size[0]), adjusted_size(size[1]))
    raise ValueError




def run_gui():
    """Run the GUI application."""
    set_dpi_aware()
    get_system_dpi()
    # create the window
    app = wx.App()
    ex = ScopeResultsWindow(None)
    ex.Show()
    app.MainLoop()


if __name__ == "__main__":
    run_gui()
