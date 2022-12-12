#pragma once
#include "../DocumentParser/DocumentParser.h"

/*
 * OOXML문서의 xml.rels파일의 내부 속성을 파싱하기 위한 구조체
 * xml.rels에서 relationship에 대해 파싱한다.
*/
struct ST_RELATIONSHIP : public IFormatterObject
{
	std::tstring strId;
	std::tstring strType;
	std::tstring strTarget;
	std::tstring strTargetMode;
	
	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("Id"), strId)
			+ sPair(TEXT("Type"), strType)
			+ sPair(TEXT("Target"), strTarget)
			+ sPair(TEXT("TargetMode"), strTargetMode)
			;
	}
};

/*
 * OOXML문서의 xml.rels파일의 내부 속성을 파싱하기 위한 구조체
*/
struct ST_RELS : public IFormatterObject
{
    std::tstring strXmlns;
	std::vector<ST_RELATIONSHIP> vecRelationship;
	
	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("xmlns"), strXmlns)
			+ sPair(TEXT("Relationship"), vecRelationship)
			;
	}
};

/*
 * OOXML문서의 xml 파일을 파싱하기 위한 구조체
 * r:id가 있는 경우 무조껀 해당 경로에 xml.rels파일이 있다는
 * 점을 바탕으로 구조체를 만든다.
*/
struct ST_DOCUMENT_XML : public IFormatterObject
{
    std::tstring strXmlns;

};

// From SHACKK 
struct ST_CONTENTTYPES_DEFAULT : public IFormatterObject
{
	std::tstring strExtension;
	std::tstring strContentType;

	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("Extension"), strExtension)
			+ sPair(TEXT("ContentType"), strContentType)
			;
	}
};

struct ST_CONTENTTYPES_OVERRIDE : public IFormatterObject
{
	std::tstring strPartName;
	std::tstring strContentType;

	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("PartName"), strPartName)
			+ sPair(TEXT("ContentType"), strContentType)
			;
	}
};

// (physically locatted at)/[Content_Types].xml
struct ST_CONTENTTYPES_XML : public IFormatterObject
{
	std::tstring strXmlns;
	std::vector<ST_CONTENTTYPES_DEFAULT> vecDefault;
	std::vector<ST_CONTENTTYPES_OVERRIDE> vecOverride;

	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("xmlns"), strXmlns)
			+ sPair(TEXT("Default"), vecDefault)
			+ sPair(TEXT("Override"), vecOverride)
			;
	}
};

struct ST_OLEOBJECT : public IFormatterObject
{
	std::tstring strAutoLoad;
	std::tstring strDvAspect;
	std::tstring strId;
	std::tstring strLink;
	std::tstring strOleUpdate;
	std::tstring strProgId;
	std::tstring strShapeId;
	
	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("autoLoad"), strAutoLoad)
			+ sPair(TEXT("dvAspect"), strDvAspect)
			+ sPair(TEXT("r:id"), strId)
			+ sPair(TEXT("link"), strLink)
			+ sPair(TEXT("oleUpdate"), strOleUpdate)
			+ sPair(TEXT("progId"), strProgId)
			+ sPair(TEXT("shapeId"), strShapeId)
			;
	}
};

struct ST_EMBEDDED_OBJECT : public IFormatterObject
{
	std::vector<ST_OLEOBJECT> vecOleObject;
	
	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("oleObject"), vecOleObject)
			;
	}
};
	
// (physically locatted at)/xl/worksheets/sheet[n].xml
struct ST_WORKSHEET_XML : public IFormatterObject
{
	std::tstring strXmlns;
	std::vector<ST_EMBEDDED_OBJECT> vecEmbeddedObject;
	
	void OnSync(IFormatter& formatter)
	{
		formatter
			+ sPair(TEXT("xmlns"), strXmlns)
			+ sPair(TEXT("oleObjects"), vecEmbeddedObject)
			;
	}
};

