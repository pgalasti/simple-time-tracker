#ifndef LINEPARSER_H
#define LINEPARSER_H

#include <list>

// Probably slow - but this won't be used for more than parsing small strings in non CPU critical conditions.
class StringParser
{
public:
    StringParser();
    StringParser(std::string stringToParse, std::string strToken);
    ~StringParser(){}

    void Parse(std::string stringToParse, std::string strToken);
    const std::string getToken() const;
    const uint32 getSize() const;
    const bool isFirst() const;
    const bool isLast() const;
    const bool getNext();
    const bool getBack();
    const bool getFirst();
    const bool getLast();
    const bool operator++(int);
    const bool operator--(int);

protected:
    std::string m_OriginalString;
    std::list<std::string> m_TokenList;
    std::list<std::string>::iterator m_TokenListItr;
    std::string m_CurrentToken;

private:
    void Init();
};

bool LoadFile(const char* pszFilePath, std::string& fileOutputStr)
{
    FILE* pFile = fopen(pszFilePath, "r+");
    if (pFile == nullptr)
        return false;

    fileOutputStr.clear();
    char szLine[1024];
    while (fgets(szLine, 1024, pFile))
        fileOutputStr += szLine;

    return true;
}

#endif // LINEPARSER_H
