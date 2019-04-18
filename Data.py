import random
import math
import wx

verbose = True


def create_signal_cluster(
    data, pixels_per_screen=2000, max_edges_per_screen=5
):
    """
    Create a signal cluster out of the given data array.

    Data is assumed to be in (x, y) format where x is a value in ticks.

    """
    # screen size in pixels
    # pixels_per_screen = asize(1000)
    # maximum number of edges on screen at once
    # max_edges_per_screen = 1000
    # hold data set cluster
    cluster = []
    # add original data set
    cluster.append([0, data])
    # loop until data set is small enough
    while len(data) > max_edges_per_screen:
        assert all(data[i + 1][0] >= data[i][0] for i in range(len(data) - 1))
        # find edge durations
        durations = [y[0] - x[0] for x, y in zip(data[:-2], data[2:])]
        durations.sort()
        # get threshold duration to collapse
        threshold = durations[len(durations) // 2]
        new_data = []
        last_index = 0
        new_data.append(data[0])
        while last_index < len(data) - 1:
            # collapse edges until threshold duration is met
            index = last_index + 1
            while (
                index < len(data) - 1
                and data[index][0] - data[index - 1][0] <= threshold
            ):
                index += 1
            if index > last_index + 1:
                new_data.append((data[index][0], 2))
            else:
                new_data.append(data[index])
            last_index = index
        data = new_data
        cluster.append([0, data])
    # TODO: find min zoom levels for each data set
    for index, (zoom, data) in enumerate(cluster):
        # if we can display all points at once, there is no minimum zoom
        if len(data) <= max_edges_per_screen:
            cluster[index][0] = 0.0
            continue
        meps = max_edges_per_screen
        min_duration = min(
            y[0] - x[0] for x, y in zip(data[:-meps], data[meps:])
        )
        min_zoom = pixels_per_screen / min_duration
        cluster[index][0] = min_zoom
    # output summary
    if verbose:
        print(
            "Collapsed data set (length %g sec) into the following cluster:"
            % (data[-1][0] - data[0][0])
        )
        for zoom, data in cluster:
            print("- %g pixels/second: %d data points" % (zoom, len(data)))
    return cluster


class DataCluster:
    """A DataCluster contains a set of data sets for different zoom levels."""

    def __init__(self, data):
        # and data_set is a TriStateData or BilevelData or PlotData object
        self.clusters = create_signal_cluster(data)
        # time in seconds corresponding to x=0 value
        self.start_time = 0.0
        # data is given with x in seconds
        self.name = "DATA"

    def get_data(self, pixels_per_second):
        """Return the data set at the given zoom level."""
        for (zoom, data) in self.clusters:
            if pixels_per_second > zoom:
                return data
        # if we didn't find an acceptable one, return the smallest set
        return self.clusters[-1][1]

    def get_master_data(self):
        """Return the master data set."""
        return self.clusters[0][1]

    def get_total_duration(self):
        """Return the total duration of this data."""
        data = self.get_master_data()
        if not data:
            return 0.0
        return data[-1][0] - data[0][0]


class Data:
    """
    A Data is a supertype meant to be implemented for various data types.

    """

    def __init__(self):
        # start time of the data
        self.start_time = None

    def get_edge_near_time(self, time):
        """Should return the time closest to the given time, or None."""
        raise NotImplementedError

    def get_length(self, time):
        """Return the length of the data."""
        raise NotImplementedError

    def draw_signal(
        self,
        dc: wx.DC,
        rect: wx.Rect,
        color: wx.Colour,
        thickness: int,
        left_time: float,
        pixels_per_second: float,
    ):
        raise NotImplementedError


class TriStateData(Data):
    """
    The TriStateData class holds data about edges for a given signal.

    For a given time, the signal is either low, high or tri-state.  The third
    state is displayed as high-z.
    """

    def __init__(self):
        # name of the channel
        # self.name = "DATA"
        # number of ticks per second
        # self.seconds_per_tick = 1.0 / 168e6
        # time offset
        self.start_time = 0.0
        # (x, y) tuples for each state, where (y=0 low, 1 high, or 2 tri-state)
        # y gives the value of the data prior to this time
        self.points = []
        self.invent_data(1000)  # DEBUG

    def get_length(self):
        """Return the length of the data in seconds, or None."""
        if not self.data:
            return None
        return (self.data[-1][0] - self.data[0][0]) * self.seconds_per_tick

    def find_index_after(self, target_time):
        """Return the first index at or after the given time."""
        if not self.data:
            return None
        low = 0
        high = len(self.data) - 1
        # loop until low and high are adjacent
        while low < high:
            test = (low + high) // 2
            time = self.start_time + self.data[test][0] * self.seconds_per_tick
            if time < target_time:
                assert low < test + 1
                low = test + 1
            else:
                assert high > test
                high = test
        return low

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.data[index][0] * self.seconds_per_tick

    def find_closest_index(self, target_time):
        """Return the data index closest to the given time."""
        if not self.data:
            return None
        if target_time < self.start_time:
            return 0
        index = self.find_index_after(target_time)
        if index is None:
            return len(self.data) - 1
        # else it's either index or index - 1
        if index == 0:
            return index
        low = self.get_time_at_index(index - 1)
        high = self.get_time_at_index(index)
        if abs(target_time - low) < abs(target_time - high):
            return index - 1
        else:
            return index

    def invent_data(self, length=200):
        """Populate with randomly generated data."""
        tick_count = 0
        new_value = 0
        self.data = []
        self.data.append((tick_count, new_value))
        for _ in range(length):
            tick_count += round(1.0 + 5.0 * random.random())
            if random.random() < 0.01:
                tick_count += 100 * round(1.0 + 5.0 * random.random())
            if random.random() < 0.001:
                tick_count += 2000 * round(1.0 + 5.0 * random.random())
            value = new_value
            new_value = random.randint(0, 1)
            if value == new_value:
                new_value = 2
            self.data.append((tick_count, new_value))

    def get_closest_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        index = self.find_closest_index(target_time)
        if index is None:
            return None
        return self.get_time_at_index(index)


class PlotData:
    """The PlotData class holds xy plot data for a given signal."""

    def __init__(self):
        # name of the data
        self.name = "DATA"
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 180e6
        # starting time in seconds
        self.start_time = 0.0
        # xy plot data, where x is in ticks
        self.data = []
        self.invent_data()  # DEBUG

    def get_length(self):
        """Return the length of the data in seconds."""
        if not self.data:
            return 0
        return self.data[-1][0] * self.seconds_per_tick

    def invent_data(self):
        """Populate with randomly generated data."""
        self.start_time = 0.0
        phi = random.uniform(0.0, math.tau)
        period = random.uniform(20, 40)
        for i in range(12000):
            self.data.append((5 * i, math.sin(phi + i * math.tau / period)))

    def get_closest_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        if len(self.data) == 0:
            return None
        time = self.start_time
        for tick_count, _ in self.data:
            this_time = self.start_time + tick_count * self.seconds_per_tick
            if abs(this_time - target_time) < abs(time - target_time):
                time = this_time
        return time


class BilevelData(Data):
    """The BilevelData class holds data about edges for a given signal."""

    def __init__(self):
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 168e6
        # starting time in seconds
        self.start_time = 0.0
        # if True, signal starts high
        self.start_high = False
        # time of each edge in ticks
        self.edges = []
        # invent random data
        self.invent_data(10000)  # DEBUG

    def invent_data(self, length=200):
        """Populate with randomly generated data."""
        self.start_high = random.choice([True, False])
        self.edges = []
        tick_count = 0
        self.edges.append(tick_count)
        for _ in range(length):
            tick_count += round(1.0 + 5.0 * random.random())
            if random.random() < 0.01:
                tick_count += 100 * round(1.0 + 5.0 * random.random())
            if random.random() < 0.001:
                tick_count += 2000 * round(1.0 + 5.0 * random.random())
            self.edges.append(tick_count)

    def get_length(self):
        """Return the length of the data in seconds, or None."""
        if not self.edges:
            return None
        return (self.edges[-1] - self.edges[0]) * self.seconds_per_tick

    def find_index_after(self, target_time):
        """Return the first index at or after the given time."""
        if not self.edges:
            return None
        low = 0
        high = len(self.edges) - 1
        # loop until low and high are adjacent
        while low < high:
            test = (low + high) // 2
            time = self.start_time + self.edges[test] * self.seconds_per_tick
            if time < target_time:
                assert low < test + 1
                low = test + 1
            else:
                assert high > test
                high = test
        return low

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.edges[index] * self.seconds_per_tick

    def find_closest_index(self, target_time):
        """Return the data index closest to the given time."""
        if not self.edges:
            return None
        if target_time < self.start_time:
            return 0
        index = self.find_index_after(target_time)
        if index is None:
            return len(self.edges) - 1
        # else it's either index or index - 1
        if index == 0:
            return index
        low = self.get_time_at_index(index - 1)
        high = self.get_time_at_index(index)
        if abs(target_time - low) < abs(target_time - high):
            return index - 1
        else:
            return index

    def get_edge_near_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        index = self.find_closest_index(target_time)
        if index is None:
            return None
        return self.get_time_at_index(index)
        if len(self.data) == 0:
            return None
        time = self.start_time
        tick_count = 0
        for ticks in self.data:
            tick_count += ticks
            this_time = self.start_time + tick_count * self.seconds_per_tick
            if abs(this_time - target_time) < abs(time - target_time):
                time = this_time
        return time

    def draw_signal(
        self, dc, rect, color, thickness, left_time, pixels_per_second
    ):
        """Draw the signal on the display."""
        # set pen and brush
        dc.SetPen(wx.Pen(color, 1))
        dc.SetBrush(wx.Brush(color))
        # clip to the specified region
        dc.SetClippingRegion(*rect)
        # alias the underlying data type
        # edges = self.edges
        # get pixels per tick
        # pixels_per_tick = self.seconds_per_tick * pixels_per_second
        # find x pixel of start of channel data
        # channel_left = (
        #    rect[0]
        #    + (data.start_time - self.start_time) / self.seconds_per_pixel
        # )
        # adjust for thickness of line
        # channel_left -= (signal.thickness - 1) / 2.0
        # true if signal is low
        # note we start on the opposite edge, since we flip it before drawing
        # the first plateau
        signal_low = self.start_high
        # alias some things to shorter names
        y1 = rect[1]
        y2 = y1 + rect[3] - 1
        height = rect[3]
        width = rect[2]
        left = rect[0]
        right = rect[0] + rect[2] - 1
        # find first index to left of window
        index = self.find_index_after(left_time)
        if index > 0:
            index -= 1
        # if we're outside the data window, there is nothing to draw
        # if index is None:
        #    dc.DestroyClippingRegion()
        #    return
        # find first index to the right of the window
        right_index = self.find_index_after(
            left_time + width / pixels_per_second
        )
        """
        # if there are more than 1 edge per pixel, just draw a grayed out signal
        if right_index - index > (right - left) * 0.5:
            # clip rectangle to data region
            left_2 = self.get_x_from_time(signal.data.get_time_at_index(0))
            right_2 = self.get_x_from_time(signal.data.get_time_at_index(-1))
            left = max(left, left_2)
            right = min(right, right_2)
            # set pen and brush to stipple pattern
            pen = wx.Pen(signal.color, 1)
            brush = wx.Brush(signal.color)
            bmp = self.create_stipple_bitmap(signal.color)
            brush.SetStipple(bmp)
            pen.SetStipple(bmp)
            dc.SetPen(pen)
            dc.SetBrush(brush)
            # draw the rectangle
            dc.DrawRectangle(left, y1, right - left + 1, y2 - y1 + 1)
            dc.DestroyClippingRegion()
            return
        """
        # get the correct signal polarity
        if index % 2 == 1:
            signal_low = not signal_low

        # get time at the index
        time = self.get_time_at_index(index)
        # time = data.start_time + data.data[index] * data.seconds_per_tick
        x2 = left + round((time - left_time) * pixels_per_second)
        data_length = len(self.edges)
        for _ in range(right_index - index):
            x1 = x2
            # if we're past the viewing window, we're done
            # if x1 > right:
            #    break
            # if not the first point, draw the vertical line
            # if i2 > 0: # and left <= x1 <= right:
            if index > 0 and index < data_length - 1:
                dc.DrawRectangle(x1, y1, thickness, height)
            # go to the next value
            index += 1
            signal_low = not signal_low
            # if index >= len(data.data):
            #    break
            time = self.get_time_at_index(index)
            # ticks += length
            x2 = left + round((time - left_time) * pixels_per_second)
            # if in range, draw the edge
            # if False and (x2 < left or x1 > right):
            #    pass
            # else:
            if x1 <= right and x2 >= left:
                y = y2 - thickness + 1 if signal_low else y1
                dc.DrawRectangle(x1, y, x2 - x1 + thickness, thickness)
        dc.DestroyClippingRegion()
