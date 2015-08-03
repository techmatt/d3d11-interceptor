
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

        params.readParameter("requiredAnimationTransitionFrames", requiredAnimationTransitionFrames);
        params.readParameter("requiredSaliency", requiredSaliency);
        params.readParameter("minAnimationLength", minAnimationLength);
    }

    string datasetDir;

    double poseClusterDistThreshold;
    double poseClusterSoftAssignmentThreshold;

    double requiredAnimationTransitionFrames;
    double requiredSaliency;
    int minAnimationLength;
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
