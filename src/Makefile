CC=g++
CFLAGS=-o
LDLIBS=-lzip -lcurl -lmysqlclient
libpath=-L/var/lib/jenkins/workspace/zip/build
incpath=-I/var/lib/jenkins/workspace/zip/src
outpath=/var/lib/jenkins/workspace/static-engine/build

all:CStatic-Engine

sha256.o: sha256.h	sha256.cpp

CException.o: CException.h CException.cpp

CEngineSuper.o: CEngineSuper.h CEngineSuper.cpp

CXMLParsingEngine.o : CXMLParsingEngine.h CXMLParsingEngine.cpp CXMLAnalyzeModule.h
	$(CC) -c -o $@ CXMLParsingEngine.cpp ${LDLIBS} ${libpath} ${incpath}

CXMLAnalyzeModule.o :  CXMLAnalyzeModule.cpp CXMLParsingEngine.h

CURLExtractionEngine.o: CException.h CEngineSuper.h CURLExtractionEngine.h CURLExtractionEngine.cpp
	$(CC) -c -o $@ CURLExtractionEngine.cpp ${LDLIBS} ${libpath} ${incpath}

CDownloadFromUrlEngine.o: sha256.h CException.h CEngineSuper.h CDownloadFromUrlEngine.h CDownloadFromUrlEngine.cpp

CScriptExtractionEngine.o: CException.h CEngineSuper.h CScriptExtractionEngine.h CScriptExtractionEngine.cpp
	$(CC) -c -o $@ CScriptExtractionEngine.cpp ${LDLIBS} ${libpath} ${incpath}

CScriptAnalyzeEngine.o: CException.h CEngineSuper.h CScriptAnalyzeEngine.h CScriptAnalyzeEngine.cpp

CMacroExtractionEngine.o: CException.h CEngineSuper.h CMacroExtractionEngine.h CMacroExtractionEngine.cpp
	$(CC) -c -o $@ CMacroExtractionEngine.cpp ${LDLIBS} ${libpath} ${incpath}

CNoSpear.o: CURLExtractionEngine.h CDownloadFromUrlEngine.h CScriptExtractionEngine.h CScriptAnalyzeEngine.h CMacroExtractionEngine.h CXMLParsingEngine.h CNoSpear.cpp
	$(CC) -c -o $@ CNoSpear.cpp ${LDLIBS} ${libpath} ${incpath}

CStatic-Engine: sha256.o CException.o CEngineSuper.o CXMLAnalyzeModule.o CURLExtractionEngine.o CDownloadFromUrlEngine.o CScriptExtractionEngine.o CScriptAnalyzeEngine.o CMacroExtractionEngine.o CXMLParsingEngine.o CNoSpear.o
	$(CC) $^ $(LOADLIBES) ${LDLIBS} ${libpath} ${incpath} -o ${outpath}/$@

clean:
	rm -f CStatic-Engine *.o 
	rm -f ../build/CStatic-Engine

# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp -o CURLExtractionEngine -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip
# g++ main.cpp CEngineSuper.cpp CURLExtractionEngine.cpp CDownloadFromUrlEngine.cpp -o process -I/home/arad4228/Desktop/zip/src -L/home/arad4228/Desktop/zip/build -lzip -lcurl