from display_fbgen import Framebuffer

fb = Framebuffer()
fb.fill_screen([255,255,255,255])
fb.fill_rect(10,10,100,100,[255,0,0,255])
fb.pixel(8,8,[255,0,0,255])

fb.vline(200, 200, 70, [255,0,0,255])
fb.hline(200, 200, 70, [0,0,255,255])

fb.rect( 300, 270, 90, 30, [0,255,0,255])