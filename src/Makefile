CC=g++
CFLAGS=-o
LDLIBS=-lzip -lcurl
libpath=-L/home/arad4228/Desktop/zip/build
incpath=-I/home/arad4228/Desktop/zip/src

all:Static-Engine

CEngineSuper.o: CEngineSuper.h CEngineSuper.cpp

CURLExtractionEngine.o: CEngineSuper.h CURLExtractionEngine.h CURLExtractionEngine.cpp
	$(CC) -c -o $@ CURLExtractionEngine.cpp ${LDLIBS} ${libpath} ${incpath}

CDownloadFromUrlEngine.o: CEngineSuper.h CDownloadFromUrlEngine.h CDownloadFromUrlEngine.cpp

main.o: CURLExtractionEngine.h CDownloadFromUrlEngine.h main.cpp
	$(CC) -c -o $@ main.cpp ${LDLIBS} ${libpath} ${incpath}

Static-Engine: CEngineSuper.o CURLExtractionEngine.o CDownloadFromUrlEngine.o main.o
	$(CC) $^ $(LOADLIBES) ${LDLIBS} ${libpath} ${incpath} -o $@

clean:
	rm -f Static-Engine *.o 

# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp -o CURLExtractionEngine -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip
# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp CDownloadFromUrlEngine.cpp -o process -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip -lcurl