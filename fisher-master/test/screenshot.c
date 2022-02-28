#include "screenshot.h"
#include <stdlib.h>

/************************************************************************
 * @brief 将 BGRX 格式的数据转换为 RGB 格式
 * @param[in] in 输入数据，应当为 BGRX 格式的像素数据
 * @param[in] width 图像宽度
 * @param[in] height 图像高度
 * @param[in] out 输出数据，为 RGB 格式，需要预先分配相应的内存
 * *********************************************************************/
static void BGRX2RGB(const uint8_t* in, int16_t width, int32_t height, uint8_t* out)
{
    for (int16_t y = 0; y < height; y++)
    {
        for(int16_t x = 0; x < width; x++)
        {
            out[(y*width+x)*3] = in[(y*width+x)*4 + 2];
            out[(y*width+x)*3 + 1] = in[(y*width+x)*4 + 1];
            out[(y*width+x)*3 + 2] = in[(y*width+x)*4];
        }
    }
}

/************************************************************************
 * @brief 截图
 * @param[in] conn X连接
 * @param[in] drawable 要截图的目标，全屏截图时使用 root 窗口
 * @return RGB 格式的图像，需要 image_free
 * *********************************************************************/
image_t* screen_shot(xcb_connection_t* conn, xcb_drawable_t drawable)
{
    int16_t width = 0;
    int16_t height = 0;
    {
        xcb_get_geometry_cookie_t cookie = xcb_get_geometry(conn, drawable);
        xcb_get_geometry_reply_t* reply = xcb_get_geometry_reply(conn, cookie, NULL);
        width = reply->width;
        height = reply->height;
        free(reply);
    }

    image_t* image = image_alloc(width, height);
    {
        xcb_get_image_cookie_t cookie = xcb_get_image(conn, XCB_IMAGE_FORMAT_Z_PIXMAP, drawable, 0, 0, width, height, UINT32_MAX);
        xcb_get_image_reply_t* reply = xcb_get_image_reply(conn, cookie, NULL);
        BGRX2RGB(xcb_get_image_data(reply), width, height, image->data);
        free(reply);
    }

    return image;
}