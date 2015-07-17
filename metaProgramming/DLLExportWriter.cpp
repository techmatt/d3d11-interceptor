
#include "main.h"

void DLLExportWriter::write(const string &input, const string &outputCpp, const string &outputDef)
{
    auto lines = util::getFileLines(input, 3);
    for (const string &s : lines)
    {
        auto parts = util::split(s, ',');

        DLLExportFunction func;
        func.name = parts[0];
        for (int i = 1; i < parts.size(); i++)
            func.parameterTypes.push_back(parts[i]);
        functions.push_back(func);
    }

    ofstream defFile(outputDef);
    defFile << "LIBRARY \"d3d11\"" << endl;
    defFile << endl;
    defFile << "EXPORTS" << endl;
    for (const DLLExportFunction &func : functions)
    {
        defFile << "    " << func.name << "=my" << func.name << endl;
    }

    auto templateLines = util::getFileLines(dataDir + "d3d11ExportsTemplate.txt");

    ofstream cppFile(outputCpp);
    cppFile << endl << "#include \"main.h\"" << endl;
    cppFile << endl;
    for (const DLLExportFunction &func : functions)
    {
        for (const string &templateLine : templateLines)
        {
            string line = util::replace(templateLine, "#FunctionName#", func.name);
            line = util::replace(line, "#FunctionParameters#", func.getParameterList());
            line = util::replace(line, "#FunctionParameterCall#", func.getParameterCallList());
            line = util::replace(line, "#FunctionReturn#", "HRESULT");
            
            cppFile << line << endl;
        }
        cppFile << endl;
    }

}
