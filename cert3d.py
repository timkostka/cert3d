"""
Cert3D.py is a GUI for showing and interpreting results.

"""

import wx
import platform
import ctypes

import dpi

# from BilevelData import BilevelData
# from ScopePanel import ScopePanel
# from BilevelData import BilevelData
from AnalysisWindow import AnalysisWindow


def run_gui():
    """Run the GUI application."""
    dpi.set_dpi_aware()
    # create the window
    app = wx.App()
    ex = AnalysisWindow(None)
    ex.Show()
    app.MainLoop()


if __name__ == "__main__":
    run_gui()
