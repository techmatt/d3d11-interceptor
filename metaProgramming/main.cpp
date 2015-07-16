#include "main.h"

int main()
{
    
    const string outputDir = "output/";
    DLLExportWriter dLLExportWriter;
    dLLExportWriter.write(dataDir + "d3d11Exports.txt", outputDir + "d3d11Exports.cpp.txt", outputDir + "exports.def");

    cout << "Press any key to continue...";
    cin.get();
    return 0;
}