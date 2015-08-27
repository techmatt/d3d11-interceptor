
struct LearningParams
{
    LearningParams()
    {
        memset(this, 0, sizeof(LearningParams));
    }

    void load(const ParameterFile &params)
    {
        params.readParameter("datasetDir", datasetDir);

        params.readParameter("requiredOverlapPercentage", requiredOverlapPercentage);
        params.readParameter("maxAnimationDropOffPercentage", maxAnimationDropOffPercentage);
        params.readParameter("minAnimationInstances", minAnimationInstances);
        params.readParameter("minAnimationLength", minAnimationLength);
        params.readParameter("maxAnimationLength", maxAnimationLength);

        params.readParameter("animationDistSqBase", animationDistSqBase);

        params.readParameter("poseDistSqThreshold", poseDistSqThreshold);
        params.readParameter("maxPoseDistSq", maxPoseDistSq);

        params.readParameter("LSHpNorm", LSHpNorm);
        params.readParameter("LSHminiHashCount", LSHminiHashCount);
        params.readParameter("LSHmacroTableCount", LSHmacroTableCount);

        params.readParameter("PCAEnergy", PCAEnergy);
    }

    string datasetDir;

    double PCAEnergy;

    double requiredOverlapPercentage;
    double maxAnimationDropOffPercentage;
    int minAnimationInstances;
    int minAnimationLength;
    int maxAnimationLength;

    double animationDistSqBase;

    double poseDistSqThreshold;
    double maxPoseDistSq;

    double LSHpNorm;
    int LSHminiHashCount;
    int LSHmacroTableCount;
};

extern LearningParams g_learningParams;

inline const LearningParams& learningParams()
{
    return g_learningParams;
}

inline LearningParams& mutableLearningParams()
{
    return g_learningParams;
}

inline void initLearningParams(const ParameterFile &params)
{
    g_learningParams.load(params);
}
