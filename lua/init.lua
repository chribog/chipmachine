------
-- Lua settings
--
print("IN LUA0");

TEXT_COLOR = 0xffe0e080
DIGITS_COLOR = 0xffb0b0a0

X0 = 80
Y0 = 54
X1 = 636
Y1 = 520
Settings.top_left = { X0, Y0 }
Settings.down_right = { X1, Y1 }

scale = 3.0
Settings.main_title = { X0, Y0, scale, TEXT_COLOR }
Settings.main_composer = { X0, Y0+25*scale, scale*0.6, TEXT_COLOR }
Settings.main_format = { X0, Y0+45*scale, scale*0.3, TEXT_COLOR }

scale = 1.2
x = 440
y = 340
Settings.next_field = { x, y-14, 0.5, 0xff444477 }
Settings.next_title = { x, y, scale, TEXT_COLOR }
Settings.next_composer = { x, y+25*scale, scale*0.6, TEXT_COLOR }
Settings.next_format = { x, y+45*scale, scale*0.3, TEXT_COLOR }

scale = 80.0
Settings.prev_title = { -3200, Y0, scale, 0 }
Settings.prev_composer = { -3200, Y0+25*scale, scale*0.6, 0 }
Settings.prev_format = { -3200, Y0+45*scale, scale*0.3, 0 }

Settings.time_field = { X0, Y0 + 200, 1.0, DIGITS_COLOR }
Settings.length_field = { X0 + 100, Y0 + 200, 1.0, DIGITS_COLOR }
Settings.song_field = { X0 + 220, Y0 + 200, 1.0, DIGITS_COLOR }

Settings.spectrum = { X0-80, Y1+50, 28, 16.0 }


Settings.font = "data/Neutra.otf"
------
print("Lua parsing done")