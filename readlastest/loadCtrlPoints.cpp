
#ifndef NOMINMAX
#define  NOMINMAX
#endif

#pragma warning(disable: 4251)
#pragma warning(disable: 4267)
#pragma warning(disable: 4244)
#pragma warning(disable: 4819)

//vacuate the las point cloud  to a small one
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <liblas/liblas.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <Eigen/Core>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/registration/transformation_estimation_svd.h>
#include "basicLibs.h"
#include "Utility.h"

#ifdef _DEBUG
#pragma comment(lib,"pcl_common_debug.lib")
#pragma comment(lib,"pcl_filters_debug.lib")
#pragma comment(lib,"liblas.lib")
#else
#pragma comment(lib,"pcl_common_release.lib")
#pragma comment(lib,"pcl_filters_release.lib")
#pragma comment(lib,"liblas.lib")
#endif // _DEBUG

bool saveLAS2 (const std::string& filepath, 
	const pcl::PointCloud<pcl::PointXYZ>::Ptr& cloud,
	const Utility::Offset& offset);

int main () {

	std::string ctrlpointspath = "E:/codefiles/PG-Code/small_pointcloud/control_points.txt";
	if (!Utility::file_exist (ctrlpointspath))
	{
		std::cout << "Absolute transformation Stage is failed\n";
		std::cout << ctrlpointspath << " doesn't exist!\n";
		system ("pause");
		return 0;
	}

	pcl::PointCloud<pcl::PointXYZ>::Ptr ctrl_pts_cloud (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr homo_ctrl_pts_cloud (new pcl::PointCloud<pcl::PointXYZ>);

	std::ifstream ctrl_pts_file;
	ctrl_pts_file.open (ctrlpointspath);
	while (!ctrl_pts_file.eof ())
	{
		//load ctrl pts
		pcl::PointXYZ ctrl_pt (0.0, 0.0, 0.0), homo_ctrl_pts (0.0, 0.0, 0.0);
		std::string dummy;
		float dummy_int = 0.0;
		int num_pt;
		ctrl_pts_file >> dummy >> ctrl_pt.x >> ctrl_pt.y >> ctrl_pt.z >> num_pt;
		//load homoym_pts
		ctrl_pts_file >> homo_ctrl_pts.x >> homo_ctrl_pts.y >> homo_ctrl_pts.z >> dummy_int;

		if (ctrl_pt.x == 0.0 || homo_ctrl_pts.x == 0.0) break;
		ctrl_pts_cloud->push_back (ctrl_pt);
		homo_ctrl_pts_cloud->push_back (homo_ctrl_pts);
	}
	ctrl_pts_file.close ();

	pcl::Correspondences  correspondences;
	pcl::Correspondence   correspondence;
	for (int i = 0; i < ctrl_pts_cloud->points.size (); i++)
	{
		correspondence.index_match = i;
		correspondence.index_query = i;
		correspondences.push_back (correspondence);
	}

	//根据对应关系correspondences计算旋转平移矩阵;
	pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ> trans_est;
	Eigen::Matrix4f src2tar = Eigen::Matrix4f::Identity ();
	trans_est.estimateRigidTransformation (*homo_ctrl_pts_cloud, *ctrl_pts_cloud, correspondences, src2tar);

	std::string outpath = Utility::get_parent (ctrlpointspath);
	saveLAS2 (outpath + "/ctrlpts.las", ctrl_pts_cloud,
		Utility::Offset (ctrl_pts_cloud->points [0].x, ctrl_pts_cloud->points [0].y, ctrl_pts_cloud->points [0].z));

	std::cout<< "Absolute transform matrix: \n" << src2tar;

	system ("pause");
	return 1;
}

bool saveLAS2 (const std::string& filepath, const pcl::PointCloud<pcl::PointXYZ>::Ptr& cloud,
	const Utility::Offset& offset)
{
	if (cloud == nullptr)
	{
		std::cout << "Pointer 'cloud' is nullptr!";
		return false;
	}

	if (cloud->empty ())
	{
		std::cout << "Point cloud is empty!";
		return false;
	}

	std::ofstream ofs;
	ofs.open (filepath, std::ios::out | std::ios::binary);
	ofs.setf (std::ios::fixed, std::ios::floatfield);
	ofs.precision (6);

	if (ofs.is_open ())
	{
		liblas::Header header;
		header.SetDataFormatId (liblas::ePointFormat2);
		header.SetVersionMajor (1);
		header.SetVersionMinor (2);
		header.SetOffset (offset.x, offset.y, offset.z);
		header.SetScale (0.0001, 0.0001, 0.0001);
		header.SetPointRecordsCount (cloud->size ());

		liblas::Writer writer (ofs, header);
		liblas::Point pt (&header);

		for (int i = 0; i < cloud->size (); ++i)
		{
			double x = static_cast<double>(cloud->points [i].x) + offset.x;
			double y = static_cast<double>(cloud->points [i].y) + offset.y;
			double z = static_cast<double>(cloud->points [i].z) + offset.z;

			pt.SetCoordinates (x, y, z);
			pt.SetIntensity (10);

			writer.WritePoint (pt);
		}
		ofs.flush ();
		ofs.close ();
	}

	std::cout << "Save file: " << filepath;
	return true;
}