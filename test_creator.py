import math

prefix="""G21 ; set units to mm
G90 ; set mode to absolute
M83 ; set E to relative
M503 ; get configuration
M92 X80 Y80 Z400 E93 ; set steps/mm
M201 X500 Y500 Z500 E8000 ; set max accel per stepper (mm/s)
M203 X15000 Y15000 Z15000 E15000 ; set max feed rate (mm/min)
M204 P1000 R1000 T1000 ; set acceleration (mm/s^2)
M205 X5 Y5 Z0 T0 ; set jerk in (mm/s)
M121 ; disable endstops on Marlin (ignore error on Duet)
G92 E0 X0 Y0 Z0 ; set position to origin
M302 S0 ; allow cold extrusions
G4 P200 ; wait 200ms for stuff to be added to planner before moving"""

suffix = "M400"

filament_diameter_mm = 1.75

extruder_diameter_mm = 0.4

layer_height_mm = 0.2

layer_width_mm = 0.5

fill_ratio = 1.0

# speed in mm/s
speed_mm_s = 20.0

# create arc
radius = 100

# approximate length of each arc
target_segment_length_mm = 1.0

##################
# END OF OPTIONS #
##################

# segment count
segment_count = round(radius * math.pi / 2 / target_segment_length_mm)

commands = []

x1 = radius
y1 = 0
x2 = radius * math.cos(math.pi / 2 / segment_count)
y2 = radius * math.sin(math.pi / 2 / segment_count)
segment_length_mm = math.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)

commands.append('G1 F%.6f' % (speed_mm_s * 60))
commands.append('\n')
commands.append('; segments: %d' % segment_count)
commands.append('; segment length %g mm' % segment_length_mm)
commands.append('; speed %g mm/s' % speed_mm_s)

for i in range(segment_count + 1):
    x = radius * math.cos(i * math.pi / 2 / segment_count)
    y = radius * math.sin(i * math.pi / 2 / segment_count)
    if i == 0:
        length = 0
    else:
        length = segment_length_mm
    target_material_mm3 = fill_ratio * length * layer_height_mm * layer_width_mm
    e = target_material_mm3 / (math.pi / 4 * filament_diameter_mm ** 2)
    commands.append('G1 X%.6f Y%.6f E%.6f' % (x, y, e))
commands.append("G1 X0 Y0")

commands.insert(0, "\n")
commands.insert(0, prefix)
commands.append("\n")
commands.append(suffix)

commands = '\n'.join(commands)

print(commands)
