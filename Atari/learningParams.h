
struct LearningParams
{
    LearningParams()
    {
        memset(this, 0, sizeof(LearningParams));
    }

    void load(const ParameterFile &params)
    {
        params.readParameter("ROMDir", ROMDir);
        params.readParameter("datasetDir", datasetDir);
        
        params.readParameter("ROMName", ROMName);

        params.readParameter("objectMotionRadius", objectMotionRadius);

        ROMDatasetDir = datasetDir + ROMName + "/";
    }

    string ROMDir;
    string datasetDir;
    string ROMDatasetDir;
    
    string ROMName;

    int objectMotionRadius;
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
