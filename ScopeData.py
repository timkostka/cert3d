import random

class ScopeData:
    """The ScopeData class holds data about edges for a given signal."""

    def __init__(self):
        # name of the channel
        self.name = 'DATA'
        # number of ticks per second
        self.seconds_per_tick = 1.0 / 180e6
        # starting time in seconds
        self.start = 0.0
        # length of each pulse in ticks, starting with low pulse
        self.data = []
        self.invent_data() # DEBUG

    def get_length(self):
        """Return the length of the data in seconds."""
        return sum(self.data) * self.seconds_per_tick

    def invent_data(self):
        """Populate with randomly generated data."""
        self.data = [random.randint(1, 10) for _ in range(100)]
        self.start = 0.0
