#ifndef XUTILS_H
#define XUTILS_H

#include <stdlib.h>
#include <assert.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xtest.h>

// 获取窗口尺寸
void get_window_size(xcb_connection_t* conn, xcb_window_t window, int16_t* width, int16_t* height);

// 获取窗口图像(RGB)
void get_window_image(xcb_connection_t* conn, xcb_window_t window, uint8_t* buff);

// 发送按键
void send_keycode(xcb_connection_t *conn, xcb_keycode_t keycode, int press);

// 发送按键
void send_keysym(xcb_connection_t *conn, xcb_keysym_t keysym, int press);

// 发送鼠标按钮
void send_button(xcb_connection_t *conn, xcb_button_t button, int press);

// 发送鼠标移动
void send_motion(xcb_connection_t *conn, int16_t x, int16_t y);

#endif