#!/usr/bin/env python3
import numpy as np

class Framebuffer:
	def __init__(self, width=480, height=320, fb_path='/dev/fb0'):
		self.width = width
		self.height = height
		self.channels = 4  # BGRA
		self.fb = np.zeros((height, width, self.channels), dtype=np.uint8)
		self.fb_mem = np.memmap(fb_path, dtype='uint8', mode='w+', shape=(self.height, self.width, self.channels))

	def fill(self,bgcolor ):
		self.fill_screen( bgcolor )

	def fill_screen(self, color):
		"""Fill the entire screen with a BGRA color."""
		self.fb[:] = color

	def fill_rect(self, x, y, w, h, color):
		"""Fill a rectangle starting at (x,y) with width w and height h."""
		x2 = min(self.width, x + w)
		y2 = min(self.height, y + h)
		self.fb[y:y2, x:x2] = color

	def pixel(self, x, y, color):
		"""Draw a single pixel at (x, y) with a BGRA color."""
		if 0 <= x < self.width and 0 <= y < self.height:
			self.fb[y, x] = color

	def vline(self, x, y, length, color):
		"""Draw a vertical line from (x, y) of given length and BGRA color."""
		if 0 <= x < self.fb.shape[1]:
			y_end = min(y + length, self.fb.shape[0])
			y_start = max(y, 0)
			if y_start < y_end:
				self.fb[y_start:y_end, x] = color

	def hline(self, x, y, length, color):
		"""Draw a horizontal line from (x, y) of given length and BGRA color."""
		if 0 <= y < self.fb.shape[0]:
			x_end = min(x + length, self.fb.shape[1])
			x_start = max(x, 0)
			if x_start < x_end:
				self.fb[y, x_start:x_end] = color

	def rect(self, x, y, w, h, color):
		self.hline(x,y,w,color)
		self.hline(x,y+h,w,color)
		self.vline(x,y,h,color)
		self.vline(x+w,y,h,color)

	def clear(self):
		"""Clear the screen to black."""
		self.fill_screen([0, 0, 0, 255])

	def update(self):
		self.fb_mem[:] = self.fb[:]
		self.fb_mem.flush()  # Ensure changes are written to disk