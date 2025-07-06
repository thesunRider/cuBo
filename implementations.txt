
## Pi Zero 2 W Setup
Pi Zero 2 OS: Bullseye Lite

1. Setup mDNS on Raspberry pi

´´´bash
sudo apt update
sudo apt install avahi-daemon
sudo systemctl enable avahi-daemon
sudo systemctl start avahi-daemon
systemctl status avahi-daemon
sudo raspi-config #set Hostname here

ping yourhostname.local #should work now
´´´

2. Setting up Camera and Streaming

1. Enable camera autodetect in boot config
2. Enable Camera in raspi-config
3. Download and enable mediamtx for camera, follow https://james-batchelor.com/index.php/2023/11/10/install-mediamtx-on-raspbian-bookworm/
4. Stream accessed using ffmplay commandon network using:
´´´
ffplay -fflags nobuffer -flags low_delay -framedrop rtsp://<your-server-ip>:8554/cam
´´´
5. For autostart follow: https://medium.com/@celecavac/building-your-own-wifi-camera-with-raspberry-pi-zero-w-6d59b494e0c9

3. Setting Up MIC and Speaker Simultaneously
PI Zero has only one I2S peripheral, we are sharing the right channel for ASpeaker output and the left channel for audio in, for this we are using the ´´dtoverlay=rpigooglevoicehat-ssoundcard´´