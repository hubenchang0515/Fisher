# Generated by [MakeMake](https://github.com/hubenchang0515/makemake)

.PHONY: all install clean

all: fisher x11-client

install: all

clean:
	rm -f main.o Socket.o Window.o Application.o Job.o Renderer.o Texture.o QRCode.o CoopJob.o LoginJob.o
	rm -f sock.o xutils.o x11-client.o keysym.o compress.o

fisher : main.o Socket.o Window.o Application.o Job.o Renderer.o Texture.o QRCode.o CoopJob.o LoginJob.o  
	g++ -o $@ $^ -pthread -ljpeg -lqrencode -lSDL2 

main.o: ../src/main.cpp ../src/core/Application.h ../src/core/Window.h \
 ../src/core/base.h ../src/core/Renderer.h ../src/core/Job.h \
 ../src/jobs/login/LoginJob.h ../src/core/Texture.h ../src/core/QRCode.h \
 ../src/core/Socket.h ../src/jobs/cooperation/CoopJob.h
	g++ -c  ../src/main.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

Socket.o: ../src/core/Socket.cpp ../src/core/Socket.h ../src/core/base.h
	g++ -c  ../src/core/Socket.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

Window.o: ../src/core/Window.cpp ../src/core/Window.h ../src/core/base.h \
 ../src/core/Application.h ../src/core/Renderer.h ../src/core/Job.h
	g++ -c  ../src/core/Window.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

Application.o: ../src/core/Application.cpp ../src/core/Application.h \
 ../src/core/Window.h ../src/core/base.h ../src/core/Renderer.h \
 ../src/core/Job.h ../src/core/Socket.h
	g++ -c  ../src/core/Application.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

Job.o: ../src/core/Job.cpp ../src/core/Job.h
	g++ -c  ../src/core/Job.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

Renderer.o: ../src/core/Renderer.cpp ../src/core/Renderer.h \
 ../src/core/Window.h ../src/core/base.h ../src/core/Texture.h \
 ../src/core/Application.h ../src/core/Job.h
	g++ -c  ../src/core/Renderer.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

Texture.o: ../src/core/Texture.cpp ../src/core/Texture.h \
 ../src/core/Renderer.h ../src/core/Window.h ../src/core/base.h
	g++ -c  ../src/core/Texture.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

QRCode.o: ../src/core/QRCode.cpp ../src/core/QRCode.h
	g++ -c  ../src/core/QRCode.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

CoopJob.o: ../src/jobs/cooperation/CoopJob.cpp \
 ../src/jobs/cooperation/CoopJob.h ../src/core/Application.h \
 ../src/core/Window.h ../src/core/base.h ../src/core/Renderer.h \
 ../src/core/Job.h ../src/core/Texture.h ../src/core/QRCode.h \
 ../src/core/Socket.h
	g++ -c  ../src/jobs/cooperation/CoopJob.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

LoginJob.o: ../src/jobs/login/LoginJob.cpp ../src/jobs/login/LoginJob.h \
 ../src/core/Application.h ../src/core/Window.h ../src/core/base.h \
 ../src/core/Renderer.h ../src/core/Job.h ../src/core/Texture.h \
 ../src/core/QRCode.h ../src/core/Socket.h
	g++ -c  ../src/jobs/login/LoginJob.cpp -O2 -W -Wall -I../src/core -I../src/jobs/cooperation -I../src/jobs/login -g 

x11-client : sock.o xutils.o x11-client.o keysym.o compress.o  
	gcc -o $@ $^ -ljpeg -lxcb -lxcb-xtest -lxcb-keysyms -pthread -g 

sock.o: ../test/sock.c ../test/sock.h
	gcc -c ../test/sock.c -O2 -W -Wall -I../test 

xutils.o: ../test/xutils.c ../test/xutils.h
	gcc -c ../test/xutils.c -O2 -W -Wall -I../test 

x11-client.o: ../test/x11-client.c ../test/sock.h ../test/xutils.h \
 ../test/compress.h ../test/keysym.h
	gcc -c ../test/x11-client.c -O2 -W -Wall -I../test 

keysym.o: ../test/keysym.c ../test/keysym.h
	gcc -c ../test/keysym.c -O2 -W -Wall -I../test 

compress.o: ../test/compress.c ../test/compress.h
	gcc -c ../test/compress.c -O2 -W -Wall -I../test 

