
#include "main.h"

namespace Game
{

void StateSpec::describe(ofstream &file) const
{
    file << "State specification" << endl;
    file << "Variables:" << endl;
    for (const VariableSpec &v : variables)
    {
        string type = "invalid";
        if (v.type == VariableBool) type = "bool ";
        if (v.type == VariableInt) type = "int ";
        file << type << v.name << " = " << v.startValue << endl;
    }

    file << endl << "Sprites:" << endl;
    for (const ObjectSpec &o : objects)
    {
        file << o.name << "," << o.objectID << endl;
    }
}

void Model::initStateSpec(const ObjectAnalyzer &objectSpec, const string &variableSpecFile)
{
    for (const string &line : util::getFileLines(variableSpecFile, 3))
    {
        const vector<string> parts = util::split(line, '\t');
        if (parts.size() == 5)
        {
            VariableSpec variable;
            variable.name = parts[0];

            if (parts[1] == "int")
                variable.type = VariableInt;
            else if (parts[1] == "bool")
                variable.type = VariableBool;

            MLIB_ASSERT_STR(variable.type != VariableInvalid, "invalid variable type");

            variable.startValue = convert::toInt(parts[2]);
            stateSpec.variables.push_back(variable);

            VariableDisplay *display = nullptr;
            
            if (parts[3] == "counter")
                display = new VariableDisplayCounter(variable.name, parts[4]);
            else if (parts[3] == "horizontalBar")
                display = new VariableDisplayHorizontalBar(variable.name, parts[4]);

            if (display != nullptr)
                displays.push_back(display);
        }
    }

    VariableSpec score;
    score.name = "score";
    score.startValue = 0;
    score.type = VariableType::VariableInt;
    stateSpec.variables.push_back(score);
    
    const int spriteCount = (int)objectSpec.objects.size();
    stateSpec.objects.resize(spriteCount);
    for (int i = 0; i < spriteCount; i++)
    {
        stateSpec.objects[i].name = objectSpec.objects[i].name;
        stateSpec.objects[i].objectID = i;
    }
}

void Model::describeModel(const string &filename) const
{
    ofstream file(filename);
    
    stateSpec.describe(file);

    file << endl << "Displays:" << endl;
    for (VariableDisplay *v : displays)
    {
        file << v->toString() << endl;
    }
}

void Model::loadObjects(AppState &state, const ObjectAnalyzer &objectSpec, const ReplayFrame &frame, StateInst &inst) const
{
    inst.objects.clear();

    for (const ObjectSpec &o : stateSpec.objects)
    {
        inst.objects[o.name] = vector<ObjectInst>();
    }

    for (const ObjectAnnotation &o : frame.objectAnnotations)
    {
        //
        // some segment configuations may have never been seen before (typically do to occlusion). ignore them.
        //
        if (o.objectID == -1)
        {
            /*const Bitmap bmp = frame.image.toBmp(state.getPalette());
            LodePNG::save(bmp, "debug.png");
            cout << "o.objectID == -1" << endl;
            cin.get();*/
            continue;
        }
        const string &objectName = objectSpec.objects[o.objectID].name;

        ObjectInst oInst;
        oInst.origin = o.origin;
        oInst.segmentHash = frame.segmentAnnotations[o.segments[0]].segmentHash;

        inst.objects[objectName].push_back(oInst);
    }

    inst.variables["score"] = frame.reward;
}

void Model::readVariables(const SegmentManager &segments, StateInst &inst) const
{
    for (const VariableDisplay *display : displays)
    {
        inst.variables[display->variableName] = display->readVariable(segments, inst);
    }
}

}