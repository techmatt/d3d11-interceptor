
//
// this is for constants that are perf-relevant, like the animation window size, or those that don't change often.
// Most constants should go in learningParams.txt and learningParams.h instead
//

struct Constants
{
    static const int posePCASamples = 80000;
    static const int poseChainPCASamples = 80000;
    static const int poseChainForwardLength = 4;
    static const int poseChainForwardMaxDim = 100;

    static const int poseChainReverseLength = 3;
    static const int poseChainReverseMaxDim = 100;
    
    const float LSHpNorm = 0.0135f;
    static const int LSHminiHashCount = 30;
    static const int LSHmacroTableCount = 20;
};
