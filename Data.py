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


def get_stipple_brush(color):
    """Return the stipple bitmap for the given color."""
    color = wx.Colour(
        (color.Red() + 1) * 1 // 4,
        (color.Green() + 1) * 1 // 4,
        (color.Blue() + 1) * 1 // 4,
    )
    rgb = color.GetRGB()
    # if we already created and cached this one, just return it
    if rgb in stipple_brushes:
        return stipple_brushes[rgb]
    # create a new bitmap stipple mask
    image = wx.Image(16, 16, clear=True)
    image.InitAlpha()
    for x in range(image.GetWidth()):
        for y in range(image.GetHeight()):
            if False and (2 * x + y) % 16 < 12:
                image.SetAlpha(x, y, wx.ALPHA_TRANSPARENT)
            else:
                rgb = (color.Red(), color.Green(), color.Blue())
                image.SetRGB(x, y, *rgb)
    brush = wx.Brush(color)
    brush.SetStipple(wx.Bitmap(image))
    stipple_brushes[rgb] = brush
    return brush


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

    def get_time_at_index(self, index):
        """Return time at the given point index."""
        raise NotImplementedError

    def get_point_count(self):
        """Return the number of data points in this set."""
        raise NotImplementedError

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

    def find_index_after(self, target_time):
        """Return the first index after the given time, or None."""
        if self.is_empty():
            return None
        low = 0
        high = self.get_point_count() - 1
        # loop until low and high are adjacent
        while low < high:
            test = (low + high) // 2
            time = self.get_time_at_index(test)
            if time <= target_time:
                assert low < test + 1
                low = test + 1
            else:
                assert high > test
                high = test
        return low

    def find_closest_index(self, target_time):
        """Return the data index closest to the given time."""
        if self.is_empty():
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

    def get_length(self):
        """Return the length of the data."""
        if self.is_empty():
            return None
        return self.get_time_at_index(-1) - self.get_time_at_index(0)

    def get_x_from_time(self, time):
        """Return the x value corresponding to the given time."""
        return (time - self.start_time) / self.seconds_per_tick


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
        gray_brush = get_stipple_brush(color)
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
        right = rect[0] + rect[2] - 1
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
        if x2 < left - thickness:
            x2 = left - thickness
        elif x2 > right + thickness:
            x2 = right - thickness
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
            if x2 < left - thickness:
                x2 = left - thickness
            elif x2 > right + thickness:
                x2 = right - thickness
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

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.edges[index] * self.seconds_per_tick

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
        right = rect[0] + rect[2] - 1
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
        if x2 < left - thickness:
            x2 = left - thickness
        elif x2 > right + thickness:
            x2 = right - thickness
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
            if x2 < left - thickness:
                x2 = left - thickness
            elif x2 > right + thickness:
                x2 = right - thickness
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

    def optimize(self):
        """Reduce the number of points if possible."""
        i = 0
        # look for straight line segments and delete points is possible
        while i < len(self.points) - 2:
            i += 1
            x1, y1 = self.points[i - 1]
            x2, y2 = self.points[i]
            x3, y3 = self.points[i + 1]
            alpha = (x2 - x1) / (x3 - x1)
            y_test = y1 + (y3 - y1) * alpha
            if y2 == y_test:
                del self.points[i]
                i -= 1

    def invent_data(self, point_count=2000):
        """Populate with randomly generated data."""
        self.start_time = 0.0
        phi = random.uniform(0.0, math.tau)
        period = random.uniform(20, 40)
        self.points = []
        for i in range(point_count):
            self.points.append((5 * i, math.sin(phi + i * math.tau / period)))

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

    def get_reduced_data(self):
        """Approximate the data with a reduced set and return it."""
        # reduce number of points by about this factor
        scaling = 4
        # get time range
        start = self.get_time_at_index(0)
        end = self.get_time_at_index(-1)
        if start == end:
            return None
        # get time values at each index
        point_count = self.get_point_count()
        new_point_count = round(point_count / scaling)
        if new_point_count < 2:
            return None
        last_index = 0
        points = [(self.points[0][0], 0, 0)]
        for i in range(1, new_point_count + 1):
            time = start + (end - start) * i / new_point_count
            next_index = self.find_closest_index(time)
            # get min value
            if next_index == last_index:
                continue
            else:
                low = min(
                    x[1] for x in self.points[last_index : next_index + 1]
                )
                high = max(
                    x[1] for x in self.points[last_index : next_index + 1]
                )
            points.append((self.points[next_index][0], low, high))
            last_index = next_index
        data = FuzzyPlotData()
        data.points = points
        data.start_time = self.start_time
        data.seconds_per_tick = self.seconds_per_tick
        return data

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
            rect[0] + (self.start_time - left_time) * pixels_per_second
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
        # find first index to draw
        left_index = self.find_index_after(left_time)
        if left_index:
            left_index -= 1
        # find last time
        right_time = left_time + width / pixels_per_second
        right_index = self.find_index_after(right_time)
        for point in self.points[left_index : right_index + 1]:
            x1, y1 = x2, y2
            x2 = int(channel_left + point[0] * pixels_per_tick + 0.5)
            y2 = int(top + (point[1] - high_value) * pixels_per_value + 0.5)
            # exit if we're drawing offscreen
            if x1 is None:
                continue
            if x1 > right:
                break
            # if x2 >= left:
            # dc.DrawPoint(x2, y2)
            dc.DrawLine(x1, y1, x2, y2)
        dc.DestroyClippingRegion()

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.points[index][0] * self.seconds_per_tick

    def get_edge_near_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        index = self.find_closest_index(target_time)
        if index is None:
            return None
        return self.get_time_at_index(index)


class FuzzyPlotData(Data):
    """The FuzzyPlotData holds reduced PlotData."""

    def __init__(self):
        # call higher level init
        super(FuzzyPlotData, self).__init__()
        # name of the data
        self.name = "DATA"
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 168e6
        # starting time in seconds
        self.start_time = 0.0
        # xy plot data, where x is in ticks and ylow and yhigh are bounds for
        # the previous region
        # [x, ylow, yhigh]
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
            low, high = sorted(random.random() for _ in range(2))
            self.points.append((5 * i, low, high))

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

    def get_time_at_index(self, index):
        """Return the time at the given data index."""
        return self.start_time + self.points[index][0] * self.seconds_per_tick

    def get_edge_near_time(self, target_time):
        """Return the edge time closest to the target time, or None."""
        index = self.find_closest_index(target_time)
        if index is None:
            return None
        return self.get_time_at_index(index)

    def get_reduced_data(self):
        """Approximate the data with a reduced set and return it."""
        # reduce number of points by about this factor
        scaling = 4
        # get time range
        start = self.get_time_at_index(0)
        end = self.get_time_at_index(-1)
        if start == end:
            return None
        # get time values at each index
        point_count = self.get_point_count()
        new_point_count = round(point_count / scaling)
        if new_point_count < 2:
            return None
        last_index = 0
        points = [(self.points[0][0], 0, 0)]
        for i in range(1, new_point_count + 1):
            time = start + (end - start) * i / new_point_count
            next_index = self.find_closest_index(time)
            # get min value
            if next_index == last_index:
                continue
            else:
                low = min(
                    x[1] for x in self.points[last_index : next_index + 1]
                )
                high = max(
                    x[2] for x in self.points[last_index : next_index + 1]
                )
            points.append((self.points[next_index][0], low, high))
            last_index = next_index
        data = FuzzyPlotData()
        data.points = points
        data.start_time = self.start_time
        data.seconds_per_tick = self.seconds_per_tick
        return data

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
        solid_pen = wx.Pen(color, thickness)
        dc.SetPen(solid_pen)
        gray_brush = get_stipple_brush(color)
        dc.SetBrush(gray_brush)
        gray_pen = wx.Pen(gray_brush.GetColour())
        gray_pen.SetStipple(gray_brush.GetStipple())
        # get pixels per tick (x scaling)
        pixels_per_tick = self.seconds_per_tick * pixels_per_second
        # x pixel of start of channel data
        channel_left = (
            rect[0] + (self.start_time - left_time) * pixels_per_second
        )
        # alias some things to shorter names
        top = rect[1]
        height = rect[3]
        width = rect[2]
        left = rect[0]
        right = left + width - 1
        # get pixels per value (y scaling)
        pixels_per_value = (height - 1) / (low_value - high_value)
        x2 = None
        y2low, y2high = 0, 0
        # find first index to draw
        left_index = self.find_index_after(left_time)
        if left_index:
            left_index -= 1
        # find last time
        right_time = left_time + width / pixels_per_second
        right_index = self.find_index_after(right_time)
        for point in self.points[left_index : right_index + 1]:
            x1 = x2
            y1low, y1high = y2low, y2high
            x2 = round(channel_left + point[0] * pixels_per_tick)
            low, high = point[1], point[2]
            assert low <= high
            y2high = round(top + (low - high_value) * pixels_per_value)
            y2low = round(top + (high - high_value) * pixels_per_value)
            assert y2low <= y2high
            # exit if we're drawing offscreen
            if x1 is None:
                continue
            if x1 > right:
                break
            assert y2high >= y2low
            if y2high == y2low:
                dc.DrawLine(x1, y2low, x2, y2low)
            else:
                dc.DrawRectangle(x1, y2low, x2 - x1 + 1, y2high - y2low + 1)
                low = max(y1low, y2low) + 1
                high = min(y1high, y2high) - 1
                if low <= high:
                    dc.SetPen(gray_pen)
                    dc.DrawLine(x1, low, x1, high)
                    dc.SetPen(solid_pen)
        dc.DestroyClippingRegion()
