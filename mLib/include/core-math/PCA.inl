
template<class T>
void PCA<T>::init(const std::vector<const T*> &points, UINT dimension)
{
    cout << "Initializing PCA, " << points.size() << << " points, " << dimension << " dimensions" << endl;
	
    const UINT n = points.size();
    DenseMatrix<T> B(dimension, n);
	
    _means.clear();
    _means.resize(dimension, (T)0.0);

    for (const T *point : points)
		for(UINT dimIndex = 0; dimIndex < dimension; dimIndex++)
		    _means[dimIndex] += point[dimIndex];
	
    for(T &x : _means)
		x /= (T)n;

	for(UINT pointIndex = 0; pointIndex < n; pointIndex++)
	{
		const T *point = points[pointIndex];
		for(UINT dimIndex = 0; dimIndex < dimension; dimIndex++)
		{
            B[dimIndex][pointIndex] = point[dimIndex] - _means[dimIndex];
		}
	}

    cout << "Building cross-correlation matrix..." << endl;
	DenseMatrix<T> C;
	DenseMatrix<T>::MultiplyMMTranspose(C, B);
	DenseMatrix<T>::MultiplyInPlace(C, T(1.0) / T(n));
    initFromCorrelationMatrix(C);
}

template<class T>
void PCA<T>::initFromCorrelationMatrix(const DenseMatrix<T> &C)
{
    const UINT dimension = C.rows();
	cout << "computing eigensystem..." << endl;
    C.EigenSystem(_Eigenvalues, _Eigenvectors);
    cout << C.EigenTest(_Eigenvalues, _Eigenvectors) << endl;

    finalizeFromEigenSystem();
}

template<class T>
void PCA<T>::finalizeFromEigenSystem()
{
    const UINT dimension = _Eigenvalues.size();
    double EigenvalueSum = 0.0;
	for(UINT dimIndex = 0; dimIndex < dimension; dimIndex++)
	{
        EigenvalueSum += Math::Max(T(0.0), _Eigenvalues[dimIndex]);
	}
	double CumulativeEnergy = 0.0;
	for(UINT dimIndex = 0; dimIndex < dimension; dimIndex++)
	{
		CumulativeEnergy += _Eigenvalues[dimIndex];
		Console::WriteLine(std::string("Energy at ") + std::string(dimIndex + 1) + std::string(" terms: ") + std::string(CumulativeEnergy / EigenvalueSum * 100.0f) + std::string("%"));
	}
}

template<class T>
UINT PCA<T>::ReducedDimension(double EnergyPercent)
{
	double EigenvalueSum = 0.0;
	for(UINT dimIndex = 0; dimIndex < _Eigenvalues.size(); dimIndex++)
	{
		EigenvalueSum += _Eigenvalues[dimIndex];
	}
	double CumulativeEnergy = 0.0;
	for(UINT dimIndex = 0; dimIndex < _Eigenvalues.size(); dimIndex++)
	{
		CumulativeEnergy += _Eigenvalues[dimIndex];
		if(CumulativeEnergy / EigenvalueSum >= EnergyPercent)
		{
			return dimIndex + 1;
		}
	}
	return _Eigenvalues.size();
}

template<class T>
void PCA<T>::Transform(std::vector<T> &Result, const std::vector<T> &Input, UINT ReducedDimension)
{
    if(Result.size() != ReducedDimension)
    {
	    Result.resize(ReducedDimension);
    }
	Transform(Result.CArray(), Input.CArray(), ReducedDimension);
}

template<class T>
void PCA<T>::InverseTransform(std::vector<T> &Result, const std::vector<T> &Input)
{
	const UINT dimension = _means.size();
	Result.resize(dimension);
	InverseTransform(Result.CArray(), Input.CArray(), Input.size());
}

template<class T>
void PCA<T>::Transform(T *Result, const T *Input, UINT ReducedDimension)
{
	const UINT dimension = _means.size();
	for(UINT Row = 0; Row < ReducedDimension; Row++)
    {
		T Total = 0.0;
		for(UINT Index = 0; Index < dimension; Index++)
		{
			Total += _Eigenvectors[Row][Index] * (Input[Index] - _means[Index]);
		}
		Result[Row] = Total;
    }
}

template<class T>
void PCA<T>::InverseTransform(T *Result, const T *Input, UINT ReducedDimension)
{
	UINT dimension = _means.size();
	for(UINT Col = 0; Col < dimension; Col++)
    {
		T Total = 0.0;
		for(UINT Index = 0; Index < ReducedDimension; Index++)
		{
			Total += _Eigenvectors[Index][Col] * Input[Index];
		}
		Result[Col] = Total + _means[Col];
    }
}

template<class T>
void PCA<T>::SaveToFile(const std::string &filename) const
{
    OutputDataStream stream;
    stream << _means << _Eigenvalues << _Eigenvectors;
    stream.SaveToFile(filename);
}

template<class T>
void PCA<T>::LoadFromFile(const std::string &filename)
{
    InputDataStream stream;
    stream.LoadFromFile(filename);
    stream >> _means >> _Eigenvalues >> _Eigenvectors;
}
