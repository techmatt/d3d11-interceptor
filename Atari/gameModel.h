
namespace Game
{

enum VariableType
{
    VariableInvalid,
    VariableBool,
    VariableInt,
};

//
// Specification of game variables
//
struct VariableSpec
{
    VariableSpec() { type = VariableInvalid; }

    VariableType type;
    string name;
    int startValue;
};

//
// Specification of game sprite
//
struct ObjectSpec
{
    string name;
    int objectID;
};

//
// Specification of game state
//
struct StateSpec
{
    void describe(ofstream &file) const;
    vector<VariableSpec> variables;
    vector<ObjectSpec> objects;
};

//
// Instance of game object
//
struct ObjectInst
{
    bbox2f bbox(const SegmentDatabase &database) const;
    vec2s origin;
    UINT64 segmentHash;
};

//
// Instance of game state
//
struct StateInst
{
    const vector<ObjectInst>& getInstances(const string &objectName) const
    {
        auto it = objects.find(objectName);
        if (it == objects.end())
        {
            //cout << "Object not found: " << objectName << endl;
            return emptyObjects;
        }
        return it->second;
    }

    map<string, int> variables;
    map<string, vector<ObjectInst> > objects;
    vector<ObjectInst> emptyObjects;
};

enum ConditionType
{
    ConditionBoolVariableType,
    ConditionIntVariableRangeType,
    ConditionSpriteBBoxRangeType,
};

struct Condition
{
    virtual ConditionType type() const = 0;
    virtual string toString() const = 0;
};

struct ConditionBoolVariable : public Condition
{
    ConditionType type() const { return ConditionBoolVariableType; }
    string toString() const
    {
        const string negation = negate ? "!" : "";
        return negation + variableName;
    }
    string variableName;
    bool negate;
};

struct ConditionIntVariableRange : public Condition
{
    ConditionType type() const { return ConditionBoolVariableType; }
    string toString() const
    {
        const string negation = negate ? " not between " : " between ";
        return variableName + negation + "{" + to_string(minValue) + "," + to_string(maxValue) + "}";
    }
    string variableName;
    bool negate;
    int minValue, maxValue;
};

struct ConditionSpriteBBoxRange : public Condition
{
    ConditionType type() const { return ConditionSpriteBBoxRangeType; }
    string toString() const
    {
        const string negation = negate ? " not touching " : " touching ";
        return spriteName + negation + "{" + bboxRange.getMin().toString(",") + "}-{" + bboxRange.getMax().toString(",") + "}";
    }
    string spriteName;
    bool negate;
    bbox2i bboxRange;
};

enum CommandType
{
    CommandModifyVariableType,
    CommandSetVariableIntType,
    CommandSetVariableBoolType,
};

struct Command
{
    virtual CommandType type() const = 0;
    virtual string toString() const = 0;
};

struct CommandModifyVariable : public Command
{
    CommandType type() const { return CommandModifyVariableType; }
    string toString() const
    {
        return variableName + " += " + to_string(delta);
    }
    string variableName;
    int delta;
};

struct CommandSetVariableInt : public Command
{
    CommandType type() const { return CommandSetVariableIntType; }
    string toString() const
    {
        return variableName + " = " + to_string(value);
    }
    string variableName;
    int value;
};

struct CommandSetVariableBool : public Command
{
    CommandType type() const { return CommandSetVariableBoolType; }
    string toString() const
    {
        return variableName + " = " + to_string(value);
    }
    string variableName;
    bool value;
};

struct Trigger
{
    // all conditions must be met
    vector<Condition*> conditions;

    vector<Command*> commands;
};

enum VariableDisplayType
{
    VariableDisplayCounterType,
    VariableDisplayHorizontalBarType,
};

struct VariableDisplay
{
    virtual VariableDisplayType type() const = 0;
    virtual string toString() const = 0;
    virtual int readVariable(const SegmentDatabase &segments, const StateInst &inst) const = 0;

    string variableName;
};

struct VariableDisplayCounter : public VariableDisplay
{
    VariableDisplayCounter(const string &_variableName, const string &_spriteName)
    {
        variableName = _variableName;
        spriteName = _spriteName;
    }
    VariableDisplayType type() const { return VariableDisplayCounterType; }
    string toString() const
    {
        return "Counter display for " + variableName;
    }

    int readVariable(const SegmentDatabase &segments, const StateInst &inst) const
    {
        return (int)inst.getInstances(spriteName).size();
    }

    string spriteName;
};

struct VariableDisplayHorizontalBar : public VariableDisplay
{
    VariableDisplayHorizontalBar(const string &_variableName, const string &_spriteName)
    {
        variableName = _variableName;
        spriteName = _spriteName;
    }
    VariableDisplayType type() const { return VariableDisplayHorizontalBarType; }
    string toString() const
    {
        return "Horizontal bar display for " + variableName;
    }
    int readVariable(const SegmentDatabase &segments, const StateInst &inst) const
    {
        const auto &list = inst.getInstances(spriteName);
        if (list.size() == 0)
            return 0;
        if (list.size() >= 2)
        {
            cout << "Unexpected cardinality in VariableDisplayHorizontalBar::readVariable" << endl;
            return 0;
        }
        SegmentAnimation *segment = segments.getSegment(list[0].segmentHash);
        if (segment == nullptr)
        {
            cout << "Unexpected missing segment" << endl;
            return 0;
        }
        return segment->dimensions.x;
    }

    string spriteName;
};

struct Model
{
    void initStateSpec(const ObjectAnalyzer &objectSpec, const string &variableSpecFile);

    void advance(AppState &state, int testReplayIndex, const vector<StateInst> &states, int action, StateInst &nextInst);

    void loadObjects(AppState &state, const ObjectAnalyzer &objectSpec, const ReplayFrame &frame, StateInst &inst) const;
    void readVariables(const SegmentDatabase &segments, StateInst &inst) const;



    //
    // debugging
    //
    void describeModel(const string &filename) const;

    StateSpec stateSpec;
    vector<VariableDisplay*> displays;

    vector<Trigger*> triggers;
    
};

}
