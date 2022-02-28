#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <unistd.h>  
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include <jpeglib.h>

#include <screenshot.h>

#define WIDTH 432
#define HEIGHT 960
#define CMD_SIZE 128

bool running = true;

// 处理挂断信号
void pipeHandle(int signum)
{
	(void)(signum);
	running = false;
}

// 压缩图像
void compress(const void* rgb, int width, int height, void** jpeg, size_t* outSize)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	jpeg_mem_dest(&cinfo, (unsigned char**)jpeg, outSize);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	size_t rowSize = cinfo.input_components * cinfo.image_width;
	JSAMPARRAY buffer = cinfo.mem->alloc_sarray((j_common_ptr)(&cinfo), JPOOL_IMAGE, rowSize, 1);

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 100, true);

	jpeg_start_compress(&cinfo, true);

	for(uint8_t* ptr = (uint8_t*)(rgb); cinfo.next_scanline < cinfo.image_height; ptr += rowSize)
    {
		memcpy(buffer[0], ptr, rowSize);
        jpeg_write_scanlines(&cinfo, buffer, 1);
    }

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
}

// 发送图像的线程
void* sendImageThread(void* userdata)
{
	// 建立连接
    xcb_connection_t* conn = xcb_connect(NULL, NULL);

    // 获取 screen
    const xcb_setup_t* setup = xcb_get_setup(conn);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t* screen = iter.data;

    // 获取 root 窗口
    xcb_window_t root = screen->root;

	// socket
	int fd = *(int*)(userdata);
	while(running)
	{
		image_t* image = screen_shot(conn, root);
		void* jpeg = NULL;
		size_t len = 0;
		compress(image->data, image->width, image->height, &jpeg, &len);
		int32_t width = htonl(image->width);
		int32_t height = htonl(image->height);
		int32_t size = htonl((uint32_t)len);
		write(fd, &width, 4);
		write(fd, &height, 4);
		write(fd, &size, 4);
		write(fd, jpeg, len);
		free(jpeg);
		image_free(image);
		usleep(1000000 / 24);
	}

	return NULL;
}

// 接收命令的线程
void* receiveCommand(void* userdata)
{
	int fd = *(int*)(userdata);
	char cmd[CMD_SIZE];
	uint32_t count;

	while(running)
	{
		// 读取数据长度
		ssize_t len = recv(fd, &count, sizeof(uint32_t), MSG_WAITALL);
		if (len <= 0 )
		{
			break;
		}
		count = ntohl(count);
		if (count > CMD_SIZE)
		{
			fprintf(stderr, "%u too large\n", count);
		}

		len = recv(fd, cmd, count, MSG_WAITALL);
		if (len <= 0 )
		{
			break;
		}

		printf("%s\n", cmd);
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

	pthread_t tidImage;
	pthread_create(&tidImage, NULL, sendImageThread, &fd);

	pthread_t tidCmd;
	pthread_create(&tidCmd, NULL, receiveCommand, &fd);

	pthread_join(tidImage, NULL);
	pthread_join(tidCmd, NULL);

	close(fd);
	return 0;
}