#include "ooxml.h"

std::string parsing(std::string input)
{
    std::istringstream iss(input);
    std::string buffer;
    // vector를 통해 넣을 수 있지만 넣고 빼는 과정에 오버해드 생각해 해당 과정으로 처리
    getline(iss, buffer, '"');
    getline(iss, buffer, '"');
    // std::cout << buffer << std::endl;
    return buffer;
}

int main(int argc, char** argv)
{
    void *buf = NULL;
    std::vector<std::string> urllist;
    size_t bufsize;

    // Open file and load "xml" content to the document variable
    zip_t *zip = zip_open(argv[1], ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');

    zip_entry_open(zip, "word/_rels/document.xml.rels");
    zip_entry_read(zip, &buf, &bufsize);

    // char to string
    std::string xml((char*)buf);
    // std::cout << xml << std::endl;

    // string에서 정규 표현식을 통해 C&C URL을 찾는다.
    // 1차 작성 정규 표현식: Target="[a-zA-Z0-9!@#$%^&*/:.-]*" TargetMode="External"
    // 2차 작성 정규 표현식: Target[\s]*=[\s]*"[a-zA-Z0-9:/.!@#$%^&*()_=+{}]*"[\s]*TargetMode[\s]*=[\s]*"External"
    // 3차 작성 정규 표현식: Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#\[\]]*"[\s]*TargetMode[\s]*=[\s]*"External"
    zip_entry_close(zip);
    zip_close(zip);

    // 정규표현식 정의
    std::regex re(R"(Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#\[\]]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    // 매칭된 정규표현식 
    std::smatch match;
    while (std::regex_search(xml, match, re)) {
        // std::cout << match.str() << '\n';
        urllist.push_back(parsing(match.str()));
        xml = match.suffix();
    }
    for(int i = 0; i < urllist.size(); i++)
        std::cout << urllist[i] << std::endl;

    free(buf);
    return 0;
}