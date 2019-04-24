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
import wx.richtext

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

        fgSizer1 = wx.FlexGridSizer(0, 2, 0, 0)
        fgSizer1.AddGrowableCol(0)
        fgSizer1.AddGrowableRow(0)
        fgSizer1.SetFlexibleDirection(wx.BOTH)
        fgSizer1.SetNonFlexibleGrowMode(wx.FLEX_GROWMODE_SPECIFIED)

        self.scope_panel = ScopePanel(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TAB_TRAVERSAL,
        )

        fgSizer1.Add(self.scope_panel, 1, wx.ALL | wx.EXPAND, 0)

        self.scroll_bar_vertical = wx.ScrollBar(
            self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.SB_VERTICAL
        )
        fgSizer1.Add(self.scroll_bar_vertical, 0, wx.ALL | wx.EXPAND, 0)

        self.scroll_bar_horizontal = wx.ScrollBar(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.SB_HORIZONTAL,
        )
        self.scroll_bar_horizontal.Hide()

        fgSizer1.Add(self.scroll_bar_horizontal, 0, wx.ALL | wx.EXPAND, 0)

        bSizer5.Add(fgSizer1, 1, wx.ALL | wx.EXPAND, 5)

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
        self.m_timer2 = wx.Timer()
        self.m_timer2.SetOwner(self, wx.ID_ANY)
        self.status_bar = self.CreateStatusBar(3, wx.STB_SIZEGRIP, wx.ID_ANY)
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
        self.scroll_bar_vertical.Bind(
            wx.EVT_SCROLL, self.event_on_vertical_scroll
        )
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

    def event_on_vertical_scroll(self, event):
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


class TestWindowBase(wx.Frame):
    def __init__(self, parent):
        wx.Frame.__init__(
            self,
            parent,
            id=wx.ID_ANY,
            title=u"Test Runner",
            pos=wx.DefaultPosition,
            size=wx.Size(704, 381),
            style=wx.DEFAULT_FRAME_STYLE | wx.TAB_TRAVERSAL,
        )

        self.SetSizeHints(wx.DefaultSize, wx.DefaultSize)
        self.SetForegroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOW)
        )
        self.SetBackgroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)
        )

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

        self.checkbox_view_when_complete = wx.CheckBox(
            sbSizer6.GetStaticBox(),
            wx.ID_ANY,
            u"Automatically view when done",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.checkbox_view_when_complete.SetValue(True)
        sbSizer6.Add(self.checkbox_view_when_complete, 0, wx.ALL, 5)

        self.gauge_test_progress = wx.Gauge(
            sbSizer6.GetStaticBox(),
            wx.ID_ANY,
            100,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.GA_HORIZONTAL,
        )
        self.gauge_test_progress.SetValue(0)
        sbSizer6.Add(self.gauge_test_progress, 0, wx.ALL | wx.EXPAND, 5)

        bSizer9.Add(sbSizer6, 0, wx.EXPAND, 5)

        self.m_panel5 = wx.Panel(
            self.m_panel3,
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
        bSizer8.Add(self.button_run_test, 0, wx.ALL, 5)

        bSizer8.Add((0, 0), 1, wx.EXPAND, 5)

        self.button_view_results = wx.Button(
            self.m_panel5,
            wx.ID_ANY,
            u"View",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        bSizer8.Add(self.button_view_results, 0, wx.ALL, 5)

        self.m_panel5.SetSizer(bSizer8)
        self.m_panel5.Layout()
        bSizer8.Fit(self.m_panel5)
        bSizer9.Add(self.m_panel5, 0, wx.EXPAND | wx.ALL, 0)

        self.m_panel3.SetSizer(bSizer9)
        self.m_panel3.Layout()
        bSizer9.Fit(self.m_panel3)
        bSizer7.Add(self.m_panel3, 0, wx.EXPAND | wx.ALL, 5)

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

        bSizer13 = wx.BoxSizer(wx.HORIZONTAL)

        self.button_create_update = wx.Button(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"Create/Update",
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.BU_EXACTFIT,
        )
        bSizer13.Add(self.button_create_update, 0, 0, 5)

        self.button_delete = wx.Button(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"Delete",
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.BU_EXACTFIT,
        )
        bSizer13.Add(self.button_delete, 0, wx.LEFT, 5)

        bSizer13.Add((0, 0), 1, wx.EXPAND, 5)

        sbSizer2.Add(bSizer13, 0, wx.EXPAND, 0)

        combo_box_test_choiceChoices = []
        self.combo_box_test_choice = wx.ComboBox(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            wx.EmptyString,
            wx.DefaultPosition,
            wx.DefaultSize,
            combo_box_test_choiceChoices,
            0,
        )
        sbSizer2.Add(self.combo_box_test_choice, 0, wx.EXPAND | wx.TOP, 5)

        self.text_ctrl_test_code = wx.TextCtrl(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"G21 ; set units to mm\nG90 ; set mode to absolute\nM92 X80 Y80 Z4000 E500 ; set steps/mm\nM203 X160 Y160 Z50 E200 ; set max feed rate (mm/s)\nM204 P1000 R1000 T1000 ; set acceleration\nM205 X0 Y0 Z0 T0 ; set jerk\nG92 E0 X0 Y0 Z0 ; set position to origin\nG0 X1\nG0 X0\nG0 X3\nG0 X0\nG0 X1\nG0 X0\nG0 X3\nG0 X0\nG0 X1\nG0 X0\nM400\n\nG0 X1\nG0 X0\nM400\n",
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TE_DONTWRAP | wx.TE_MULTILINE,
        )
        self.text_ctrl_test_code.SetFont(
            wx.Font(
                9,
                wx.FONTFAMILY_MODERN,
                wx.FONTSTYLE_NORMAL,
                wx.FONTWEIGHT_NORMAL,
                False,
                "Consolas",
            )
        )

        sbSizer2.Add(
            self.text_ctrl_test_code, 1, wx.BOTTOM | wx.EXPAND | wx.TOP, 5
        )

        bSizer11 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_staticText9 = wx.StaticText(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"Marlin: ",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.m_staticText9.Wrap(-1)

        bSizer11.Add(self.m_staticText9, 0, wx.ALL, 0)

        self.static_text_gcode_reference = wx.StaticText(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"http://marlinfw.org/meta/gcode/",
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

        bSizer11.Add(
            self.static_text_gcode_reference, 0, wx.ALL | wx.EXPAND, 0
        )

        sbSizer2.Add(bSizer11, 0, wx.EXPAND, 0)

        bSizer12 = wx.BoxSizer(wx.HORIZONTAL)

        self.m_staticText10 = wx.StaticText(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"Reprap: ",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.m_staticText10.Wrap(-1)

        bSizer12.Add(self.m_staticText10, 0, wx.ALL, 0)

        self.static_text_reprap_link = wx.StaticText(
            sbSizer2.GetStaticBox(),
            wx.ID_ANY,
            u"https://reprap.org/wiki/G-code",
            wx.DefaultPosition,
            wx.DefaultSize,
            0,
        )
        self.static_text_reprap_link.Wrap(-1)

        self.static_text_reprap_link.SetFont(
            wx.Font(
                9,
                wx.FONTFAMILY_SWISS,
                wx.FONTSTYLE_NORMAL,
                wx.FONTWEIGHT_NORMAL,
                True,
                "Arial",
            )
        )
        self.static_text_reprap_link.SetForegroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHT)
        )

        bSizer12.Add(self.static_text_reprap_link, 0, wx.ALL, 0)

        sbSizer2.Add(bSizer12, 0, wx.EXPAND, 0)

        self.m_panel4.SetSizer(sbSizer2)
        self.m_panel4.Layout()
        sbSizer2.Fit(self.m_panel4)
        bSizer7.Add(self.m_panel4, 1, wx.EXPAND | wx.ALL, 5)

        self.m_panel41 = wx.Panel(
            self,
            wx.ID_ANY,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TAB_TRAVERSAL,
        )
        sbSizer8 = wx.StaticBoxSizer(
            wx.StaticBox(self.m_panel41, wx.ID_ANY, u"Serial port monitor"),
            wx.VERTICAL,
        )

        self.rich_text_serial_log = wx.TextCtrl(
            sbSizer8.GetStaticBox(),
            wx.ID_ANY,
            wx.EmptyString,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TE_DONTWRAP | wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_RICH2,
        )
        sbSizer8.Add(self.rich_text_serial_log, 1, wx.ALL | wx.EXPAND, 5)

        self.rich_text_serial_log2 = wx.richtext.RichTextCtrl(
            sbSizer8.GetStaticBox(),
            wx.ID_ANY,
            wx.EmptyString,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TE_READONLY
            | wx.VSCROLL
            | wx.HSCROLL
            | wx.NO_BORDER
            | wx.WANTS_CHARS,
        )
        self.rich_text_serial_log2.Hide()

        sbSizer8.Add(self.rich_text_serial_log2, 1, wx.EXPAND | wx.ALL, 5)

        bSizer14 = wx.BoxSizer(wx.HORIZONTAL)

        self.text_ctrl_message = wx.TextCtrl(
            sbSizer8.GetStaticBox(),
            wx.ID_ANY,
            wx.EmptyString,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TE_PROCESS_ENTER,
        )
        self.text_ctrl_message.SetFont(
            wx.Font(
                9,
                wx.FONTFAMILY_MODERN,
                wx.FONTSTYLE_NORMAL,
                wx.FONTWEIGHT_NORMAL,
                False,
                "Consolas",
            )
        )

        bSizer14.Add(self.text_ctrl_message, 1, wx.ALL | wx.EXPAND, 5)

        self.button_send = wx.Button(
            sbSizer8.GetStaticBox(),
            wx.ID_ANY,
            u"Send",
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.BU_EXACTFIT,
        )
        bSizer14.Add(self.button_send, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALL, 0)

        sbSizer8.Add(bSizer14, 0, wx.EXPAND, 0)

        self.m_panel41.SetSizer(sbSizer8)
        self.m_panel41.Layout()
        sbSizer8.Fit(self.m_panel41)
        bSizer7.Add(self.m_panel41, 2, wx.EXPAND | wx.ALL, 5)

        bSizer6.Add(bSizer7, 1, wx.EXPAND, 5)

        self.SetSizer(bSizer6)
        self.Layout()

        self.Centre(wx.BOTH)

        # Connect Events
        self.Bind(wx.EVT_CLOSE, self.event_close)
        self.button_run_test.Bind(
            wx.EVT_BUTTON, self.event_button_run_test_click
        )
        self.button_view_results.Bind(
            wx.EVT_BUTTON, self.event_button_view_results_click
        )
        self.button_create_update.Bind(
            wx.EVT_BUTTON, self.event_button_create_update_click
        )
        self.button_delete.Bind(wx.EVT_BUTTON, self.event_button_delete_click)
        self.combo_box_test_choice.Bind(
            wx.EVT_COMBOBOX, self.event_combo_selection_made
        )
        self.combo_box_test_choice.Bind(
            wx.EVT_TEXT_ENTER, self.event_combo_on_text_enter
        )
        self.text_ctrl_test_code.Bind(
            wx.EVT_CHAR, self.event_text_ctrl_test_code_char
        )
        self.text_ctrl_test_code.Bind(
            wx.EVT_TEXT, self.event_text_ctrl_test_code_on_text
        )
        self.static_text_gcode_reference.Bind(
            wx.EVT_LEFT_DOWN, self.event_static_text_gcode_reference_click
        )
        self.static_text_reprap_link.Bind(
            wx.EVT_LEFT_DOWN, self.event_static_text_reprap_reference_click
        )
        self.rich_text_serial_log.Bind(
            wx.EVT_CHAR, self.event_rich_text_serial_log_char
        )
        self.text_ctrl_message.Bind(
            wx.EVT_TEXT_ENTER, self.event_text_ctrl_message_enter
        )
        self.button_send.Bind(wx.EVT_BUTTON, self.event_button_send_click)

    def __del__(self):
        pass

    # Virtual event handlers, overide them in your derived class
    def event_close(self, event):
        event.Skip()

    def event_button_run_test_click(self, event):
        event.Skip()

    def event_button_view_results_click(self, event):
        event.Skip()

    def event_button_create_update_click(self, event):
        event.Skip()

    def event_button_delete_click(self, event):
        event.Skip()

    def event_combo_selection_made(self, event):
        event.Skip()

    def event_combo_on_text_enter(self, event):
        event.Skip()

    def event_text_ctrl_test_code_char(self, event):
        event.Skip()

    def event_text_ctrl_test_code_on_text(self, event):
        event.Skip()

    def event_static_text_gcode_reference_click(self, event):
        event.Skip()

    def event_static_text_reprap_reference_click(self, event):
        event.Skip()

    def event_rich_text_serial_log_char(self, event):
        event.Skip()

    def event_text_ctrl_message_enter(self, event):
        event.Skip()

    def event_button_send_click(self, event):
        event.Skip()
