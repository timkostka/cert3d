"""
Cert3D.py is a GUI for showing and interpreting results.

"""

import wx
import platform
import ctypes

from ScopeData import ScopeData
from ScopePanel import ScopePanel

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


# -*- coding: utf-8 -*-

###########################################################################
## Python code generated with wxFormBuilder (version Oct 26 2018)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc

###########################################################################
## Class ScopeResultsWindow
###########################################################################


class ScopeResultsWindow(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(
            self,
            parent,
            id=wx.ID_ANY,
            title=u"Oscilloscope output",
            pos=wx.DefaultPosition,
            size=adjusted_size(wx.Size(500, 387)),
            style=wx.DEFAULT_FRAME_STYLE | wx.TAB_TRAVERSAL,
        )

        self.SetSizeHints(wx.DefaultSize, wx.DefaultSize)
        self.SetBackgroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)
        )

        bSizer1 = wx.BoxSizer(wx.VERTICAL)

        bSizer5 = wx.BoxSizer(wx.HORIZONTAL)

        bSizer8 = wx.BoxSizer(wx.VERTICAL)

        self.m_staticText1 = wx.StaticText(
            self, wx.ID_ANY, u"X_STEP", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText1.Wrap(-1)

        bSizer8.Add(self.m_staticText1, 0, wx.ALL, 5)

        self.m_staticText2 = wx.StaticText(
            self, wx.ID_ANY, u"X_DIR", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText2.Wrap(-1)

        bSizer8.Add(self.m_staticText2, 0, wx.ALL, 5)

        self.m_staticText3 = wx.StaticText(
            self, wx.ID_ANY, u"Y_STEP", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText3.Wrap(-1)

        bSizer8.Add(self.m_staticText3, 0, wx.ALL, 5)

        self.m_staticText4 = wx.StaticText(
            self, wx.ID_ANY, u"Y_DIR", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText4.Wrap(-1)

        bSizer8.Add(self.m_staticText4, 0, wx.ALL, 5)

        self.m_staticText5 = wx.StaticText(
            self, wx.ID_ANY, u"Z_STEP", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText5.Wrap(-1)

        bSizer8.Add(self.m_staticText5, 0, wx.ALL, 5)

        self.m_staticText6 = wx.StaticText(
            self, wx.ID_ANY, u"Z_DIR", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText6.Wrap(-1)

        bSizer8.Add(self.m_staticText6, 0, wx.ALL, 5)

        self.m_staticText7 = wx.StaticText(
            self, wx.ID_ANY, u"E_STEP", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText7.Wrap(-1)

        bSizer8.Add(self.m_staticText7, 0, wx.ALL, 5)

        self.m_staticText8 = wx.StaticText(
            self, wx.ID_ANY, u"E_DIR", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.m_staticText8.Wrap(-1)

        bSizer8.Add(self.m_staticText8, 0, wx.ALL, 5)

        self.m_staticText9 = wx.StaticText(
            self,
            wx.ID_ANY,
            wx.EmptyString,
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.m_staticText9.Wrap(-1)

        bSizer8.Add(self.m_staticText9, 0, wx.ALL, 5)

        bSizer5.Add(bSizer8, 0, wx.EXPAND, 5)

        self.m_panel1 = ScopePanel(self, wx.ID_ANY, wx.DefaultPosition,
                                 wx.DefaultSize, wx.TAB_TRAVERSAL)

        #self.m_panel1 = wx.Panel(self, wx.ID_ANY, wx.DefaultPosition,
        #                         wx.DefaultSize, wx.TAB_TRAVERSAL)
        #self.m_panel1.SetBackgroundColour(wx.BLACK)

        bSizer5.Add(self.m_panel1, 1, wx.EXPAND | wx.ALL, 5)

        bSizer1.Add(bSizer5, 1, wx.EXPAND, 5)

        bSizer2 = wx.BoxSizer(wx.HORIZONTAL)

        bSizer2.Add((0, 0), 1, wx.EXPAND, 5)

        self.m_button1 = wx.Button(
            self, wx.ID_ANY, u"Done", wx.DefaultPosition, wx.DefaultSize, 0
        )
        bSizer2.Add(self.m_button1, 0, wx.ALIGN_RIGHT | wx.ALL, 5)

        bSizer1.Add(bSizer2, 0, wx.EXPAND, 0)

        self.SetSizer(bSizer1)
        self.Layout()
        self.m_statusBar1 = self.CreateStatusBar(1, wx.STB_SIZEGRIP, wx.ID_ANY)

        self.Centre(wx.BOTH)

        # Connect Events
        #self.m_panel1.Bind(wx.EVT_PAINT, self.m_panel1.event_paint)

    def __del__(self):
        pass

    # Virtual event handlers, overide them in your derived class
    def event_paint_scope(self, event):
        print('Painting2')
        dc = wx.PaintDC(self.m_panel1)
        gc = wx.GCDC(dc)
        #cdc = wx.ClientDC(self.m_panel1)
        #self.m_panel1.PrepareDC(cdc)
        #dc = wx.BufferedDC(cdc, self.m_panel1.buffer)
        gc.SetPen(wx.Pen(wx.GREEN, 3))
        #event.Skip()


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
