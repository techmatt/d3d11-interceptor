
#include "main.h"

namespace Game
{

bbox2f ObjectInst::bbox(const SegmentDatabase &database) const
{
    bbox2i result;
    return bbox2f(origin, origin + database.getSegment(segmentHash)->dimensions);
}

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
    for (const string &line : util::getFileLinesCreate(variableSpecFile, 3))
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

    VariableSpec reward;
    reward.name = "reward";
    reward.startValue = 0;
    reward.type = VariableType::VariableInt;
    stateSpec.variables.push_back(reward);

    VariableSpec action;
    action.name = "action";
    action.startValue = 0;
    action.type = VariableType::VariableInt;
    stateSpec.variables.push_back(action);
    
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

    inst.variables["reward"] = frame.reward;
    inst.variables["action"] = frame.action;
}

void Model::readVariables(const SegmentDatabase &segments, StateInst &inst) const
{
    for (const VariableDisplay *display : displays)
    {
        inst.variables[display->variableName] = display->readVariable(segments, inst);
    }
}

void Model::advance(AppState &state, int testReplayIndex, const vector<StateInst> &states, int action, StateInst &nextInst)
{
    
    for (const auto &o : stateSpec.objects)
    {
        if (o.name == "unnamed")
            continue;

        //if (o.name != "padB")
        //    continue;

        const ObjectInst *mostRecentInst = nullptr;
        for (int frame = (int)states.size() - 1; !mostRecentInst && frame >= 0; frame--)
        {
            auto &objects = states[frame].objects;
            if (objects.count(o.name) > 0)
            {
                const auto &instances = objects.find(o.name)->second;
                if (instances.size() > 0)
                    mostRecentInst = &instances[0];
            }
        }

        HistoryMetricWeights metric;
        metric.action = 1.0f;
        metric.animation = 0.001f;
        metric.position = 0.0001f;
        ObjectSampleDataset &oDataset = *state.recallDatabase.objectSamples[o.name];
        ObjectTransition transition = oDataset.predictTransitionSingleton(state, state.replayDatabase, testReplayIndex, states, (int)states.size() - 1, action, o.name, metric);

        const StateInst &curState = states.back();
        
        if (mostRecentInst != nullptr)
        {
            if (transition.nextAlive)
            {
                ObjectInst objectInst;
                objectInst.origin = mostRecentInst->origin + transition.velocity;
                objectInst.segmentHash = transition.nextAnimation;
                nextInst.objects[o.name].push_back(objectInst);
            }
        }
    }

    nextInst.variables["action"] = action;
}

}