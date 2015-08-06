
struct LearningParams
{
    LearningParams()
    {
        memset(this, 0, sizeof(LearningParams));
    }

    void load(const ParameterFile &params)
    {
        params.readParameter("datasetDir", datasetDir);

        params.readParameter("poseClusterDistThreshold", poseClusterDistThreshold);
        params.readParameter("poseClusterSoftAssignmentThreshold", poseClusterSoftAssignmentThreshold);

        params.readParameter("requiredOverlapPercentage", requiredOverlapPercentage);
        params.readParameter("minAnimationInstances", minAnimationInstances);
        params.readParameter("minAnimationLength", minAnimationLength);
        params.readParameter("maxAnimationLength", maxAnimationLength);

        params.readParameter("maxAnimationFeatureDistSq", maxAnimationFeatureDistSq);

        params.readParameter("LSHpNorm", LSHpNorm);
        params.readParameter("LSHminiHashCount", LSHminiHashCount);
        params.readParameter("LSHmacroTableCount", LSHmacroTableCount);
    }

    string datasetDir;

    double poseClusterDistThreshold;
    double poseClusterSoftAssignmentThreshold;

    double requiredOverlapPercentage;
    int maxAnimationFeatureDistSq;
    int minAnimationInstances;
    int minAnimationLength;
    int maxAnimationLength;

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
