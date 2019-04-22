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

        self.m_scrollBar2 = wx.ScrollBar(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.SB_HORIZONTAL,
        )
        bSizer51.Add(self.m_scrollBar2, 0, wx.ALL | wx.EXPAND, 5)

        bSizer5.Add(bSizer51, 1, wx.EXPAND, 5)

        bSizer52 = wx.BoxSizer(wx.VERTICAL)

        self.static_text_usb_port_status = wx.StaticText(
            self,
            wx.ID_ANY,
            u"Disconnected",
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.ALIGN_CENTER_HORIZONTAL | wx.ST_NO_AUTORESIZE,
        )
        self.static_text_usb_port_status.Wrap(-1)

        bSizer52.Add(
            self.static_text_usb_port_status, 0, wx.ALL | wx.EXPAND, 0
        )

        self.static_text_data_rate = wx.StaticText(
            self,
            wx.ID_ANY,
            u"n/a",
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.ALIGN_CENTER_HORIZONTAL | wx.ST_NO_AUTORESIZE,
        )
        self.static_text_data_rate.Wrap(-1)

        bSizer52.Add(self.static_text_data_rate, 0, wx.ALL | wx.EXPAND, 0)

        self.button_open_test_window = wx.Button(
            self,
            wx.ID_ANY,
            u"Test Window",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer52.Add(self.button_open_test_window, 0, wx.ALL, 5)

        self.button_start_stream = wx.Button(
            self,
            wx.ID_ANY,
            u"Start Stream",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer52.Add(self.button_start_stream, 0, wx.ALL, 5)

        self.button_stop_stream = wx.Button(
            self,
            wx.ID_ANY,
            u"Stop Stream",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer52.Add(self.button_stop_stream, 0, wx.ALL, 5)

        self.button_zoom_all = wx.Button(
            self, wx.ID_ANY, u"Zoom All", wx.DefaultPosition, wx.DefaultSize, 0
        )
        bSizer52.Add(self.button_zoom_all, 0, wx.ALL, 5)

        self.button_interpret = wx.Button(
            self,
            wx.ID_ANY,
            u"Interpret",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer52.Add(self.button_interpret, 0, wx.ALL, 5)

        self.button_debug = wx.Button(
            self, wx.ID_ANY, u"Debug", wx.DefaultPosition, wx.DefaultSize, 0
        )
        bSizer52.Add(self.button_debug, 0, wx.ALL, 5)

        self.button_debug_2 = wx.Button(
            self,
            wx.ID_ANY,
            u"Interpret Old",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer52.Add(self.button_debug_2, 0, wx.ALL, 5)

        self.button_trim = wx.Button(
            self, wx.ID_ANY, u"Trim", wx.DefaultPosition, wx.DefaultSize, 0
        )
        bSizer52.Add(self.button_trim, 0, wx.ALL, 5)

        self.button_reset = wx.Button(
            self,
            wx.ID_ANY,
            u"Reset C3D",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer52.Add(self.button_reset, 0, wx.ALL, 5)

        self.button_clear_log = wx.Button(
            self,
            wx.ID_ANY,
            u"Clear Data",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer52.Add(self.button_clear_log, 0, wx.ALL, 5)

        self.button_exit = wx.Button(
            self, wx.ID_ANY, u"Exit", wx.DefaultPosition, wx.DefaultSize, 0
        )
        bSizer52.Add(self.button_exit, 0, wx.ALL, 5)

        bSizer5.Add(bSizer52, 0, wx.EXPAND, 5)

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
        self.menu_file_open = wx.MenuItem(
            self.menu_file,
            wx.ID_OPEN,
            u"&Open" + u"\t" + u"Ctrl+O",
            wx.EmptyString,
            wx.ITEM_NORMAL,
        )
        self.menu_file_open.SetBitmap(
            wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN)
        )
        self.menu_file.Append(self.menu_file_open)

        self.menu_file.AppendSeparator()

        self.menu_file_exit = wx.MenuItem(
            self.menu_file,
            wx.ID_EXIT,
            u"E&xit" + u"\t" + u"Alt+F4",
            wx.EmptyString,
            wx.ITEM_NORMAL,
        )
        self.menu_file.Append(self.menu_file_exit)

        self.menu_bar.Append(self.menu_file, u"&File")

        self.SetMenuBar(self.menu_bar)

        self.timer_update_ui = wx.Timer()
        self.timer_update_ui.SetOwner(self, wx.ID_ANY)
        self.timer_update_ui.Start(100)

        self.Centre(wx.BOTH)

        # Connect Events
        self.Bind(wx.EVT_CLOSE, self.event_close)
        self.scope_panel.Bind(wx.EVT_MOUSEWHEEL, self.event_mouse_wheel)
        self.button_open_test_window.Bind(
            wx.EVT_BUTTON, self.event_button_open_test_window_click
        )
        self.button_start_stream.Bind(
            wx.EVT_BUTTON, self.event_button_start_stream_click
        )
        self.button_stop_stream.Bind(
            wx.EVT_BUTTON, self.event_button_stop_stream_click
        )
        self.button_zoom_all.Bind(
            wx.EVT_BUTTON, self.event_button_zoom_all_click
        )
        self.button_interpret.Bind(
            wx.EVT_BUTTON, self.event_button_interpret_click
        )
        self.button_debug.Bind(wx.EVT_BUTTON, self.event_button_debug_click)
        self.button_debug_2.Bind(
            wx.EVT_BUTTON, self.event_button_debug_2_click
        )
        self.button_trim.Bind(wx.EVT_BUTTON, self.event_button_trim_click)
        self.button_reset.Bind(wx.EVT_BUTTON, self.event_button_reset_click)
        self.button_clear_log.Bind(
            wx.EVT_BUTTON, self.event_button_clear_log_click
        )
        self.button_exit.Bind(wx.EVT_BUTTON, self.event_menu_file_exit)
        self.Bind(
            wx.EVT_MENU,
            self.event_menu_file_open,
            id=self.menu_file_open.GetId(),
        )
        self.Bind(
            wx.EVT_MENU,
            self.event_menu_file_exit,
            id=self.menu_file_exit.GetId(),
        )
        self.Bind(wx.EVT_TIMER, self.event_timer_update_ui, id=wx.ID_ANY)

    def __del__(self):
        pass

    # Virtual event handlers, overide them in your derived class
    def event_close(self, event):
        event.Skip()

    def event_mouse_wheel(self, event):
        event.Skip()

    def event_button_open_test_window_click(self, event):
        event.Skip()

    def event_button_start_stream_click(self, event):
        event.Skip()

    def event_button_stop_stream_click(self, event):
        event.Skip()

    def event_button_zoom_all_click(self, event):
        event.Skip()

    def event_button_interpret_click(self, event):
        event.Skip()

    def event_button_debug_click(self, event):
        event.Skip()

    def event_button_debug_2_click(self, event):
        event.Skip()

    def event_button_trim_click(self, event):
        event.Skip()

    def event_button_reset_click(self, event):
        event.Skip()

    def event_button_clear_log_click(self, event):
        event.Skip()

    def event_menu_file_exit(self, event):
        event.Skip()

    def event_menu_file_open(self, event):
        event.Skip()

    def event_timer_update_ui(self, event):
        event.Skip()


###########################################################################
## Class TestWindowBase
###########################################################################


class TestWindowBase(wx.Dialog):
    def __init__(self, parent):
        wx.Dialog.__init__(
            self,
            parent,
            id=wx.ID_ANY,
            title=wx.EmptyString,
            pos=wx.DefaultPosition,
            size=wx.DefaultSize,
            style=wx.DEFAULT_DIALOG_STYLE,
        )

        self.SetSizeHints(wx.DefaultSize, wx.DefaultSize)

        bSizer6 = wx.BoxSizer(wx.VERTICAL)

        bSizer7 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_panel3 = wx.Panel(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TAB_TRAVERSAL,
        )
        bSizer9 = wx.BoxSizer(wx.VERTICAL)

        sbSizer1 = wx.StaticBoxSizer(
            wx.StaticBox(self.m_panel3, wx.ID_ANY, u"Cert3D board"),
            wx.VERTICAL,
        )

        self.static_text_c3d_board_connection = wx.StaticText(
            sbSizer1.GetStaticBox(),
            wx.ID_ANY,
            u"Connected on COM3",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.static_text_c3d_board_connection.Wrap(-1)

        sbSizer1.Add(self.static_text_c3d_board_connection, 0, wx.ALL, 0)

        self.static_text_c3d_board_data_rate = wx.StaticText(
            sbSizer1.GetStaticBox(),
            wx.ID_ANY,
            u"Data rate: 1.234 Mbps",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.static_text_c3d_board_data_rate.Wrap(-1)

        sbSizer1.Add(self.static_text_c3d_board_data_rate, 0, wx.ALL, 0)

        self.m_staticText8 = wx.StaticText(
            sbSizer1.GetStaticBox(),
            wx.ID_ANY,
            u"Data size: 200 MB",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.m_staticText8.Wrap(-1)

        sbSizer1.Add(self.m_staticText8, 0, wx.ALL, 0)

        bSizer9.Add(sbSizer1, 1, wx.EXPAND, 5)

        sbSizer5 = wx.StaticBoxSizer(
            wx.StaticBox(self.m_panel3, wx.ID_ANY, u"Printer board"),
            wx.VERTICAL,
        )

        self.static_text_printer_board_connection = wx.StaticText(
            sbSizer5.GetStaticBox(),
            wx.ID_ANY,
            u"Connected on COM23",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.static_text_printer_board_connection.Wrap(-1)

        sbSizer5.Add(self.static_text_printer_board_connection, 0, wx.ALL, 0)

        bSizer9.Add(sbSizer5, 1, wx.EXPAND, 5)

        sbSizer6 = wx.StaticBoxSizer(
            wx.StaticBox(self.m_panel3, wx.ID_ANY, u"Test progress"),
            wx.VERTICAL,
        )

        self.gauge_test_progress = wx.Gauge(
            sbSizer6.GetStaticBox(),
            wx.ID_ANY,
            100,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.GA_HORIZONTAL,
        )
        self.gauge_test_progress.SetValue(50)
        sbSizer6.Add(self.gauge_test_progress, 0, wx.ALL | wx.EXPAND, 5)

        bSizer9.Add(sbSizer6, 0, wx.EXPAND, 5)

        self.m_panel3.SetSizer(bSizer9)
        self.m_panel3.Layout()
        bSizer9.Fit(self.m_panel3)
        bSizer7.Add(self.m_panel3, 1, wx.EXPAND | wx.ALL, 5)

        self.m_panel4 = wx.Panel(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TAB_TRAVERSAL,
        )
        sbSizer2 = wx.StaticBoxSizer(
            wx.StaticBox(self.m_panel4, wx.ID_ANY, u"Test G-code"), wx.VERTICAL
        )

        self.m_textCtrl1 = wx.TextCtrl(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"G21\nG90\nG92 E0 X0 Y0 Z0\nG0 X1\nG0 X0\nG0 X3\nG0 X0\nG0 X1\nG0 X0\nM400\n",
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TE_DONTWRAP | wx.TE_MULTILINE,
        )
        sbSizer2.Add(self.m_textCtrl1, 1, wx.ALL | wx.EXPAND, 5)

        self.static_text_gcode_reference = wx.StaticText(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"Reference manual",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.static_text_gcode_reference.Wrap(-1)

        self.static_text_gcode_reference.SetFont(
            wx.Font(
                wx.NORMAL_FONT.GetPointSize(),
                wx.FONTFAMILY_DEFAULT,
                wx.FONTSTYLE_NORMAL,
                wx.FONTWEIGHT_NORMAL,
                True,
                wx.EmptyString,
            )
        )
        self.static_text_gcode_reference.SetForegroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHT)
        )

        sbSizer2.Add(
            self.static_text_gcode_reference, 0, wx.ALL | wx.EXPAND, 5
        )

        self.m_panel4.SetSizer(sbSizer2)
        self.m_panel4.Layout()
        sbSizer2.Fit(self.m_panel4)
        bSizer7.Add(self.m_panel4, 1, wx.EXPAND | wx.ALL, 5)

        bSizer6.Add(bSizer7, 1, wx.EXPAND, 5)

        self.m_panel5 = wx.Panel(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TAB_TRAVERSAL,
        )
        bSizer8 = wx.BoxSizer(wx.HORIZONTAL)

        self.button_run_test = wx.Button(
            self.m_panel5,
            wx.ID_ANY,
            u"Run Test",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.button_run_test.Enable(False)

        bSizer8.Add(self.button_run_test, 0, wx.ALL, 5)

        self.m_button14 = wx.Button(
            self.m_panel5,
            wx.ID_ANY,
            u"MyButton",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer8.Add(self.m_button14, 0, wx.ALL, 5)

        bSizer8.Add((0, 0), 1, wx.EXPAND, 5)

        self.button_back = wx.Button(
            self.m_panel5,
            wx.ID_ANY,
            u"Done",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer8.Add(self.button_back, 0, wx.ALL, 5)

        self.m_panel5.SetSizer(bSizer8)
        self.m_panel5.Layout()
        bSizer8.Fit(self.m_panel5)
        bSizer6.Add(self.m_panel5, 0, wx.EXPAND | wx.ALL, 5)

        self.SetSizer(bSizer6)
        self.Layout()
        bSizer6.Fit(self)

        self.Centre(wx.BOTH)

        # Connect Events
        self.static_text_gcode_reference.Bind(
            wx.EVT_LEFT_DOWN, self.event_static_text_gcode_reference_click
        )

    def __del__(self):
        pass

    # Virtual event handlers, overide them in your derived class
    def event_static_text_gcode_reference_click(self, event):
        event.Skip()
