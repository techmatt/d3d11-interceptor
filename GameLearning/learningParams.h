
struct LearningParams
{
    LearningParams()
    {
        memset(this, 0, sizeof(LearningParams));
    }

    void load(const ParameterFile &params)
    {
        params.readParameter("datasetDir", datasetDir);

        params.readParameter("maxReplayCount", maxReplayCount);

        params.readParameter("PCAEnergy", PCAEnergy);

        params.readParameter("controllerHistorySize", controllerHistorySize);
        params.readParameter("velocityHistorySize", velocityHistorySize);
        params.readParameter("poseHistorySize", poseHistorySize);

        params.readParameter("minAnimationInstances", minAnimationInstances);
        
        params.readParameter("poseChainDistSq", poseChainDistSq);

        params.readParameter("poseDistSqThreshold", poseDistSqThreshold);
        params.readParameter("predictionReverseChainDistSq", predictionReverseChainDistSq);

        params.readParameter("LSHpNorm", LSHpNorm);
        params.readParameter("LSHminiHashCount", LSHminiHashCount);
        params.readParameter("LSHmacroTableCount", LSHmacroTableCount);

        params.readParameter("minObjectIndexCount", minObjectIndexCount);
    }

    string datasetDir;

    int maxReplayCount;

    double PCAEnergy;

    int minAnimationInstances;

    int velocityHistorySize;
    int controllerHistorySize;
    int poseHistorySize;

    float poseChainDistSq;

    float predictionReverseChainDistSq;

    float poseDistSqThreshold;

    double LSHpNorm;
    int LSHminiHashCount;
    int LSHmacroTableCount;

    int minObjectIndexCount;
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
