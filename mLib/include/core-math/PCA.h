namespace ml {

template <class T>
class PCA
{
public:
    void init(const std::vector<const T*> &points, size_t dimension);

    // points is a matrix with dimensions (# data points, # dimensions)
    // points will be mean-centered.
    void init(DenseMatrix<T> &points);
    
    void save(const std::string &filename) const;
    void load(const std::string &filename);

    size_t reducedDimension(double energyPercent);
	
    void transform(const std::vector<T> &input, size_t reducedDimension, std::vector<T> &result);
    void inverseTransform(const std::vector<T> &input, std::vector<T> &result);
	
    void transform(const T *input, size_t reducedDimension, T *result);
    void inverseTransform(const T *input, size_t reducedDimension, T *result);

private:
    void initFromCorrelationMatrix(const DenseMatrix<T> &m);
    void finalizeFromEigenSystem();

	std::vector<T> _means;
    EigenSystem<T> _system;
};

#include "PCA.cpp"

typedef PCA<float> PCAf;
typedef PCA<double> PCAd;

}