
struct DLLExportFunction
{
    string getParameterList() const
    {
        string result;
        for (int i = 0; i < parameterTypes.size(); i++)
        {
            result += parameterTypes[i] + " p" + to_string(i);
            if (i != parameterTypes.size() - 1) result += ", ";
        }
        return result;
    }
    string getParameterCallList() const
    {
        string result;
        for (int i = 0; i < parameterTypes.size(); i++)
        {
            result += "p" + to_string(i);
            if (i != parameterTypes.size() - 1) result += ", ";
        }
        return result;
    }

    string name;
    vector<string> parameterTypes;
};

class DLLExportWriter
{
public:
    void write(const string &input, const string &outputCpp, const string &outputDef);


private:
    vector<DLLExportFunction> functions;
};