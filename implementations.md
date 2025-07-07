
## Pi Zero 2 W Setup
Pi Zero 2 OS: Bullseye Lite

Scanning for the pi before mDns is enabled can be done using
```bash
ipconfig or ifconfig (Windows or linux)
nmap -p 22 --open -sV subnet.0/24
ping <Scanned ip>
```

1. Setup mDNS on Raspberry pi
On windows you can use: https://support.apple.com/kb/DL999 to install mDNS resolution

```bash
sudo apt update
sudo apt install avahi-daemon
sudo systemctl enable avahi-daemon
sudo systemctl start avahi-daemon
systemctl status avahi-daemon
sudo raspi-config #set Hostname here

ping yourhostname.local #should work now
```

2. Setting up Camera and Streaming
1. Enable camera autodetect in boot config
2. Enable Camera in raspi-config
3. Install tools
```bash
sudo apt install ffmpeg -y
sudo apt install libcamera-apps
sudo apt-get install tio
```

4. Stream accessed using ffmplay commandon network using:
```bash
libcamera-vid -t 0 -g 10 --bitrate 4500000 --inline --width 1920 --height 1080 --framerate 30 --rotation 180 --codec libav --libav-format h264  --av-sync 200000 -n -o  udp://(recievingcomputersip):1234
ffplay -fflags nobuffer -flags low_delay -framedrop udp://(recievingcomputersip):1234
```
5. For autostart follow: https://medium.com/@celecavac/building-your-own-wifi-camera-with-raspberry-pi-zero-w-6d59b494e0c9

3. Setting Up MIC and Speaker Simultaneously

PI Zero has only one I2S peripheral, we are sharing the right channel for ASpeaker output and the left channel for audio in, for this we are using the `dtoverlay=rpigooglevoicehat-ssoundcard`
Enable overlays;
```bash
#This is /boot/config.txt
dtparam=i2s=on
dtoverlay=googlevoicehat-soundcard
```

##  USeful tools

Tools used in setting up pi zero 2 w streaming section
```bash
1. aplay -l
2. speaker-test
3. arecord -l
4. arecord -d 5 test.wav
5. aplay test.wav
6. v4l2-ctl --list-devices #list cameras
7. libcamera-hello
8. arecord --dump-hw-params -D hw:<card_id>,<device_id>
9. ls -l /dev/serial*
10. ls -l /dev/
11. fbset -fb /dev/fb0 
12. tvservice -d edid # Retrieve EDID settings from monitor and write into a file called "edid"
13. edidparser edid # Parse the file we just created to see what the attached monitor is capable of
```

4. Setup Display
Check display Connections
Follow: https://techeonics.com/setting-up-lcd-display-with-raspberry-pi/


No need for any window manager, we can directly write to the fb as follows
```python
import numpy as np

# Map the screen as Numpy array
# N.B. Numpy stores in format HEIGHT then WIDTH, not WIDTH then HEIGHT!
# c is the number of channels, 4 because BGRA
h, w, c = 1024, 1280, 4
fb = np.memmap('/dev/fb0', dtype='uint8',mode='w+', shape=(h,w,c)) 

# Fill entire screen with blue - takes 29 ms on Raspi 4
fb[:] = [255,0,0,255]

# Fill top half with red - takes 15 ms on Raspi 4
fb[:h//2] = [0,0,255,255]

# Fill bottom right quarter with green - takes 7 ms on Raspi 4
fb[h//2:, w//2:] = [0,255,0,255] 
```

Rewriting robot eyes to fit framebuffer
