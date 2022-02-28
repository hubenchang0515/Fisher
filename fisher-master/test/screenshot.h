#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <xcb/xcb.h>
#include "image.h"

/************************************************************************
 * @brief 截图
 * @param[in] conn X连接
 * @param[in] drawable 要截图的目标，全屏截图时使用 root 窗口
 * @return RGB 格式的图像，需要 image_free
 * *********************************************************************/
image_t* screen_shot(xcb_connection_t* conn, xcb_drawable_t drawable);

#endif // SCREENSHOT_H