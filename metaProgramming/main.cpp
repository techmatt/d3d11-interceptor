#include "main.h"

int main()
{
    const string outputDir = "output/";
    
    DLLExportWriter exportWriter;
    exportWriter.write(dataDir + "d3d11Exports.txt", outputDir + "d3d11Exports.cpp.txt", outputDir + "exports.def");

    D3D11InterfaceWriter interfaceWriter;

    interfaceWriter.write("D3D11Device", dataDir + "d3d11Device.txt", outputDir + "d3d11Device.cpp.txt", outputDir + "d3d11Device.h.txt");

    cout << "Press any key to continue...";
    cin.get();
    return 0;
}
