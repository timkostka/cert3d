# -*- coding: utf-8 -*-

###########################################################################
## Python code generated with wxFormBuilder (version Oct 26 2018)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc
from ScopePanel import ScopePanel

###########################################################################
## Class AnalysisWindowBase
###########################################################################


class AnalysisWindowBase(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(
            self,
            parent,
            id=wx.ID_ANY,
            title=u"Oscilloscope output",
            pos=wx.DefaultPosition,
            size=wx.Size(800, 600),
            style=wx.DEFAULT_FRAME_STYLE | wx.TAB_TRAVERSAL,
        )

        self.SetSizeHints(wx.DefaultSize, wx.DefaultSize)
        self.SetBackgroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)
        )

        bSizer1 = wx.BoxSizer(wx.VERTICAL)

        bSizer5 = wx.BoxSizer(wx.HORIZONTAL)

        bSizer51 = wx.BoxSizer(wx.VERTICAL)

        self.scope_panel = ScopePanel(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TAB_TRAVERSAL,
        )

        bSizer51.Add(self.scope_panel, 1, wx.ALL | wx.EXPAND, 5)

        self.scope_scroll_bar = wx.ScrollBar(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.SB_HORIZONTAL,
        )
        bSizer51.Add(self.scope_scroll_bar, 0, wx.ALL | wx.EXPAND, 0)

        bSizer5.Add(bSizer51, 1, wx.EXPAND, 5)

        bSizer1.Add(bSizer5, 1, wx.EXPAND, 5)

        bSizer2 = wx.BoxSizer(wx.HORIZONTAL)

        bSizer2.Add((0, 0), 1, wx.EXPAND, 5)

        self.button_done = wx.Button(
            self, wx.ID_ANY, u"Done", wx.DefaultPosition, wx.DefaultSize, 0
        )
        self.button_done.Hide()

        bSizer2.Add(self.button_done, 0, wx.ALIGN_RIGHT | wx.ALL, 5)

        bSizer1.Add(bSizer2, 0, wx.EXPAND, 0)

        self.SetSizer(bSizer1)
        self.Layout()
        self.status_bar = self.CreateStatusBar(1, wx.STB_SIZEGRIP, wx.ID_ANY)
        self.menu_bar = wx.MenuBar(0)
        self.menu_file = wx.Menu()
        self.menu_file_exit = wx.MenuItem(
            self.menu_file, wx.ID_ANY, u"E&xit", wx.EmptyString, wx.ITEM_NORMAL
        )
        self.menu_file.Append(self.menu_file_exit)

        self.menu_bar.Append(self.menu_file, u"&File")

        self.SetMenuBar(self.menu_bar)

        self.Centre(wx.BOTH)

        # Connect Events
        self.Bind(
            wx.EVT_MENU, self.event_file_exit, id=self.menu_file_exit.GetId()
        )

    def __del__(self):
        pass

    # Virtual event handlers, overide them in your derived class
    def event_file_exit(self, event):
        event.Skip()
