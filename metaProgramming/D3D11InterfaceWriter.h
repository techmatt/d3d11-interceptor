
struct FunctionParameter
{
    string type;
    string name;
};

struct D3D11InterfaceFunction
{
    string getParameterList() const
    {
        string result;
        for (int i = 0; i < params.size(); i++)
        {
            result += params[i].type + " " + params[i].name;
            if (i != params.size() - 1) result += ", ";
        }

        if (result == "void") return "";

        return result;
    }

    string getParameterCallList() const
    {
        string result;
        for (int i = 0; i < params.size(); i++)
        {
            result += params[i].name;
            if (i != params.size() - 1) result += ", ";
        }

        if (result == "void") return "";

        return result;
    }

    string name;
    string returnType;
    vector<FunctionParameter> params;
};

class D3D11InterfaceWriter
{
public:
    void write(const string &_name, const string &input, const string &outputCpp, const string &outputHeader);


private:
    void writeCpp(const string &outputCpp);
    void writeHeader(const string &outputHeader);

    string name;
    vector<D3D11InterfaceFunction> functions;
};
