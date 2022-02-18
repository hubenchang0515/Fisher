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

// 发送图像的线程
void* sendImageThread(void* userdata)
{
	uint32_t stride = htonl(WIDTH * 4);
	uint32_t width = htonl(WIDTH);
	uint32_t height = htonl(HEIGHT);

	int fd = *(int*)(userdata);
	uint8_t* data = malloc(WIDTH * HEIGHT * 4);
	uint8_t temp = 0xff;
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	while(running)
	{
		b = g;
		g = r;
		r = temp;
		temp = b;
		for (int y = 0; y < HEIGHT; y++)
		{
			for (int x = 0; x < WIDTH; x++)
			{
				// ABGR
				data[(y*WIDTH+x)*4] = r;
				data[(y*WIDTH+x)*4+1] = g;
				data[(y*WIDTH+x)*4+2] = b;
				data[(y*WIDTH+x)*4+3] = 0xff; // alpha
			}
		}
		write(fd, &stride, 4);
		write(fd, &width, 4);
		write(fd, &height, 4);
		write(fd, data, WIDTH * HEIGHT * 4);
		sleep(1);
	}

	free(data);
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
		fprintf(stderr, "%s", strerror(errno));
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr;
    memset(&addr, 0 , sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "%s", strerror(errno));
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