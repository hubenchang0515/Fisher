#include <xutils.h>

/************************************************************************
 * @brief 将 BGRX 格式的数据转换为 RGB 格式
 * @param[in] in 输入数据，应当为 BGRX 格式的像素数据
 * @param[in] width 图像宽度
 * @param[in] height 图像高度
 * @param[in] buff 存放返回的图像数据，需要预先分配 3 * width * height 字节的内存
 * *********************************************************************/
void BGRX2RGB(const uint8_t* in, int16_t width, int32_t height, uint8_t* buff)
{
    assert(in != NULL);
    assert(buff != NULL);

    for (int16_t y = 0; y < height; y++)
    {
        for(int16_t x = 0; x < width; x++)
        {
            buff[(y*width+x)*3] = in[(y*width+x)*4 + 2];
            buff[(y*width+x)*3 + 1] = in[(y*width+x)*4 + 1];
            buff[(y*width+x)*3 + 2] = in[(y*width+x)*4];
        }
    }
}

/************************************************************************
 * @brief 获取窗口的尺寸
 * @param[in] conn X连接
 * @param[in] window 要截图的窗口
 * @param[out] width 返回窗口的宽度
 * @param[out] height 返回窗口的高度
 * *********************************************************************/
void get_window_size(xcb_connection_t* conn, xcb_window_t window, int16_t* width, int16_t* height)
{
    assert(conn != NULL);
    assert(width != NULL);
    assert(height != NULL);

    xcb_get_geometry_cookie_t cookie = xcb_get_geometry(conn, window);
    xcb_get_geometry_reply_t* reply = xcb_get_geometry_reply(conn, cookie, NULL);
    assert(reply != NULL);

    *width = reply->width;
    *height = reply->height;
    free(reply);
}

/************************************************************************
 * @brief 获取RGB格式的窗口图像
 * @param[in] conn X连接
 * @param[in] window 要截图的窗口
 * @param[in] buff 存放返回的图像数据，需要预先分配 3 * width * height 字节的内存
 * *********************************************************************/
void get_window_image(xcb_connection_t* conn, xcb_window_t window, uint8_t* buff)
{
    assert(conn != NULL);
    assert(buff != NULL);

    int16_t width = 0;
    int16_t height = 0;
    get_window_size(conn, window, &width, &height);

    xcb_get_image_cookie_t cookie = xcb_get_image(conn, XCB_IMAGE_FORMAT_Z_PIXMAP, window, 0, 0, width, height, UINT32_MAX);
    xcb_get_image_reply_t* reply = xcb_get_image_reply(conn, cookie, NULL);
    assert(reply != NULL);

    BGRX2RGB(xcb_get_image_data(reply), width, height, buff);
    free(reply);
}

/************************************************************************
 * @brief 发送按键
 * @param[in] conn xcb连接
 * @param[in] keycode 键码
 * @param[in] press 是否按下
 * *********************************************************************/
void send_keycode(xcb_connection_t *conn, xcb_keycode_t keycode, int press) 
{
    xcb_test_fake_input(conn, press ? XCB_KEY_PRESS : XCB_KEY_RELEASE, keycode, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
    xcb_flush(conn);
}


/************************************************************************
 * @brief 发送按键
 * @param[in] conn xcb连接
 * @param[in] keysym 键码
 * @param[in] press 是否按下
 * *********************************************************************/
void send_keysym(xcb_connection_t *conn, xcb_keysym_t keysym, int press) 
{
    xcb_key_symbols_t* symbols = xcb_key_symbols_alloc(conn);
    xcb_keycode_t* code = xcb_key_symbols_get_keycode(symbols, keysym);
    for (; code != NULL && *code != XCB_NO_SYMBOL; code++)
    {
        send_keycode(conn, *code, press);
    }
    xcb_key_symbols_free(symbols);
}

/************************************************************************
 * @brief 发送鼠标按钮
 * @param[in] conn xcb连接
 * @param[in] button 按钮
 * @param[in] press 是否按下
 * *********************************************************************/
void send_button(xcb_connection_t *conn, xcb_button_t button, int press)
{
    xcb_test_fake_input(conn, press ? XCB_BUTTON_PRESS : XCB_BUTTON_RELEASE, button, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
    xcb_flush(conn);
}

/************************************************************************
 * @brief 发送鼠标移动
 * @param[in] conn xcb连接
 * @param[in] button 按钮
 * @param[in] press 是否按下
 * *********************************************************************/
void send_motion(xcb_connection_t *conn, int16_t x, int16_t y)
{
    xcb_test_fake_input(conn, XCB_MOTION_NOTIFY, 0, XCB_CURRENT_TIME, XCB_NONE, x, y, 0);
    xcb_flush(conn);
}