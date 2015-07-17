
#include "main.h"

string getFirstWord(const string &s)
{
    return util::split(s, ' ')[0];
}

string wordify(const string &s)
{
    bool spaceEmitted = false;
    string result;
    for (char c : s)
    {
        if ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '*' || c == ',' || c == '(' ||
            c == ')' || c == '_')
        {
            result += c;
            if (c == '*') result += ' ';
            spaceEmitted = false;
        }
        else
        {
            if (!spaceEmitted)
            {
                spaceEmitted = true;
                result += ' ';
            }
        }
    }
    return result;
}

void D3D11InterfaceWriter::write(const string &_name, const string &input, const string &outputCpp, const string &outputHeader)
{
    name = _name;
    auto lines = util::getFileLines(input, 3);
    
    string fullDesc;
    for (const string &s : lines)
        fullDesc += s + "\n";

    fullDesc = util::remove(fullDesc, "/* [annotation] */");
    fullDesc = util::remove(fullDesc, "_In_opt_");
    fullDesc = util::remove(fullDesc, "_Out_opt_");
    fullDesc = util::remove(fullDesc, "_In_");
    fullDesc = util::remove(fullDesc, "_Out_");
    fullDesc = util::remove(fullDesc, ") = 0;");

    auto functionDescriptions = util::split(fullDesc, "virtual ");

    functions.clear();

    for (const string &funcDesc : functionDescriptions)
    {
        auto funcParts = util::split(wordify(funcDesc), '(');
        auto header = funcParts[0];
        auto params = util::split(funcParts[1], ',');

        D3D11InterfaceFunction func;

        func.returnType = util::split(header, ' ')[0];
        func.name = util::split(header, ' ')[2];

        for (const string &p : params)
        {
            //const D3D11_SHADER_RESOURCE_VIEW_DESC * pDesc
            auto words = util::split(p, ' ');

            FunctionParameter newParam;

            for (int i = 0; i < words.size() - 1; i++)
                newParam.type += words[i] + ' ';
            newParam.name = words.back();

            func.params.push_back(newParam);
        }

        functions.push_back(func);
    }

    writeCpp(outputCpp);
    writeHeader(outputHeader);
}

void D3D11InterfaceWriter::writeCpp(const string &outputCpp)
{
    auto templateLinesA = util::getFileLines(dataDir + "d3d11InterfaceTemplate.txt");
    auto templateLinesB = util::getFileLines(dataDir + "d3d11InterfaceVoidTemplate.txt");

    ofstream cppFile(outputCpp);
    cppFile << endl << "#include \"main.h\"" << endl;
    cppFile << endl;

    for (const D3D11InterfaceFunction &func : functions)
    {
        for (const string &templateLine : func.returnType == "void" ? templateLinesB : templateLinesA)
        {
            string line = util::replace(templateLine, "#FunctionName#", func.name);
            line = util::replace(line, "#FunctionParameters#", func.getParameterList());
            line = util::replace(line, "#FunctionParameterCall#", func.getParameterCallList());
            line = util::replace(line, "#FunctionReturn#", func.returnType);
            line = util::replace(line, "#InterfaceName#", name);

            cppFile << line << endl;
        }
        cppFile << endl;
    }
}

void D3D11InterfaceWriter::writeHeader(const string &outputHeader)
{
    ofstream headerFile(outputHeader);
    headerFile << endl << "class my" << name << " : public " << "I" << name << endl;
    headerFile << "{" << endl;
    headerFile << "public:" << endl;

    headerFile << "    I" << name << " *base;" << endl << endl;

    for (const D3D11InterfaceFunction &func : functions)
    {
        headerFile << "    ";
        headerFile << func.returnType << " ";
        headerFile << func.name << "(";
        headerFile << func.getParameterList();
        headerFile << ");" << endl;
    }
    headerFile << "};" << endl;
}
