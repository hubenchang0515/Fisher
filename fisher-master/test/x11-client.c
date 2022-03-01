#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <assert.h>
#include <pthread.h>

#include <sock.h>
#include <xutils.h>
#include <compress.h>
#include <keysym.h>

// SDL EVENT TYPE
#define EV_KEY_DOWN 	768
#define EV_KEY_UP   	769
#define EV_BUTTON_DOWN	1025
#define EV_BUTTON_UP	1026
#define EV_MOUSE_MOTION 1024
#define EV_MOUSE_WHEEL	1027

static bool running = true;
xcb_connection_t* conn;
xcb_window_t root;

// 处理挂断信号
static void pipeHandle(int signum)
{
	(void)(signum);
	running = false;
}

// 发送图像的线程
void* sendImageThread(void* userdata)
{
	// 获取窗口尺寸
	int16_t width;
	int16_t height;
	get_window_size(conn, root, &width, &height);

	// 创建 RGB buffer
	void* buffer = malloc(width * height * 3);
	assert(buffer);

	// socket
	int fd = *(int*)(userdata);
	
	int frames = 0;
	time_t t = 0;
	while(running)
	{
		get_window_image(conn, root, (uint8_t*)buffer);
		void* jpeg = NULL;
		size_t len = 0;
		compress(buffer, width, height, &jpeg, &len);
		assert(jpeg);

		writeI32(fd, width);
		writeI32(fd, height);
		writeI32(fd, len);
		write(fd, jpeg, len);
		free(jpeg);
		
		frames += 1;
		if (time(NULL) > t)
		{
			printf("FPS: %d\n", frames);
			frames = 0;
			t = time(NULL);
		}
	}

	free(buffer);

	return NULL;
}

// 接收命令的线程
void* receiveCommand(void* userdata)
{
	int fd = *(int*)(userdata);

	while(running)
	{
		uint32_t type = readU32(fd);
		if (type == EV_KEY_DOWN)
		{
			int32_t scancode = readI32(fd);
			int32_t keysym = readI32(fd);
			(void)readU8(fd); // state

			uint32_t x11keysym = get_x11_keysym(scancode);
			if (x11keysym == KEY_SYM_UNKNOWN)
				send_keysym(conn, (uint32_t)keysym, 1);
			else
				send_keysym(conn, x11keysym, 1);
		}

		if (type == EV_KEY_UP)
		{
			int32_t scancode = readI32(fd);
			int32_t keysym = readI32(fd);
			(void)readU8(fd); // state

			uint32_t x11keysym = get_x11_keysym(scancode);
			if (x11keysym == KEY_SYM_UNKNOWN)
				send_keysym(conn, (uint32_t)keysym, 0);
			else
				send_keysym(conn, x11keysym, 0);
		}

		if (type == EV_MOUSE_MOTION)
		{
			int32_t x = readI32(fd);
			int32_t y = readI32(fd);
			send_motion(conn, (int16_t)x, (int16_t)y);
		}

		if (type == EV_BUTTON_DOWN)
		{
			uint8_t button = readU8(fd);
			(void)readU8(fd); // state
			(void)readI32(fd); // x
			(void)readI32(fd); // y
			send_button(conn, button, 1);
		}

		if (type == EV_BUTTON_UP)
		{
			uint8_t button = readU8(fd);
			(void)readU8(fd); // state
			(void)readI32(fd); // x
			(void)readI32(fd); // y
			send_button(conn, button, 0);
		}

		if (type == EV_MOUSE_WHEEL)
		{
			(void)readI32(fd); // x
			int32_t y = readI32(fd);
			if (y > 0)
			{
				send_button(conn, 4, 1);
				send_button(conn, 4, 0);
			}
			else
			{
				send_button(conn, 5, 1);
				send_button(conn, 5, 0);
			}
		}
		
	}
	return NULL;
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "args error\n");
		return EXIT_FAILURE;
	}
	char* ip = argv[1];
	int port = atoi(argv[2]);
	printf("connect to %s:%d\n", ip, port);

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0)
	{
		fprintf(stderr, "%s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr;
    memset(&addr, 0 , sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }

	signal(SIGPIPE, pipeHandle);

	// 建立连接
    conn = xcb_connect(NULL, NULL);
	assert(conn);

    const xcb_setup_t* setup = xcb_get_setup(conn);
	assert(setup);

	// 获取 screen
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = iter.data;
	assert(screen);

    // 获取 root 窗口
    root = screen->root;

	pthread_t tidImage;
	pthread_create(&tidImage, NULL, sendImageThread, &fd);

	pthread_t tidCmd;
	pthread_create(&tidCmd, NULL, receiveCommand, &fd);

	pthread_join(tidImage, NULL);
	pthread_join(tidCmd, NULL);

	close(fd);
	return 0;
}