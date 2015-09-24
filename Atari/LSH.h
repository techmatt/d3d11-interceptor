
#include <random>

//
// Locality-sensitive hashing for vectors in Euclidean space
//
template<class ValueType>
class LSHEuclidean
{
public:
    struct Params
    {
        Params(UINT _dimension, float _pNorm, UINT _miniHashFunctionCount, UINT _macroTableCount)
        {
            dimension = _dimension;
            pNorm = _pNorm;
            miniHashFunctionCount = _miniHashFunctionCount;
            macroTableCount = _macroTableCount;
        }
        // the dimensionality of the input points
        UINT dimension;

        // the expected distance between points
        //float expectedDistance;

        float pNorm;

        // the number of mini-hash functions
        UINT miniHashFunctionCount;

        // the number of tables (each composed of k mini-hash functions)
        UINT macroTableCount;
    };
    
    void init(UINT dimension, float pNorm, UINT miniHashFunctionCount, UINT macroTableCount)
    {
        _count = 0;

        Params params(dimension, pNorm, miniHashFunctionCount, macroTableCount);
        _tables.resize(params.macroTableCount);
        for (Table &t : _tables)
        {
            t.function.hashFunctions.resize(params.miniHashFunctionCount);
            for (HashFunction &h : t.function.hashFunctions)
            {
                h = HashFunction(params);
            }
        }
    }

    void insert(const vector<float> &point, ValueType value)
    {
        _count++;

        for (Table &t : _tables)
            t.insert(point, value);
    }

    set<ValueType> findSimilar(const vector<float> &point) const
    {
        set<ValueType> result;
        for (const Table &t : _tables)
        {
            t.appendSimilar(point, result);
        }
        return result;
    }

    size_t count()
    {
        return _count;
    }

private:
    

    struct HashFunction
    {
        HashFunction() {}
        HashFunction(const Params &params)
        {
            p.resize(params.dimension);
            float sum = 0.0f;
            for (auto &x : p)
            {
                x = (float)RNG::global.normal();
                sum += x * x;
            }
            const float scale = params.pNorm / sqrt(sum);
            for (auto &x : p)
                x *= scale;
            offset = util::randomUniform(0.0f, params.pNorm);
        }
        INT64 operator()(const vector<float> &point) const
        {
            float sum = 0.0f;
            const size_t n = point.size();
            for (size_t i = 0; i < n; i++)
                sum += point[i] * p[i];
            return (INT64)floor(sum + offset);
        }

        vector<float> p;
        float offset;
    };

    struct AmplifiedHashFunction
    {
        UINT64 operator()(const vector<float> &point) const
        {
            UINT64 prime = 173873417;
            UINT64 result = 0;
            for (const auto &h : hashFunctions)
            {
                INT64 value = h(point);
                result = result * prime + value;
            }
            return result;
        }

        vector<HashFunction> hashFunctions;
    };

    struct Table
    {
        void insert(const vector<float> &point, ValueType value)
        {
            UINT64 key = function(point);
            entries[key].push_back(value);
        }

        void appendSimilar(const vector<float> &point, set<ValueType> &result) const
        {
            UINT64 key = function(point);
            auto it = entries.find(key);
            if (it == entries.end()) return;
            for (ValueType value : it->second)
                result.insert(value);
        }

        unordered_map<UINT64, vector<ValueType> > entries;
        AmplifiedHashFunction function;
    };

    vector<Table> _tables;
    size_t _count;
};