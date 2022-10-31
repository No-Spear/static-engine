#include "CEngineSuper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex> 
#include <set>
#include <map>
#include <stdexcept>
enum{
    LATEX_VOL
};



using std::string;

class CXMLAnalyzeModule
{
public:
    bool Analyze(string xmls, ST_ANALYZE_RESULT* output);

private:

};