
//
// this is for constants that are perf-relevant, like the animation window size, or those that don't change often.
// Most constants should go in learningParams.txt and learningParams.h instead
//

struct Constants
{
    static const int posePCASamples = 40000;
    static const int animationPCASamples = 40000;
    static const int animationPoseCount = 3;
    
    const float LSHpNorm = 0.0135f;
    const int LSHminiHashCount = 30;
    const int LSHmacroTableCount = 20;
};
