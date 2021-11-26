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
	f << "class " << className << " : public " <<
		baseName << ", public std::enable_shared_from_this<" <<
		className << ">" << std::endl;
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
	f << L"\tstd::any accept(" << baseName << "Visitor *visitor)" << std::endl;
	f << L"\t{" << std::endl;
	f << L"\t\treturn visitor->visit" << className << baseName <<
		"(shared_from_this());" << std::endl;
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
	f << L"class " << baseName << "Visitor" << std::endl;
	f << L"{" << std::endl;
	f << L"public:" << std::endl;

	for (const auto &typeName : typeNames)
	{
		f << L"\tvirtual std::any visit" << typeName << baseName <<
			"(std::shared_ptr<" << typeName << "> " << lower << ") = 0;" <<
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
	f << L"#include <vector>" << std::endl;
	f << L"#include <any>" << std::endl;
	f << L"#include \"token.h\"" << std::endl;
	f << std::endl;

	defineExprClasses(f, types);

	defineVisitor(f, baseName, types);

	f << std::endl;
	f << L"class " << baseName << std::endl;
	f << L"{" << std::endl;
	f << L"public:" << std::endl;


	// The base accept() method.
	f << L"\tvirtual std::any accept(" << baseName << "Visitor *visitor) = 0;" << std::endl;
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
		L"Assign   : std::shared_ptr<Token> name, std::shared_ptr<Expr> value",
		L"Binary   : std::shared_ptr<Expr> left, std::shared_ptr<Token> operatorX, std::shared_ptr<Expr> right",
		L"Call     : std::shared_ptr<Expr> callee, std::shared_ptr<Token> paren, std::vector<std::shared_ptr<Expr>> arguments",
		L"Grouping : std::shared_ptr<Expr> expression",
		L"DoubleLiteral  : double value",
		L"StringLiteral  : std::wstring value",
		L"BooleanLiteral  : bool value",
		L"NilLiteral  :",
		L"Logical  : std::shared_ptr<Expr> left, std::shared_ptr<Token> operatorX, std::shared_ptr<Expr> right",
		L"Unary    : std::shared_ptr<Token> operatorX, std::shared_ptr<Expr> right",
		L"Variable : std::shared_ptr<Token> name"
	};
	defineAst(outputDir, L"Expr", types);

	const std::vector<std::wstring> statementTypes =
	{
		L"Block      : std::vector<std::shared_ptr<Stmt>> statements",
		L"Expression : std::shared_ptr<Expr> expression",
		L"Function   : std::shared_ptr<Token> name, std::vector<std::shared_ptr<Token>> params, std::vector<std::shared_ptr<Stmt>> body",
		L"If         : std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch",
		L"Print      : std::shared_ptr<Expr> expression",
		L"Return     : std::shared_ptr<Token> keyword, std::shared_ptr<Expr> value",
		L"Var        : std::shared_ptr<Token> name, std::shared_ptr<Expr> initializer",
		L"While      : std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body"
	};
	defineAst(outputDir, L"Stmt", statementTypes);
}
