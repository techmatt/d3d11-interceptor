
namespace ml
{

class FASTKDTree2
{
public:
    FASTKDTree2();
    ~FASTKDTree2();
    void FreeMemory();

    void BuildTree(const vector<vec2i> &points);
    void KNearest(const vec2f &pos, UINT k, vector<UINT> &result, float epsilon);
    
private:
    size_t           _maxK;    // max number of results
    ANNpointArray    _dataPts; // data points
    ANNpoint         _queryPt; // query point
    ANNidxArray      _nnIdx;   // near neighbor indices
    ANNdistArray     _dists;   // near neighbor distances
    ANNkd_tree*      _kdTree;  // search structure
};

}