CC = gcc
CFLAGS = -O0 -Wall
OUT = .

all: $(OUT)/dbsbuild64.exe $(OUT)/dbsutil.exe $(OUT)/dbsview.exe

$(OUT)/dbsbuild64.exe: dbsbuild64.c fakedbs_common.h
	$(CC) $(CFLAGS) -o $@ dbsbuild64.c

$(OUT)/dbsutil.exe: dbsutil.c fakedbs_common.h
	$(CC) $(CFLAGS) -o $@ dbsutil.c

$(OUT)/dbsview.exe: dbsview.c
	$(CC) $(CFLAGS) -mwindows -o $@ dbsview.c

clean:
	-del /Q dbsbuild64.exe dbsutil.exe dbsview.exe
