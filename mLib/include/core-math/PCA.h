template <class T>
class PCA
{
public:
    void init(const std::vector<const T*> &points, UINT dimension);
    
    void save(const std::string &filename) const;
    void load(const std::string &filename);

    UINT reducedDimension(double energyPercent);
	
    void transform(const std::vector<T> &input, UINT reducedDimension, std::vector<T> &result);
    void inverseTransform(const std::vector<T> &input, std::vector<T> &result);
	
    void transform(const T *input, UINT reducedDimension, T *result);
    void inverseTransform(const T *input, UINT reducedDimension, T *result);

private:
    void initFromCorrelationMatrix(const DenseMatrix<T> &m);
    void finalizeFromEigenSystem();

	std::vector<T> _means;
    std::vector<T> _eigenvalues;
	DenseMatrix<T> _eigenvectors;
};

#include "PCA.inl"
