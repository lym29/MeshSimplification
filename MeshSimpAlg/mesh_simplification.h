#pragma once
#undef min
#undef max
#include <OpenMesh/Core/IO/MeshIO.hh> 
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Geometry/EigenVectorT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>  
#include <Eigen/Dense>

struct EigenTraits : OpenMesh::DefaultTraits {
	using Point = Eigen::Vector3f;
	using Normal = Eigen::Vector3f;
	using TexCoord2D = Eigen::Vector2f;
};
using MyMesh = OpenMesh::TriMesh_ArrayKernelT<EigenTraits>;

class MeshSimplification
{
public:
	MeshSimplification(MyMesh &mesh);
	void set_mesh(MyMesh &mesh);
	void get_mesh(MyMesh &mesh);
	void run_simp(int vert_num);
private:
	MyMesh &mesh_;
	void AssembleQuadMatrix(OpenMesh::VertexHandle v, Eigen::Matrix<float, 3, 3> &A, Eigen::Vector3f &b, float &c);
	float QuadMetricMinimum(OpenMesh::VertexHandle v1, OpenMesh::VertexHandle v2, Eigen::Vector3f &v_min);
	void FindMinCostEdge(OpenMesh::EdgeHandle &min_e, Eigen::Vector3f &new_p);
	float FaceArea(OpenMesh::FaceHandle f);
};