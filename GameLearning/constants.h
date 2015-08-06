
//
// this is for constants that are perf-relevant, like the animation window size, or those that don't change often.
// Most constants should go in learningParams.txt and learningParams.h instead
//

struct Constants
{
    static const int animationPCASamples = 10000;
    static const int animationWindowRadius = 2;
    static const int animationWindowTotalSize = (animationWindowRadius * 2 + 1);
};
