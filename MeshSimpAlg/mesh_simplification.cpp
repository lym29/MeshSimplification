#include "mesh_simplification.h"

MeshSimplification::MeshSimplification(MyMesh &mesh) : mesh_(mesh){}

void MeshSimplification::set_mesh(MyMesh &mesh)
{
	mesh_ = mesh;
}

void MeshSimplification::get_mesh(MyMesh &mesh)
{
	mesh.assign(mesh_);
}

float MeshSimplification::FaceArea(OpenMesh::FaceHandle f)
{
	Eigen::Vector3f p1, p2, p3;
	MyMesh::ConstFaceVertexIter cfv_iter = mesh_.cfv_iter(f);
	p1 = mesh_.point(*cfv_iter);
	p2 = mesh_.point(*(++cfv_iter));
	p3 = mesh_.point(*(++cfv_iter));
	float area = 0.5 * (p2 - p1).cross(p3 - p1).norm();

	return area;
}

void MeshSimplification::AssembleQuadMatrix(OpenMesh::VertexHandle v, Eigen::Matrix<float, 3, 3> &A, Eigen::Vector3f &b, float &c)
{
	for (auto vf : mesh_.vf_range(v))
	{
		Eigen::Vector3f fn = mesh_.calc_face_normal(vf);
		Eigen::Vector3f p = mesh_.point(v);
		float area = FaceArea(vf);
		A += area * fn * fn.transpose();
		b += area * fn.dot(p) * fn;
		c += area * pow(fn.dot(p), 2);
	}
}

float MeshSimplification::QuadMetricMinimum(OpenMesh::VertexHandle v1, OpenMesh::VertexHandle v2, Eigen::Vector3f &v_min)
{
	Eigen::Matrix<float, 3, 3> A; A.setZero();
	Eigen::Vector3f b; b.setZero();
	float c = 0;
	AssembleQuadMatrix(v1, A, b, c);
	AssembleQuadMatrix(v2, A, b, c);
	v_min = A.ldlt().solve(b);
	float cost = v_min.transpose() * A * v_min - 2 * v_min.dot(b) + c;

	return cost;
}

void MeshSimplification::FindMinCostEdge(OpenMesh::EdgeHandle &min_e, Eigen::Vector3f &new_p)
{
	float min_cost = FLT_MAX;
	for (auto e : mesh_.edges())
	{
		OpenMesh::HalfedgeHandle he = mesh_.halfedge_handle(e, 0);
		OpenMesh::VertexHandle v1 = mesh_.from_vertex_handle(he);
		OpenMesh::VertexHandle v2 = mesh_.to_vertex_handle(he);
		if (mesh_.is_boundary(v1) || mesh_.is_boundary(v2))
			continue;
		Eigen::Vector3f p;
		float cost = QuadMetricMinimum(v1, v2, p);
		if (cost < min_cost)
		{
			min_cost = cost;
			min_e = e;
			new_p = p;
		}
	}
}

void MeshSimplification::run_simp(int target_num)
{
	OpenMesh::EdgeHandle min_e;
	Eigen::Vector3f new_p;
	mesh_.request_halfedge_status();
	mesh_.request_edge_status();
	mesh_.request_vertex_status();
	mesh_.request_face_status();
	int counter = mesh_.n_vertices();
	while (counter > target_num)
	{
		FindMinCostEdge(min_e, new_p);
		OpenMesh::HalfedgeHandle he = mesh_.halfedge_handle(min_e, 0);
		OpenMesh::VertexHandle new_v = mesh_.to_vertex_handle(he);
		mesh_.set_point(new_v, new_p);
		if (mesh_.is_collapse_ok(he))
		{
			mesh_.collapse(he);
			counter--;
		}
	}
	mesh_.garbage_collection();
}