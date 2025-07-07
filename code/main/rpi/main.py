import serial
import time

from roboeyes import *
import time
from display_fbgen import Framebuffer


start_timer_val = time.perf_counter()  # high-precision timer 

def ticks_ms():
	return (time.perf_counter() - start_timer_val) * 1000

lcd = Framebuffer()

# Start the display
lcd.fill(1)
time.sleep(1)


fps_screen = 7

# Open the serial port (adjust as needed)
ser = serial.Serial(port='/dev/ttyS0', baudrate=9600, timeout=1)  # Use '/dev/ttyUSB0' on Linux


# RoboEyes callback event
def robo_show( roboeyes ):
	lcd.update()

# Plug RoboEyes on any FrameBuffer descendant
robo = RoboEyes( lcd, 480, 320, frame_rate=fps_screen, on_show = robo_show )

# Define some automated eyes behaviour
robo.set_auto_blinker( ON, 4, 2) # Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
robo.set_idle_mode( ON, 5, 2) # Start idle animation cycle (eyes looking in random directions) -> turn on/off, set interval between each eye repositioning in full seconds, set range for random time interval variation in full seconds

# --[ Define eye shapes, all values in pixels ]--
#robo.eyes_width(160, 140) # byte leftEye, byte rightEye
#robo.eyes_height(160, 140) # byte leftEye, byte rightEye
#robo.eyes_radius(8, 8) # byte leftEye, byte rightEye
#robo.eyes_radius(18, 18) # byte leftEye, byte rightEye (looking round when height=width=36)
#robo.eyes_spacing(50) # int space between eyes-> can also be negative

# --[ Cyclops mode ]--
# robo.cyclops = True  #  if turned on, robot has only on eye

# --[ Initial setup animation ]-- 
# Give a second to the eyes to open in their default state
start = ticks_ms()
while ticks_diff( ticks_ms(), start ) < 1000 :
	robo.update()  

# --[ Open/Close Eyes ]-- 
# Auto blinker must be disable to properly run this
#robo.close() # Close Eyes
#robo.open() # Open Eyes

# --[ Define mood, curiosity and position ]--
robo.mood = DEFAULT  # mood expressions, can be TIRED, ANGRY, HAPPY, FROZEN, AFRAID, CURIOUS, DEFAULT
#robo.position = DEFAULT # cardinal directions, can be N, NE, E, SE, S, SW, W, NW, DEFAULT (default = horizontally and vertically centered)

robo.curious = True  # bool on/off -> when turned on, height of the outer eyes increases when moving to the very left or very right

# --[ Set horizontal or vertical flickering ]--
#robo.horiz_flicker(True, 2) # bool on/off, byte amplitude -> horizontal flicker: alternately displacing the eyes in the defined amplitude in pixels
#robo.vert_flicker(True, 2) # bool on/off, byte amplitude -> vertical flicker: alternately displacing the eyes in the defined amplitude in pixels

# --[ Play prebuilt oneshot animations ]--
#robo.confuse() # confused - eyes shaking left and right
#robo.laugh() # laughing - eyes shaking up and down
#robo.wink( right=True ) # make the right Eye Winking


# Send command
ser.write(b'sm;s1:-1000;s2:-1000\n')  # Send as bytes, add newline if Arduino expects it

send_interval = 3  # seconds
last_send_time =  time.perf_counter()

try:
	while True:
		current_time = time.perf_counter()
		if current_time - last_send_time >= send_interval:
			last_send_time = current_time
			ser.reset_input_buffer()
			ser.write(b'swa;\n')
			time.sleep((1/fps_screen)/1.5)
			# Check for incoming data
			response = ser.readline().decode().strip()
			if response:
				if response.startswith("ACK;swa:"):
					value = int(response.split(":")[1])
					if (value > 9):
						robo.mood = HAPPY
						print("PAt Received,Robot happy")
					else:
						robo.mood = DEFAULT

				
		# update eyes drawings
		robo.update()  
		time.sleep((1/fps_screen)/1.5) #set refresh rate slightly below fps of screen

except KeyboardInterrupt:
	print("Keyboard interrupt caught. Exiting gracefully...")
	ser.close()
except Exception as e:
	print(f"An error occurred: {e}")
	ser.close()
