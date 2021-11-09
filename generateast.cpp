#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

std::wstring
trim(const std::wstring &str)
{
	size_t startIndex = str.find_first_not_of(' ');
	size_t endIndex = str.find_last_not_of(' ');
	if (endIndex != std::wstring::npos)
	{
		return str.substr(startIndex, endIndex - startIndex + 1);
	}
	else if (startIndex != std::wstring::npos)
	{
		return str.substr(startIndex);
	}
	else
	{
		return str;
	}
}

std::wstring
to_lower(const std::wstring &str)
{
	std::wstring lower = str;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower;
}

void
defineType(std::wofstream &f,
	const std::wstring &baseName,
	const std::wstring &className,
	const std::wstring &fieldList)
{
	f << std::endl;
	f << "template<class R>" << std::endl;
	f << "class " << className << " : public " <<
		baseName << "<R>" << std::endl;
	f << "{" << std::endl;
	f << "public:" << std::endl;


	// Store parameters in fields.
	std::wstring::size_type beginIndex = 0;
	std::wstring::size_type endIndex = 0;
	std::vector<std::wstring> fields;
	std::wstring separator;
	do
	{
		std::wstring field;
		endIndex = fieldList.find(L',', beginIndex);
		if (endIndex == std::wstring::npos)
		{
			field = fieldList.substr(beginIndex);
		}
		else
		{
			field = fieldList.substr(beginIndex,
				endIndex - beginIndex);
		}
		field = trim(field);

		if (!field.empty())
		{
			fields.push_back(field);
		}
		beginIndex = endIndex + 1;
	}
	while (endIndex != std::wstring::npos);

	// Constructor.
	f << "\t" << className << "(" << fieldList << ")";
	if (!fields.empty())
	{
		f << " :";
	}
	f << std::endl;

	for (std::wstring field : fields)
	{
		std::wstring::size_type index = field.find(L'*');
		if (index == std::wstring::npos)
		{
			index = field.find(L' ');
		}
		if (index != std::wstring::npos)
		{
			field = field.substr(index + 1);
		}

		std::wstring name;
		f << L"\t\t" << separator << "m_" << field << "(" << field << ")" << std::endl;
		separator = L",";
	}

	f << L"\t{" << std::endl;
	f << L"\t}" << std::endl;

	// Visitor pattern.
	f << L"\tR accept(Visitor<R> *visitor)" << std::endl;
	f << L"\t{" << std::endl;
	f << L"\t\treturn visitor->visit" << className << baseName <<
		"(this);" << std::endl;
	f << L"\t}" << std::endl;

	// Fields.
	f << std::endl;
	for (std::wstring field : fields)
	{
		std::wstring::size_type index = field.find(L'*');
		if (index == std::wstring::npos)
		{
			index = field.find(L' ');
		}
		if (index != std::wstring::npos)
		{
			field.insert(index + 1, L"m_");
		}
		f << L"\t" << field << L";" << std::endl;
	}

	f << L"};" << std::endl;
}

void
defineExprClasses(
	std::wofstream &f,
	const std::vector<std::wstring> &types)
{
	for (const auto &entry : types)
	{
		auto index = entry.find(':');
		if (index != std::string::npos)
		{
			auto typeName = entry.substr(0, index);
			typeName = trim(typeName);
			f << L"template <class R>" << std::endl;
			f << L"class " << typeName << ";" << std::endl;
		}
	}
}

void
defineVisitor(
	std::wofstream &f,
	const std::wstring &baseName,
	const std::vector<std::wstring> &types)
{

	std::wstring lower = to_lower(baseName);

	std::vector<std::wstring> typeNames;
	for (const auto &entry : types)
	{
		auto index = entry.find(':');
		if (index != std::string::npos)
		{
			auto typeName = entry.substr(0, index);
			typeName = trim(typeName);
			typeNames.push_back(typeName);
		}
	}

	f << std::endl;
	f << L"template<class R>" << std::endl;
	f << L"class Visitor" << std::endl;
	f << L"{" << std::endl;
	f << L"public:" << std::endl;

	for (const auto &typeName : typeNames)
	{
		f << L"\tvirtual R visit" << typeName << baseName <<
			"(" << typeName << "<R> *" << lower << ") = 0;" <<
		       	std::endl;
	}

	f << L"};" << std::endl;
}

void
defineAst(char *outputDir,
	const std::wstring &baseName,
	const std::vector<std::wstring> &types)
{
	std::string dir(outputDir);
	std::filesystem::path outPath(dir);
	std::wstring lower = to_lower(baseName);
	outPath.append(lower + L".h");
	std::wofstream f;
	f.open(outPath.string());
	f << L"#pragma once" << std::endl;
	f << std::endl;
	f << L"#include <memory>" << std::endl;
	f << L"#include \"token.h\"" << std::endl;
	f << std::endl;

	defineExprClasses(f, types);

	defineVisitor(f, baseName, types);

	f << std::endl;
	f << L"template<class R>" << std::endl;
	f << L"class " << baseName << std::endl;
	f << L"{" << std::endl;
	f << L"public:" << std::endl;


	// The base accept() method.
	f << L"\tvirtual R accept(Visitor<R> *visitor) = 0;" << std::endl;
	f << L"};" << std::endl;

	for (const auto &entry : types)
	{
		auto index = entry.find(':');
		if (index != std::string::npos)
		{
			auto className = entry.substr(0, index);
			className = trim(className);
			auto fields = entry.substr(index + 1);
			defineType(f, baseName, className, fields);
		}
	}
}

int
main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::wcerr << "Usage: " << argv[0] << " <output_directory>" << std::endl;
		exit(64);
	}
	char *outputDir = argv[1];

	const std::vector<std::wstring> types =
	{
		L"Binary   : std::shared_ptr<Expr<R>> left, std::shared_ptr<Token> operatorX, std::shared_ptr<Expr<R>> right",
		L"Grouping : std::shared_ptr<Expr<R>> expression",
		L"DoubleLiteral  : double value",
		L"StringLiteral  : std::wstring value",
		L"Unary    : std::shared_ptr<Token> operatorX, std::shared_ptr<Expr<R>> right"
	};
	defineAst(outputDir, L"Expr", types);
}
