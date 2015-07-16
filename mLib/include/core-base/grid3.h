
#ifndef CORE_BASE_GRID3D_H_
#define CORE_BASE_GRID3D_H_

namespace ml
{

	template <class T> class Grid3
	{
	public:
		Grid3();
		Grid3(size_t dimX, size_t dimY, size_t dimZ);
		Grid3(size_t dimX, size_t dimY, size_t dimZ, const T &value);
		Grid3(const vec3ul& dim) : Grid3(dim.x, dim.y, dim.z) {}
		Grid3(const vec3ul& dim, const T& value) : Grid3(dim.x, dim.y, dim.z, value) {}

		Grid3(const Grid3<T> &G);
		Grid3(Grid3<T> &&G);
		Grid3(size_t dimX, size_t dimY, size_t dimZ, const std::function< T(size_t x, size_t y, size_t z) > &fillFunction);

		~Grid3();

		//
		// Memory
		//
		void free();
		Grid3<T>& operator = (const Grid3<T> &G);
		Grid3<T>& operator = (Grid3<T> &&G);

		void allocate(size_t dimX, size_t dimY, size_t dimZ);
		void allocate(size_t dimX, size_t dimY, size_t dimZ, const T &value);
		void allocate(const vec3ul& dim) {						allocate(dim.x, dim.y, dim.z);				}
		void allocate(const vec3ul& dim, const T& value) {		allocate(dim.x, dim.y, dim.z, value);		}

		inline Grid3<T>& operator += (const Grid3<T> &right)
		{
			MLIB_ASSERT_STR(m_dimX == right.m_dimX && m_dimY == right.m_dimY && m_dimZ == right.m_dimZ, "grid dimensions must be equal");
			for (size_t i = 0; i < getNumElements(); i++) {
				m_data[i] += right.m_data[i];
			}
			return *this;
		}
		inline Grid3<T>& operator *= (T right)
		{
			for (size_t i = 0; i < getNumElements(); i++) {
				m_data[i] *= right.m_data[i];
			}
			return *this;
		}

		inline Grid3<T> operator * (T x)
		{
			Grid3<T> result(m_dimX, m_dimY, m_dimZ);
			for (size_t i = 0; i < getNumElements(); i++) {
				result.m_data = m_data * x;
			}
			return result;
		}

		//
		// Accessors
		//
		inline T& operator() (size_t x, size_t y, size_t z)
		{
#if defined(MLIB_BOUNDS_CHECK) || defined(_DEBUG)
			MLIB_ASSERT_STR((x < m_dimX) && (y < m_dimY) && (z < m_dimZ), "Out-of-bounds grid access");
#endif
			return m_data[z*m_dimY*m_dimX + x*m_dimY + y];
		}
		inline const T& operator() (size_t dimX, size_t dimY, size_t slice) const
		{
#if defined(MLIB_BOUNDS_CHECK) || defined(_DEBUG)
			MLIB_ASSERT_STR((dimX < m_dimX) && (dimY < m_dimY) && (slice < m_dimZ), "Out-of-bounds grid access");
#endif
			return m_data[slice*m_dimY*m_dimX + dimX*m_dimY + dimY];
		}
		inline size_t getDimX() const
		{
			return m_dimX;
		}
		inline size_t getDimY() const
		{
			return m_dimY;
		}
		inline size_t getDimZ() const
		{
			return m_dimZ;
		}

		inline vec3ul getDimensions() const {
			return vec3ul(getDimX(), getDimY(), getDimZ());
		}

		size_t getNumElements() const {
			return m_dimX * m_dimY * m_dimZ;
		}

		inline bool isSquare() const
		{
			return (m_dimX == m_dimY && m_dimY == m_dimZ);
		}
		inline T* getPointer()
		{
			return m_data;
		}
		inline const T* getPointer() const
		{
			return m_data;
		}

		//
		// Query
		//
		inline bool isValidCoordinate(size_t x, size_t y, size_t z) const
		{
			return (x < m_dimX && y < m_dimY && z < m_dimZ);
		}

		vec3ul getMaxIndex() const;
		const T& getMaxValue() const;
		vec3ul getMinIndex() const;
		const T& getMinValue() const;

		//
		// Modifiers
		//
		void setValues(const T &clearValue);

		void fill(const std::function<T(size_t x, size_t y, size_t z)> &fillFunction)
		{
			for (UINT xIndex = 0; xIndex < m_dimX; xIndex++)
				for (UINT yIndex = 0; yIndex < m_dimY; yIndex++)
					for (UINT zIndex = 0; zIndex < m_dimZ; zIndex++)
					{
						(*this)(xIndex, yIndex, zIndex) = fillFunction(xIndex, yIndex, zIndex);
					}
		}

	protected:
		T *m_data;
		size_t m_dimX, m_dimY, m_dimZ;
	};

	template <class T> inline bool operator == (const Grid3<T> &a, const Grid3<T> &b)
	{
		if (a.getDimX() != b.getDimX() || a.getDimY() != b.getDimY() || a.getDimZ() != b.getDimZ()) return false;
		const size_t totalEntries = a.getNumElements();
		for (size_t i = 0; i < totalEntries; i++) {
			if (a.ptr()[i] != b.ptr()[i])	return false;
		}
		return true;
	}

	template <class T> inline bool operator != (const Grid3<T> &a, const Grid3<T> &b)
	{
		return !(a == b);
	}

	template<class BinaryDataBuffer, class BinaryDataCompressor, class T>
	inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator<<(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, const Grid3<T>& g) {
		s << (UINT64)g.getDimX() << (UINT64)g.getDimY() << (UINT64)g.getDimZ();
		s.reserve(sizeof(T) * g.getDimX() * g.getDimY() * g.getDimZ());

		for (UINT64 z = 0; z < g.getDimZ(); z++)
			for (UINT64 y = 0; y < g.getDimY(); y++)
				for (UINT64 x = 0; x < g.getDimX(); x++)
					s << g(y, x, z);
		return s;
	}

	template<class BinaryDataBuffer, class BinaryDataCompressor, class T>
	inline BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& operator>>(BinaryDataStream<BinaryDataBuffer, BinaryDataCompressor>& s, Grid3<T>& g) {
		UINT64 dimX, dimY, dimZ;
		s >> dimX >> dimY, dimZ;
		g.allocate(dimX, dimY, dimZ);
		for (UINT64 z = 0; z < g.getDimZ(); z++)
			for (UINT64 y = 0; y < g.getDimY(); y++)
				for (UINT x = 0; x < g.getDimX(); x++)
					s << g(x, y, z);
		return s;
	}

	typedef Grid3<float> Grid3f;
	typedef Grid3<double> Grid3d;

}  // namespace ml

#include "grid3.cpp"

#endif  // CORE_BASE_GRID3D_H_
