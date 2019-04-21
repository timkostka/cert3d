import random
import math
import wx

verbose = True

# hold stipple brushes
stipple_brushes = {}

# reduction_threshold
reduction_threshold = 0.25

# DEBUG
random.seed(0)


def create_stipple_bitmap(color):
    """Return the stipple bitmap for the given color."""
    rgb = color.GetRGB()
    # if we already created and cached this one, just return it
    if rgb in stipple_brushes:
        return stipple_brushes[rgb]
    # create a new bitmap stipple mask
    image = wx.Image(16, 16, clear=True)
    image.InitAlpha()
    for x in range(image.GetWidth()):
        for y in range(image.GetHeight()):
            if (2 * x + y) % 16 < 8:
                image.SetAlpha(x, y, wx.ALPHA_TRANSPARENT)
            else:
                rgb = (color.Red(), color.Green(), color.Blue())
                image.SetRGB(x, y, *rgb)
    bmp = wx.Bitmap(image)
    stipple_brushes[rgb] = bmp
    return bmp


'''
def create_signal_cluster(
    data, pixels_per_screen=2000, max_edges_per_screen=500
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
        # the median of the combined duration of each adjacent edge pair
        threshold = durations[round(len(durations) * reduction_threshold)]
        # get durations between adjacent edges
        durations = [y[0] - x[0] for x, y in zip(data[:-1], data[1:])]
        # location of new edges
        new_edges = []
        last_index = 0
        new_edges.append(data[0])
        while last_index < len(data) - 1:
            # collapse edges until threshold duration is met
            index = last_index + 1
            while (
                index < len(data) - 1
                and data[index][0] - data[index - 1][0] <= threshold
            ):
                index += 1
            if index > last_index + 1:
                new_edges.append((data[index][0], 2))
            else:
                new_edges.append(data[index])
            last_index = index
        data = new_edges
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
'''


class Data:
    """
    A Data is a supertype meant to be implemented for various data types.

    """

    def __init__(self):
        # start time of the data
        self.start_time = None
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 168e6
        # starting time in seconds
        self.start_time = 0.0

    def is_empty(self):
        """Return True if empty."""
        return self.get_point_count() == 0

    def get_x_from_time(self, time):
        """Return the x value corresponding to the given time."""
        return (time - self.start_time) / self.seconds_per_tick

    def get_time_at_index(self, index):
        """Return time at the given point index."""
        raise NotImplementedError

    def get_point_count(self):
        """Return the number of data points in this set."""
        raise NotImplementedError

    def get_length(self):
        """Return the length of the data."""
        if self.is_empty():
            return None
        return self.get_time_at_index(-1) - self.get_time_at_index(0)

    def get_edge_near_time(self, time):
        """Should return the time closest to the given time, or None."""
        raise NotImplementedError

    def draw_signal(
        self,
        dc: wx.DC,
        rect: wx.Rect,
        color: wx.Colour,
        thickness: int,
        left_time: float,
        pixels_per_second: float,
        low_value: float,
        high_value: float,
    ):
        """Draw the signal on the screen."""
        raise NotImplementedError

    def get_min_period_with_point_count(self, point_count):
        """Return the minimum period in seconds with X points."""
        raise NotImplementedError

    def get_reduced_data(self):
        """Approximate the data with a reduced set and return it."""
        raise NotImplementedError


class TriStateData(Data):
    """
    The TriStateData class holds data about edges for a given signal.

    For a given time, the signal is either low, high or tri-state.  The third
    state is displayed as high-z.
    """

    def __init__(self):
        # call higher level init
        super(TriStateData, self).__init__()
        # time corresponding to tick 0
        self.start_time = 0.0
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 168e6
        # (x, y) tuples for each state, where (y=0 low, 1 high, or 2 tri-state)
        # y gives the value of the data prior to this time
        self.points = []
        self.invent_data(1000)  # DEBUG

    def is_empty(self):
        """Return True if empty."""
        return not self.points

    def get_point_count(self):
        """Return the number of data points in this set."""
        return len(self.points)

    def get_edge_near_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        index = self.find_closest_index(target_time)
        if index is None:
            return None
        return self.get_time_at_index(index)

    def find_index_after(self, target_time):
        """Return the first index at or after the given time."""
        if self.is_empty():
            return None
        low = 0
        high = len(self.points) - 1
        # loop until low and high are adjacent
        while low < high:
            test = (low + high) // 2
            time = self.get_time_at_index(test)
            if time < target_time:
                assert low < test + 1
                low = test + 1
            else:
                assert high > test
                high = test
        return low

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.points[index][0] * self.seconds_per_tick

    def get_min_period_with_point_count(self, point_count):
        """Return the minimum period in seconds with X points."""
        assert point_count > 1
        if self.get_point_count() <= point_count:
            return float("inf")
        ticks = min(
            y[0] - x[0]
            for x, y in zip(
                self.points[:-point_count], self.points[point_count:]
            )
        )
        return ticks * self.seconds_per_tick

    def find_closest_index(self, target_time):
        """Return the data index closest to the given time."""
        if self.is_empty():
            return None
        if target_time < self.start_time:
            return 0
        index = self.find_index_after(target_time)
        if index is None:
            return len(self.points) - 1
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
        new_value = random.randint(0, 2)
        self.points = []
        self.points.append((tick_count, new_value))
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
            self.points.append((tick_count, new_value))

    def draw_signal(
        self,
        dc,
        rect,
        color,
        thickness,
        left_time,
        pixels_per_second,
        low_value,
        high_value,
    ):
        """Draw the signal on the screen."""
        # if it's empty, nothing to draw
        if self.is_empty():
            return
        # create solid brush for drawing edges
        solid_pen = wx.Pen(color, 1)
        solid_brush = wx.Brush(color)
        # create stipple brush for third state data
        gray_color = wx.Colour(
            (color.Red() + 1) * 2 // 3,
            (color.Green() + 1) * 2 // 3,
            (color.Blue() + 1) * 2 // 3,
        )
        if True:
            gray_brush = wx.Brush(gray_color)
            bmp = create_stipple_bitmap(gray_color)
        else:
            gray_brush = wx.Brush(color)
            bmp = create_stipple_bitmap(color)
        gray_brush.SetStipple(bmp)
        # set the pen to use (but not the brush)
        dc.SetPen(solid_pen)
        # clip to the specified region
        dc.SetClippingRegion(*rect)
        # alias some things to shorter names
        y1 = rect[1]
        y2 = y1 + rect[3] - 1
        height = rect[3]
        width = rect[2]
        left = rect[0]
        # find first index within window
        index = self.find_index_after(left_time)
        if index > 0:
            index -= 1
        # find first index after window
        right_index = self.find_index_after(
            left_time + width / pixels_per_second
        )
        time = self.get_time_at_index(index)
        x2 = left + round((time - left_time) * pixels_per_second)
        for _ in range(right_index - index):
            x1 = x2
            # draw transition on leading edge
            if index > 0:
                dc.DrawRectangle(x1, y1, thickness, height)
            # go to the next value
            index += 1
            time = self.get_time_at_index(index)
            value = self.points[index][1]
            x2 = left + round((time - left_time) * pixels_per_second)
            # if it's a high or low edge, draw it
            if value == 0 or value == 1:
                dc.SetBrush(solid_brush)
                y = y2 - thickness + 1 if value == 0 else y1
                dc.DrawRectangle(x1, y, x2 - x1 + thickness, thickness)
            else:
                dc.SetBrush(gray_brush)
                dc.DrawRectangle(x1, y1, x2 - x1 + thickness, y2 - y1 + 1)
        dc.DestroyClippingRegion()

    def validate(self):
        """Raise an error is the data is invalid."""
        # if empty, it's fine
        if self.is_empty():
            return True
        # must have at least 2 points
        assert self.get_point_count() >= 2
        # ensure data points are ascending
        assert all(
            self.points[i + 1][0] >= self.points[i][0]
            for i in range(len(self.points) - 1)
        )
        # ensure adjacent points apart from the first two are different types
        check = all(
            self.points[i + 1][1] != self.points[i][1]
            for i in range(len(self.points) - 1, 1)
        )
        # DEBUG
        if not check:
            print([x[1] for x in self.points[:50]])
            print([x[1] for x in self.points[-50:]])
        assert check
        return True

    def get_reduced_data(self):
        """Approximate the data with a reduced set and return it."""
        # can only reduce if we have enough data
        if self.get_point_count() < 3:
            return
        # ensure data is valid
        self.validate()
        # find durations for consecutive pairs of durations
        durations = [
            y[0] - x[0] for x, y in zip(self.points[:-2], self.points[2:])
        ]
        durations.sort()
        # get threshold duration to collapse
        # the median of the combined duration of each adjacent edge pair
        threshold = 2 * durations[round(len(durations) * reduction_threshold)]
        del durations
        # old index of last edge in the new data set
        last_index = 0
        # add the first edge
        new_points = []
        new_points.append(self.points[0])
        for i in range(len(self.points) - 1):
            # collapse if value is high-z or if duration is under threshold
            collapse = (
                self.points[i + 1][0] - self.points[i][0] <= threshold
                or self.points[i + 1][1] == 2
            )
            if not collapse:
                # add previous region if necessary
                if last_index != i:
                    if last_index == i - 1:
                        new_points.append(self.points[i])
                    else:
                        new_points.append((self.points[i][0], 2))
                # add this region
                new_points.append(self.points[i + 1])
                last_index = i + 1
        # add last region if necessary
        if last_index != len(self.points) - 1:
            if last_index == len(self.points) - 2:
                new_points.append(self.points[-1])
            else:
                new_points.append((self.points[-1][0], 2))
        # create new data
        new_data = TriStateData()
        new_data.start_time = self.start_time
        new_data.seconds_per_tick = self.seconds_per_tick
        new_data.points = new_points
        new_data.validate()
        return new_data


class BilevelData(Data):
    """The BilevelData class holds data about edges for a given signal."""

    def __init__(self):
        # call higher level init
        super(BilevelData, self).__init__()
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

    def validate(self):
        """Raise an error is the data is invalid."""
        # if empty, it's fine
        if self.is_empty():
            return True
        # must have at least 2 points
        assert self.get_point_count() >= 2
        # ensure data points are ascending
        check = all(
            self.edges[i + 1] >= self.edges[i]
            for i in range(len(self.edges) - 1)
        )
        if not check:
            delta = [
                self.edges[i + 1] >= self.edges[i]
                for i in range(len(self.edges) - 1)
            ]
            indices = [i for i, x in enumerate(delta) if x <= 0]
            print(indices)
            print([self.edges[i - 1 : i + 2] for i in indices][:5])
        # DEBUG
        if not check:
            self.edges.sort()
        # assert check
        return True

    def is_empty(self):
        """Return True if empty."""
        return not self.edges

    def get_point_count(self):
        """Return the number of data points in this set."""
        return len(self.edges)

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

    def get_min_period_with_point_count(self, point_count):
        """Return the minimum period in seconds with X points."""
        assert point_count > 1
        if self.get_point_count() <= point_count:
            return float("inf")
        ticks = min(
            y - x
            for x, y in zip(
                self.edges[:-point_count], self.edges[point_count:]
            )
        )
        return ticks * self.seconds_per_tick

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

    def draw_signal(
        self,
        dc,
        rect,
        color,
        thickness,
        left_time,
        pixels_per_second,
        low_value,
        high_value,
    ):
        """Draw the signal on the screen."""
        # if it's empty, nothing to draw
        if self.is_empty():
            return
        # set pen and brush
        dc.SetPen(wx.Pen(color, 1))
        dc.SetBrush(wx.Brush(color))
        # clip to the specified region
        dc.SetClippingRegion(*rect)
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
        # find first index to left of window
        index = self.find_index_after(left_time)
        if index > 0:
            index -= 1
        # find first index to the right of the window
        right_index = self.find_index_after(
            left_time + width / pixels_per_second
        )
        # get the correct signal polarity
        if index % 2 == 1:
            signal_low = not signal_low
        # get time at the index
        time = self.get_time_at_index(index)
        x2 = left + round((time - left_time) * pixels_per_second)
        for _ in range(right_index - index):
            x1 = x2
            # draw transition on leading edge
            if index > 0:
                dc.DrawRectangle(x1, y1, thickness, height)
            # go to the next value
            index += 1
            signal_low = not signal_low
            time = self.get_time_at_index(index)
            x2 = left + round((time - left_time) * pixels_per_second)
            y = y2 - thickness + 1 if signal_low else y1
            dc.DrawRectangle(x1, y, x2 - x1 + thickness, thickness)
        dc.DestroyClippingRegion()

    def get_reduced_data(self):
        """Approximate the data with a reduced set and return it."""
        # can only reduce if we have enough data
        if self.get_point_count() < 3:
            return
        # ensure data is monotonically increasing
        self.validate()
        # find durations for consecutive pairs of edges
        durations = [y - x for x, y in zip(self.edges[:-2], self.edges[2:])]
        durations.sort()
        # get threshold duration to collapse
        # the median of the combined duration of each adjacent edge pair
        threshold = 2 * durations[round(len(durations) * reduction_threshold)]
        del durations
        # old index of last edge in the new data set
        last_index = 0
        # add the first edge
        new_points = []
        new_points.append((self.edges[0], 0))
        # print('edges =', self.edges[-10:])
        value = 0 if self.start_high else 1
        for i in range(len(self.edges) - 1):
            value = (value + 1) % 2
            if self.edges[i + 1] - self.edges[i] > threshold:
                # add previous region if necessary
                if last_index != i:
                    if last_index == i - 1:
                        new_points.append((self.edges[i], (value + 1) % 2))
                    else:
                        new_points.append((self.edges[i], 2))
                # add this region
                new_points.append((self.edges[i + 1], value))
                last_index = i + 1
        # add last region if necessary
        if last_index != len(self.edges) - 1:
            if last_index == len(self.edges) - 2:
                new_points.append((self.edges[-1], value))
            else:
                new_points.append((self.edges[-1], 2))
        # create new data
        new_data = TriStateData()
        new_data.start_time = self.start_time
        new_data.seconds_per_tick = self.seconds_per_tick
        new_data.points = new_points
        new_data.validate()
        return new_data


class PlotData(Data):
    """The PlotData class holds xy plot data for a given signal."""

    def __init__(self):
        # call higher level init
        super(PlotData, self).__init__()
        # name of the data
        self.name = "DATA"
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 168e6
        # starting time in seconds
        self.start_time = 0.0
        # xy plot data, where x is in ticks
        self.points = []
        self.invent_data()  # DEBUG

    def is_empty(self):
        return not self.points

    def get_point_count(self):
        return len(self.points)

    def invent_data(self, point_count=2000):
        """Populate with randomly generated data."""
        self.start_time = 0.0
        phi = random.uniform(0.0, math.tau)
        period = random.uniform(20, 40)
        self.points = []
        for i in range(point_count):
            self.points.append((5 * i, math.sin(phi + i * math.tau / period)))

    def get_closest_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        return None

    def get_min_period_with_point_count(self, point_count):
        """Return the minimum period in seconds with X points."""
        # TODO: implement this
        return float("inf")

    def get_reduced_data(self):
        """Approximate the data with a reduced set and return it."""
        return None

    def draw_signal(
        self,
        dc,
        rect,
        color,
        thickness,
        left_time,
        pixels_per_second,
        low_value,
        high_value,
    ):
        """Draw the signal on the screen."""
        # if it's empty, nothing to draw
        if self.is_empty():
            return
        # set clipping region
        dc.SetClippingRegion(*rect)
        # set pen
        dc.SetPen(wx.Pen(color, thickness))
        # get pixels per tick (x scaling)
        pixels_per_tick = self.seconds_per_tick * pixels_per_second
        # x pixel of start of channel data
        channel_left = (
            rect[0]
            + (self.start_time - left_time) * pixels_per_second
        )
        # alias some things to shorter names
        top = rect[1]
        bottom = rect[1] + rect[3] - 1
        height = rect[3]
        width = rect[2]
        left = rect[0]
        right = left + width - 1
        # get pixels per value (y scaling)
        pixels_per_value = (height - 1) / (low_value - high_value)
        x2 = None
        y2 = None
        for point in self.points:
            x1, y1 = x2, y2
            x2 = int(channel_left + point[0] * pixels_per_tick + 0.5)
            y2 = int(top + (point[1] - high_value) * pixels_per_value + 0.5)
            # exit if we're drawing offscreen
            if x1 is None:
                continue
            if x1 > right:
                break
            #if x2 >= left:
            dc.DrawLine(x1, y1, x2, y2)
        dc.DestroyClippingRegion()
        return


        # find first index to left of window
        index = self.find_index_after(left_time)
        if index > 0:
            index -= 1
        # find first index to the right of the window
        right_index = self.find_index_after(
            left_time + width / pixels_per_second
        )
        # get the correct signal polarity
        if index % 2 == 1:
            signal_low = not signal_low
        # get time at the index
        time = self.get_time_at_index(index)
        x2 = left + round((time - left_time) * pixels_per_second)
        for _ in range(right_index - index):
            x1 = x2
            # draw transition on leading edge
            if index > 0:
                dc.DrawRectangle(x1, y1, thickness, height)
            # go to the next value
            index += 1
            signal_low = not signal_low
            time = self.get_time_at_index(index)
            x2 = left + round((time - left_time) * pixels_per_second)
            y = y2 - thickness + 1 if signal_low else y1
            dc.DrawRectangle(x1, y, x2 - x1 + thickness, thickness)
        dc.DestroyClippingRegion()

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.points[index][0] * self.seconds_per_tick

    def find_index_after(self, target_time):
        """Return the first index at or after the given time."""
        if not self.points:
            return None
        low = 0
        high = len(self.points) - 1
        # loop until low and high are adjacent
        while low < high:
            test = (low + high) // 2
            time = self.get_time_at_index(test)
            if time < target_time:
                assert low < test + 1
                low = test + 1
            else:
                assert high > test
                high = test
        return low

    def find_closest_index(self, target_time):
        """Return the data index closest to the given time."""
        if not self.points:
            return None
        if target_time < self.start_time:
            return 0
        index = self.find_index_after(target_time)
        if index is None:
            return self.get_point_count() -1
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
