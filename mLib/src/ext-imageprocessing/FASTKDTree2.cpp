
namespace ml
{

static const UINT KDTree2MaxK = 2048;

FASTKDTree2::FASTKDTree2()
{
    _nnIdx = NULL;
    _dists = NULL;
    _queryPt = NULL;
    _dataPts = NULL;
    _kdTree = NULL;
}

FASTKDTree2::~FASTKDTree2()
{
    FreeMemory();
}

void FASTKDTree2::FreeMemory()
{
    if(_nnIdx)
    {
        delete[] _nnIdx;
        _nnIdx = NULL;
    }
    if(_dists)
    {
        delete[] _dists;
        _dists = NULL;
    }
    if(_kdTree)
    {
        delete _kdTree;
        _kdTree = NULL;
    }
    if(_queryPt)
    {
        annDeallocPt(_queryPt);
        _queryPt = NULL;
    }
    if(_dataPts)
    {
        annDeallocPts(_dataPts);
        _dataPts = NULL;
    }
}

void FASTKDTree2::BuildTree(const vector<vec2i> &points)
{
    FreeMemory();
    UINT pointCount = (UINT)points.size();
    _queryPt = annAllocPt(2); // allocate query point
    _dataPts = annAllocPts(pointCount, 2); // allocate data points
    _nnIdx = new ANNidx[KDTree2MaxK];  // allocate near neigh indices
    _dists = new ANNdist[KDTree2MaxK]; // allocate near neighbor dists
    for(UINT i = 0; i < pointCount; i++)
    {
        for(UINT elementIndex = 0; elementIndex < 2; elementIndex++)
        {
            _dataPts[i][elementIndex] = points[i][elementIndex];
        }
    }

    _kdTree = new ANNkd_tree( // build search structure
        _dataPts,    // the data points
        pointCount, // number of points
        2);         // dimension of space
}

void FASTKDTree2::KNearest(const vec2f &pos, UINT k, vector<UINT> &result, float epsilon)
{
    //Assert(k <= KDTree2MaxK, "k too large");
    for(UINT elementIndex = 0; elementIndex < 2; elementIndex++)
    {
        _queryPt[elementIndex] = pos[elementIndex];
    }
    _kdTree->annkSearch( // search
        _queryPt,        // query point
        k,                // number of near neighbors
        _nnIdx,            // nearest neighbors (returned)
        _dists,            // distance (returned)
        epsilon);        // error bound

    if(result.size() != k) result.resize(k);
    for(UINT i = 0; i < k; i++) result[i] = _nnIdx[i];
}

}