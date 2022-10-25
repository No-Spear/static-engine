CC=g++
CFLAGS=-o
LDLIBS=-lzip -lcurl -lmysqlclient
libpath=-L/home/arad4228/Desktop/zip/build
incpath=-I/home/arad4228/Desktop/zip/src
outpath=/home/arad4228/Desktop/static-engine/build

all:CStatic-Engine

sha256.o: sha256.h	sha256.cpp

CEngineSuper.o: CEngineSuper.h CEngineSuper.cpp

CURLExtractionEngine.o: CEngineSuper.h CURLExtractionEngine.h CURLExtractionEngine.cpp
	$(CC) -c -o $@ CURLExtractionEngine.cpp ${LDLIBS} ${libpath} ${incpath}

CDownloadFromUrlEngine.o: sha256.h CEngineSuper.h CDownloadFromUrlEngine.h CDownloadFromUrlEngine.cpp

CScriptExtractionEngine.o: CEngineSuper.h CScriptExtractionEngine.h CScriptExtractionEngine.cpp

CScriptAnalyzeEngine.o: CEngineSuper.h CScriptAnalyzeEngine.h CScriptAnalyzeEngine.cpp

CNoSpear.o: CURLExtractionEngine.h CDownloadFromUrlEngine.h CScriptExtractionEngine.h CScriptAnalyzeEngine.h CNoSpear.cpp
	$(CC) -c -o $@ CNoSpear.cpp ${LDLIBS} ${libpath} ${incpath}

CStatic-Engine: sha256.o CEngineSuper.o CURLExtractionEngine.o CDownloadFromUrlEngine.o CScriptExtractionEngine.o CScriptAnalyzeEngine.o CNoSpear.o
	$(CC) $^ $(LOADLIBES) ${LDLIBS} ${libpath} ${incpath} -o ${outpath}/$@

clean:
	rm -f CStatic-Engine *.o 
	rm -f ../build/CStatic-Engine

# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp -o CURLExtractionEngine -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip
# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp CDownloadFromUrlEngine.cpp -o process -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip -lcurl