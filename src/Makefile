CC=g++
CFLAGS=-o
LDLIBS=-lzip -lcurl
libpath=-L/home/arad4228/Desktop/zip/build
incpath=-I/home/arad4228/Desktop/zip/src

all:CStatic-Engine

CEngineSuper.o: CEngineSuper.h CEngineSuper.cpp

CURLExtractionEngine.o: CEngineSuper.h CURLExtractionEngine.h CURLExtractionEngine.cpp
	$(CC) -c -o $@ CURLExtractionEngine.cpp ${LDLIBS} ${libpath} ${incpath}

CDownloadFromUrlEngine.o: CEngineSuper.h CDownloadFromUrlEngine.h CDownloadFromUrlEngine.cpp

CScriptExtractionEngine.o: CEngineSuper.h CScriptExtractionEngine.h CScriptExtractionEngine.cpp

# CScriptAnalyzeEngine.o: CEngineSuper.h CScriptAnalyzeEngine.h CScriptAnalyzeEngine.cpp

CNoSpear.o: CURLExtractionEngine.h CDownloadFromUrlEngine.h CScriptExtractionEngine.h CNoSpear.cpp
	$(CC) -c -o $@ CNoSpear.cpp ${LDLIBS} ${libpath} ${incpath}

CStatic-Engine: CEngineSuper.o CURLExtractionEngine.o CDownloadFromUrlEngine.o CScriptExtractionEngine.o CNoSpear.o
	$(CC) $^ $(LOADLIBES) ${LDLIBS} ${libpath} ${incpath} -o $@

clean:
	rm -f CStatic-Engine *.o 

# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp -o CURLExtractionEngine -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip
# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp CDownloadFromUrlEngine.cpp -o process -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip -lcurl