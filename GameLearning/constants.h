
//
// this is for constants that are perf-relevant, like the animation window size, or those that don't change often.
// Most constants should go in learningParams.txt and learningParams.h instead
//

struct Constants
{
    static const int posePCASamples = 40000;
    static const int poseChainPCASamples = 40000;
    static const int poseChainForwardLength = 8;
    static const int poseChainForwardMaxDim = 20;

    static const int poseChainReverseLength = 3;
    static const int poseChainReverseMaxDim = 40;
    
    const float LSHpNorm = 0.0135f;
    static const int LSHminiHashCount = 30;
    static const int LSHmacroTableCount = 20;
};
