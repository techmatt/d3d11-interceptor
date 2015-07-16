#ifndef CORE_MESH_TRIMESH_H_
#define CORE_MESH_TRIMESH_H_

namespace ml {

	template<class FloatType>
	class TriMesh
	{
	public:

		///MOVE VERTEX AND TRIANGLE BACK ONCE WE HAVE TEMPLATE ALIASING !!!! (for triangle BVH Accelerator)

		// ********************************
		// Vertex class of the Tri Mesh
		// ********************************
		template<class FloatTypeInner>
		class Vertex {
		public:
			Vertex() : position(vec3<FloatTypeInner>::origin), normal(vec3<FloatTypeInner>::origin), color(vec4<FloatTypeInner>::origin), texCoord(vec2<FloatTypeInner>::origin) { }
			Vertex(const vec3<FloatTypeInner>& _position) : position(_position) { }
			Vertex(const vec3<FloatTypeInner>& _p, const vec3<FloatTypeInner>& _n) : position(_p), normal(_n) { }
			Vertex(const vec3<FloatTypeInner>& _p, const vec3<FloatTypeInner>& _n, const vec4<FloatTypeInner>& _c, const vec2<FloatTypeInner>& _t) : position(_p), normal(_n), color(_c), texCoord(_t) { }

			Vertex operator*(FloatTypeInner t) const {
				return Vertex(position*t, normal*t, color*t, texCoord*t);
			}
			Vertex operator/(FloatTypeInner t) const {
				return Vertex(position/t, normal/t, color/t, texCoord/t);
			}
			Vertex operator+(const Vertex& other) const {
				return Vertex(position+other.position, normal+other.normal, color+other.color, texCoord+other.texCoord);
			}
			Vertex operator-(const Vertex& other) const {
				return Vertex(position-other.position, normal-other.normal, color-other.color, texCoord-other.texCoord);
			}

			void operator*=(FloatTypeInner t) {
				*this = *this * t;
			}
			void operator/=(FloatTypeInner t) const {
				*this = *this / t;
			}
			void operator+=(const Vertex& other) const {
				*this = *this + other;
			}
			void operator-=(const Vertex& other) const {
				*this = *this - other;
			}

			//
			// If you change the order of these fields, you must update the layout fields in D3D11TriMesh::layout 
			//
			vec3<FloatTypeInner> position;
			vec3<FloatTypeInner> normal;
			vec4<FloatTypeInner> color;
			vec2<FloatTypeInner> texCoord;
		private:
		};
		typedef Vertex<float> Vertexf;
		typedef Vertex<double> Vertexd;

		// ********************************
		// Triangle class of the Tri Mesh
		// ********************************
		template<class FloatTypeInner>
		class Triangle {
		public:

			Triangle(const Vertex<FloatTypeInner> *v0, const Vertex<FloatTypeInner> *v1, const Vertex<FloatTypeInner> *v2, unsigned int triIdx = 0, unsigned int meshIdx = 0) {
				assert (v0 && v1 && v2);
				this->v0 = v0;
				this->v1 = v1;
				this->v2 = v2;
				m_Center = (v0->position + v1->position + v2->position)/(FloatTypeInner)3.0;
				m_TriangleIndex = triIdx;
				m_MeshIndex = meshIdx;
			}


			Vertex<FloatTypeInner> getSurfaceVertex(FloatTypeInner u, FloatTypeInner v) const {
				return *v0 *((FloatTypeInner)1.0 - u - v) + *v1 *u + *v2 *v;
			}
			vec3<FloatTypeInner> getSurfacePosition(FloatTypeInner u, FloatTypeInner v) const 	{
				return v0->position*((FloatTypeInner)1.0 - u - v) + v1->position*u + v2->position*v;
			}
			vec4<FloatTypeInner> getSurfaceColor(FloatTypeInner u, FloatTypeInner v) const {
				return v0->color*((FloatTypeInner)1.0 - u - v) + v1->color*u + v2->color*v;
			}
			vec3<FloatTypeInner> getSurfaceNormal(FloatTypeInner u, FloatTypeInner v) const {
				return v0->normal*((FloatTypeInner)1.0 - u - v) + v1->normal*u + v2->normal*v;
			}
			vec2<FloatTypeInner> getSurfaceTexCoord(FloatTypeInner u, FloatTypeInner v) const {
				return v0->texCoord*((FloatTypeInner)1.0 - u - v) + v1->texCoord*u + v2->texCoord*v;
			}

			bool intersect(const Ray<FloatTypeInner> &r, FloatTypeInner& _t, FloatTypeInner& _u, FloatTypeInner& _v, FloatTypeInner tmin = (FloatTypeInner)0, FloatTypeInner tmax = std::numeric_limits<FloatTypeInner>::max(), bool intersectOnlyFrontFaces = false) const {
				return intersection::intersectRayTriangle(v0->position, v1->position, v2->position, r, _t, _u, _v, tmin, tmax, intersectOnlyFrontFaces);
			}

			bool intersects(const Triangle<FloatTypeInner>& other) const {
				return intersection::intersectTriangleTriangle(v0->position,v1->position,v2->position, other.v0->position,other.v1->position,other.v2->position);
			}

			void includeInBoundingBox(BoundingBox3<FloatTypeInner> &bb) const {
				bb.include(v0->position);
				bb.include(v1->position);
				bb.include(v2->position);
			}

			BoundingBox3<FloatTypeInner> getBoundingBox() const {
				BoundingBox3<FloatTypeInner> bb;
				includeInBoundingBox(bb);
				return bb;
			}

			const vec3<FloatTypeInner>& getCenter() const {
				return m_Center;
			}

			const Vertex<FloatTypeInner>& getV0() const {
				return *v0;
			}
			const Vertex<FloatTypeInner>& getV1() const {
				return *v1;
			}
			const Vertex<FloatTypeInner>& getV2() const {
				return *v2;
			}

			unsigned int getIndex() const {
				return m_TriangleIndex;
			}
			unsigned int getMeshIndex() const {
				return m_MeshIndex;
			}

		private:
			const Vertex<FloatTypeInner> *v0, *v1, *v2;			
			vec3<FloatTypeInner> m_Center;	//TODO check if we want to store the center
			unsigned int m_TriangleIndex;	//! 0-based triangle index within it's mesh 
			unsigned int m_MeshIndex;		//! 0-based mesh index; used for accelerators that take an std::vector of triMeshes 
		};
		typedef Triangle<float> Trianglef;
		typedef Triangle<double> Triangled;



		// ********************************
		// TriMesh itself
		// ********************************
		TriMesh() : m_Vertices(), m_Indices() {
			m_bHasNormals = false;
			m_bHasTexCoords = false;
			m_bHasColors = false;
		}
		TriMesh(const MeshData<FloatType>& meshData);

		TriMesh(const std::vector<Vertex<FloatType>>& vertices, const std::vector<unsigned int>& indices, const bool recomputeNormals = false,
			const bool hasNormals = false, const bool hasTexCoords = false, const bool hasColors = false) {
				if (indices.size()%3 != 0)	throw MLIB_EXCEPTION("not a tri mesh");
				m_Vertices = vertices;
				m_Indices.resize(indices.size()/3);
				memcpy(&m_Indices[0], &indices[0], indices.size()*sizeof(unsigned int));
				m_bHasNormals = hasNormals;
				m_bHasTexCoords = hasTexCoords;
				m_bHasColors = hasColors;
				if (recomputeNormals) {
					computeNormals();
				}
		}

		TriMesh(const std::vector<Vertex<FloatType>>& vertices, const std::vector<vec3ui>& indices, bool recomputeNormals = false,
			const bool hasNormals = false, const bool hasTexCoords = false, const bool hasColors = false) {
				m_Vertices = vertices;
				m_Indices = indices;
				m_bHasNormals = hasNormals;
				m_bHasTexCoords = hasTexCoords;
				m_bHasColors = hasColors;
				if (recomputeNormals) {
					computeNormals();
				}
		}

		TriMesh(
			const std::vector<vec3<FloatType>>& vertices, 
			const std::vector<unsigned int>& indices, 
			const std::vector<vec4<FloatType>>& colors,
			const std::vector<vec3<FloatType>>& normals,
			const std::vector<vec3<FloatType>>& texCoords) :
		TriMesh(vertices.size(), indices.size(), 
			&vertices[0], &indices[0],
			colors.size() > 0 ? &colors[0] : nullptr,
			normals.size() > 0 ? &normals[0] : nullptr,
			texCoords.size() > 0 ? &texCoords[0] : nullptr) 
		{
		}

		TriMesh(const std::vector<vec3<FloatType> >& vertices, const std::vector<unsigned int>& indices) : TriMesh(vertices.size(), indices.size(), &vertices[0], &indices[0]) {}

		TriMesh(
			size_t numVertices, size_t numIndices,
			const vec3<FloatType>* vertices, 
			const unsigned int* indices, 
			const vec4<FloatType>* colors = nullptr, 
			const vec3<FloatType>* normals = nullptr, 
			const vec2<FloatType>* texCoords = nullptr) 
		{
			if (numIndices % 3 != 0) throw MLIB_EXCEPTION("not a tri mesh");
			m_bHasColors = colors != nullptr;
			m_bHasNormals = normals != nullptr;
			m_bHasTexCoords = texCoords != nullptr;
			m_Vertices.resize(numVertices);
			for (size_t i = 0; i < numVertices; i++) {
				m_Vertices[i].position = vertices[i];
				if (colors) m_Vertices[i].color = colors[i];
				if (normals) m_Vertices[i].normal = normals[i];
				if (texCoords) m_Vertices[i].texCoord = texCoords[i];
			}
			m_Indices.resize(numIndices/3);
			for (size_t i = 0; i < numIndices/3; i++) {
				m_Indices[i] = vec3ui(indices[3*i+0],indices[3*i+1],indices[3*i+2]);
			}
		}

		TriMesh(const BoundingBox3<FloatType>& bbox, const vec4<FloatType>& color = vec4<FloatType>(1.0,1.0,1.0,1.0)) {
			std::vector<vec3<FloatType>> vertices;
			std::vector<vec3ui> indices;
			std::vector<vec3<FloatType>> normals;
			bbox.makeTriMesh(vertices, indices, normals);

			m_Vertices.resize(vertices.size());
			for (size_t i = 0; i < vertices.size(); i++) {
				m_Vertices[i].color = color;
				m_Vertices[i].position = vertices[i];
				m_Vertices[i].normal = normals[i];
			}
			m_Indices = indices;
			m_bHasColors = m_bHasNormals = m_bHasTexCoords = true;
		}

		TriMesh(const TriMesh& other) {
			m_Vertices = other.m_Vertices;
			m_Indices = other.m_Indices;
			m_bHasNormals = other.m_bHasNormals;
			m_bHasTexCoords = other.m_bHasTexCoords;
			m_bHasColors = other.m_bHasColors;
		}
		TriMesh(TriMesh&& t) {
			m_Vertices = std::move(t.m_Vertices);
			m_Indices = std::move(t.m_Indices);
			m_bHasNormals = t.m_bHasNormals;
			m_bHasTexCoords = t.m_bHasTexCoords;
			m_bHasColors = t.m_bHasColors;
		}


		TriMesh(const BinaryGrid3& grid, Matrix4x4<FloatType> voxelToWorld = Matrix4x4<FloatType>::identity(), bool withNormals = false, const vec4<FloatType>& color = vec4<FloatType>(0.5,0.5,0.5,0.5)) {
			for (unsigned int z = 0; z < grid.getDimZ(); z++) {
				for (unsigned int y = 0; y < grid.getDimY(); y++) {
					for (unsigned int x = 0; x < grid.getDimX(); x++) {
						if (grid.isVoxelSet(x,y,z)) {
							vec3<FloatType> p((FloatType)x,(FloatType)y,(FloatType)z);
							vec3<FloatType> pMin = p - (FloatType)0.5;
							vec3<FloatType> pMax = p + (FloatType)0.5;
							p = voxelToWorld * p;

							BoundingBox3<FloatType> bb;
							bb.include(voxelToWorld * pMin);
							bb.include(voxelToWorld * pMax);

							if (withNormals) {
								vec3<FloatType> verts[24];
								vec3ui indices[12];
								vec3<FloatType> normals[24];
								bb.makeTriMesh(verts,indices,normals);

								unsigned int vertIdxBase = (unsigned int)m_Vertices.size();
								for (unsigned int i = 0; i < 24; i++) {
									m_Vertices.push_back(Vertex<FloatType>(verts[i], normals[i]));
								}
								for (unsigned int i = 0; i < 12; i++) {
									indices[i] += vertIdxBase;
									m_Indices.push_back(indices[i]);
								}
							} else {
								vec3<FloatType> verts[8];
								vec3ui indices[12];
								bb.makeTriMesh(verts, indices);

								unsigned int vertIdxBase = (unsigned int)m_Vertices.size();
								for (unsigned int i = 0; i < 8; i++) {
									m_Vertices.push_back(Vertex<FloatType>(verts[i]));
								}
								for (unsigned int i = 0; i < 12; i++) {
									indices[i] += vertIdxBase;
									m_Indices.push_back(indices[i]);
								}
							}
						}
					}
				}
				for (unsigned int i = 0; i < m_Vertices.size(); i++) {
					m_Vertices[i].color = color;
				}
			}
		}

		~TriMesh() {
		}

		void operator=(TriMesh&& t) {
			m_Vertices = std::move(t.m_Vertices);
			m_Indices = std::move(t.m_Indices);
			m_bHasNormals = t.m_bHasNormals;
			m_bHasTexCoords = t.m_bHasTexCoords;
			m_bHasColors = t.m_bHasColors;
		}

		void operator=(const TriMesh& other) {
			m_Vertices = other.m_Vertices;
			m_Indices = other.m_Indices;
			m_bHasNormals = other.m_bHasNormals;
			m_bHasTexCoords = other.m_bHasTexCoords;
			m_bHasColors = other.m_bHasColors;
		}

		void clear() {
			m_Vertices.clear();
			m_Indices.clear();
			m_bHasNormals = false;
			m_bHasTexCoords = false;
			m_bHasColors = false;
		}

		void transform(const Matrix4x4<FloatType>& m) {
      Matrix4x4<FloatType> invTrans = m.getInverse().getTranspose();
			for (Vertex<FloatType>& v : m_Vertices) {
        v.position = m * v.position;
        v.normal = invTrans.transformNormalAffine(v.normal);
        v.normal.normalizeIfNonzero();
      }
		}

		void scale(FloatType s) { scale(vec3<FloatType>(s, s, s)); }

		void scale(const vec3<FloatType>& v) {
			for (Vertex<FloatType>& mv : m_Vertices) for (UINT i = 0; i < 3; i++) { mv.position[i] *= v[i]; }
		}

		//! overwrites/sets the mesh color
		void setColor(const vec4<FloatType>& c) {
			for (auto& v : m_Vertices) {
				v.color = c;
			}
		}

		//! Computes the bounding box of the mesh (not cached!)
		BoundingBox3<FloatType> getBoundingBox() const {
			BoundingBox3<FloatType> bb;
			for (size_t i = 0; i < m_Vertices.size(); i++) {
				bb.include(m_Vertices[i].position);
			}
			return bb;
		}

		//! Computes the vertex normals of the mesh
		void computeNormals();

        //! Creates a flat Loop-subdivision of the mesh
        TriMesh<FloatType> flatLoopSubdivision(float minEdgeLength) const;
        TriMesh<FloatType> flatLoopSubdivision(UINT iterations, float minEdgeLength) const;

        TriMesh<FloatType> flatten() const;

		const std::vector<Vertex<FloatType>>& getVertices() const { return m_Vertices; }
		const std::vector<vec3ui>& getIndices() const { return m_Indices; }

		std::vector<Vertex<FloatType>>& getVertices() { return m_Vertices; }
		std::vector<vec3ui>& getIndices() { return m_Indices; }

		void getMeshData(MeshData<FloatType>& meshData) const {

			meshData.clear();

			meshData.m_Vertices.resize(m_Vertices.size());
			meshData.m_FaceIndicesVertices.resize(m_Indices.size());
			if (m_bHasColors) {
				meshData.m_Colors.resize(m_Vertices.size());
			}
			if (m_bHasNormals)	{
				meshData.m_Normals.resize(m_Vertices.size());
			}
			if (m_bHasTexCoords) {
				meshData.m_TextureCoords.resize(m_Vertices.size());
			}
			for (size_t i = 0; i < m_Indices.size(); i++) {
				meshData.m_FaceIndicesVertices[i][0] = m_Indices[i].x;
				meshData.m_FaceIndicesVertices[i][1] = m_Indices[i].y;
				meshData.m_FaceIndicesVertices[i][2] = m_Indices[i].z;
			}

			for (size_t i = 0; i < m_Vertices.size(); i++) {
				meshData.m_Vertices[i] = m_Vertices[i].position;
				if (m_bHasColors)		meshData.m_Colors[i] = m_Vertices[i].color;
				if (m_bHasNormals)		meshData.m_Normals[i] = m_Vertices[i].normal;
				if (m_bHasTexCoords)	meshData.m_TextureCoords[i] = m_Vertices[i].texCoord;
			}
		}

		MeshData<FloatType> getMeshData() const {
			MeshData<FloatType> meshData;
			getMeshData(meshData);
			return meshData;
		}


		bool hasNormals() const {
			return m_bHasNormals;
		}
		bool hasColors() const {
			return m_bHasColors;
		}
		bool hasTexCoords() const {
			return m_bHasTexCoords;
		}
		void setHasColors(bool b) {
			m_bHasColors = b;
		}


		std::pair<BinaryGrid3, Matrix4x4<FloatType>> voxelize(FloatType voxelSize, const BoundingBox3<FloatType>& bounds = BoundingBox3<FloatType>(), bool solid = false) const {

			BoundingBox3<FloatType> bb;
			if (bounds.isInitialized()) {
				bb = bounds;
			} else {
				bb = getBoundingBox();
				bb.scale((FloatType)1 + (FloatType)3.0*voxelSize);	//safety margin
			}

			Matrix4x4<FloatType> worldToVoxel = Matrix4x4<FloatType>::scale((FloatType)1/voxelSize) * Matrix4x4<FloatType>::translation(-bb.getMin());

            std::pair<BinaryGrid3, Matrix4x4<FloatType>> gridTrans = std::make_pair(BinaryGrid3(ml::math::max(vec3ui(bb.getExtent() / voxelSize), 1U)), worldToVoxel);

			voxelize(gridTrans.first, gridTrans.second, solid);

			return gridTrans;
		}


		void voxelize(BinaryGrid3& grid, const mat4f& worldToVoxel = mat4f::identity(), bool solid = false) const {
			for (size_t i = 0; i < m_Indices.size(); i++) {
				vec3<FloatType> p0 = worldToVoxel * m_Vertices[m_Indices[i].x].position;
				vec3<FloatType> p1 = worldToVoxel * m_Vertices[m_Indices[i].y].position;
				vec3<FloatType> p2 = worldToVoxel * m_Vertices[m_Indices[i].z].position;

				BoundingBox3<FloatType> bb0(p0, p1, p2);
                //
                // TODO MATTHIAS: this + 1.0f should be investigated more.
                //
				BoundingBox3<FloatType> bb1(vec3<FloatType>(0,0,0), vec3<FloatType>((FloatType)grid.getDimX() + 1.0f, (FloatType)grid.getDimY() + 1.0f, (FloatType)grid.getDimZ() + 1.0f));
				if (bb0.intersects(bb1)) {
					voxelizeTriangle(p0, p1, p2, grid, solid);
				} else {
					std::cerr << "out of bounds: " << p0 << "\tof: " << grid.getDimensions() << std::endl;
					std::cerr << "out of bounds: " << p1 << "\tof: " << grid.getDimensions() << std::endl;
					std::cerr << "out of bounds: " << p2 << "\tof: " << grid.getDimensions() << std::endl;
					MLIB_WARNING("triangle outside of grid - ignored");
				}
			}
		}
	private:

		void voxelizeTriangle(const vec3<FloatType>& v0, const vec3<FloatType>& v1, const vec3<FloatType>& v2, BinaryGrid3& grid, bool solid = false) const {

			FloatType diagLenSq = (FloatType)3.0;
			if ((v0-v1).lengthSq() < diagLenSq && (v0-v2).lengthSq() < diagLenSq &&	(v1-v2).lengthSq() < diagLenSq) {
				BoundingBox3<FloatType> bb(v0, v1, v2);
				vec3ui minI = math::floor(bb.getMin());
				vec3ui maxI = math::ceil(bb.getMax());

				//test for accurate voxel-triangle intersections
				for (unsigned int i = minI.x; i <= maxI.x; i++) {
					for (unsigned int j = minI.y; j <= maxI.y; j++) {
						for (unsigned int k = minI.z; k <= maxI.z; k++) {
							vec3<FloatType> v((FloatType)i,(FloatType)j,(FloatType)k);
							BoundingBox3<FloatType> voxel;
							const FloatType eps = (FloatType)0.0000;
							voxel.include((v - (FloatType)0.5-eps));
							voxel.include((v + (FloatType)0.5+eps));
							if (voxel.intersects(v0, v1, v2)) {
								if (solid) {
									//project to xy-plane
									vec2<FloatType> pv = v.getVec2();
                                    if (intersection::intersectTrianglePoint(v0.getVec2(), v1.getVec2(), v2.getVec2(), pv)) {
										Ray<FloatType> r0(vec3<FloatType>(v), vec3<FloatType>(0,0,1));
										Ray<FloatType> r1(vec3<FloatType>(v), vec3<FloatType>(0,0,-1));
										FloatType t0, t1, _u0, _u1, _v0, _v1;
										bool b0 = intersection::intersectRayTriangle(v0,v1,v2,r0,t0,_u0,_v0);
										bool b1 = intersection::intersectRayTriangle(v0,v1,v2,r1,t1,_u1,_v1);
										if ((b0 && t0 <= (FloatType)0.5) || (b1 && t1 <= (FloatType)0.5)) {
											if (i < grid.getDimX() && j < grid.getDimY() && k < grid.getDimZ()) {
												grid.toggleVoxelAndBehindSlice(i, j, k);
											}
										}
										//grid.setVoxel(i,j,k);
									}
								} else {
									if (i < grid.getDimX() && j < grid.getDimY() && k < grid.getDimZ()) {
										grid.setVoxel(i, j, k);
									}
								}
							}
						}
					}
				} 
			} else {
				vec3<FloatType> e0 = (FloatType)0.5*(v0 + v1);
				vec3<FloatType> e1 = (FloatType)0.5*(v1 + v2);
				vec3<FloatType> e2 = (FloatType)0.5*(v2 + v0);
				voxelizeTriangle(v0,e0,e2, grid, solid);
				voxelizeTriangle(e0,v1,e1, grid, solid);
				voxelizeTriangle(e1,v2,e2, grid, solid);
				voxelizeTriangle(e0,e1,e2, grid, solid);
			}
		}

    // boost archive serialization
    friend class boost::serialization::access;
    template<class Archive>
    inline void serialize(Archive& ar, const unsigned int version) {
      ar & m_Vertices & m_Indices;
      if (version >= 1) {
        ar & m_bHasColors & m_bHasNormals & m_bHasTexCoords;
      }
    }

		bool m_bHasNormals;
		bool m_bHasTexCoords;
		bool m_bHasColors;

		std::vector<Vertex<FloatType>>		m_Vertices;
		std::vector<vec3ui>					m_Indices;
	};

	typedef TriMesh<float> TriMeshf;
	typedef TriMesh<double> TriMeshd;

}  // namespace ml

#include "triMesh.cpp"

#endif  // INCLUDE_CORE_MESH_TRIMESH_H_
