
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
	const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud,
	const Utility::Offset& offset);

int parseLAS (std::string filename,
	const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud,
	Utility::Offset& offset);

int main () {

	std::string ctrlpointspath = "E:/codefiles/PG-Code/small_pointcloud/control_points.txt";
	if (!Utility::file_exist (ctrlpointspath))
	{
		std::cout << "Absolute transformation Stage is failed\n";
		std::cout << ctrlpointspath << " doesn't exist!\n";
		system ("pause");
		return 0;
	}

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr ctrl_pts_cloud (new pcl::PointCloud<pcl::PointXYZRGB>);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr homo_ctrl_pts_cloud (new pcl::PointCloud<pcl::PointXYZRGB>);

	std::ifstream ctrl_pts_file;
	ctrl_pts_file.open (ctrlpointspath);
	Utility::Offset ctrl_offset (0.0, 0.0, 0.0);
	//load ctrl pts
	pcl::PointXYZRGB ctrl_pt (0.0f, 0.0f, 0.0f), homo_ctrl_pts (0.0f, 0.0f, 0.0f);
	std::string dummy;
	float dummy_int = 0.0f;
	int num_pt = 0;

	int pointnum = 0;
	while (!ctrl_pts_file.eof ())
	{
		double x = 0.0, y = 0.0, z = 0.0;
		ctrl_pts_file >> x >> y >> z >> num_pt;
		if (ctrl_pts_cloud->points.size () == 0) {
			std::cout << "Set ctrl points offset\n";
			ctrl_offset.x = x;
			ctrl_offset.y = y;
			ctrl_offset.z = z;
		}
		ctrl_pt.x = x - ctrl_offset.x;
		ctrl_pt.y = y - ctrl_offset.y;
		ctrl_pt.z = z - ctrl_offset.z;

		//load homoym_pts
		ctrl_pts_file >> x >> y >> z >> dummy_int;
		homo_ctrl_pts.x = x - ctrl_offset.x;
		homo_ctrl_pts.y = y - ctrl_offset.y;
		homo_ctrl_pts.z = z - ctrl_offset.z;

		std::cout << std::fixed << "pointnum: " << pointnum << std::endl;
		std::cout << ctrl_pt.x << ", " << ctrl_pt.y << ", " << ctrl_pt.z << std::endl;
		std::cout << homo_ctrl_pts.x << ", " << homo_ctrl_pts.y << ", " << homo_ctrl_pts.z << std::endl;

		//skip the last empty line of ctrl point file
		if (ctrl_pt.x == homo_ctrl_pts.x) break;
		ctrl_pt.r = 255;
		ctrl_pt.g = 255;
		ctrl_pt.b = 0;
		homo_ctrl_pts.r = 0;
		homo_ctrl_pts.g = 255;
		homo_ctrl_pts.b = 0;
		ctrl_pts_cloud->push_back (ctrl_pt);
		homo_ctrl_pts_cloud->push_back (homo_ctrl_pts);
		++pointnum;
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
	pcl::registration::TransformationEstimationSVD<pcl::PointXYZRGB, pcl::PointXYZRGB> trans_est;
	Eigen::Matrix4f src2tar = Eigen::Matrix4f::Identity ();
	trans_est.estimateRigidTransformation (*homo_ctrl_pts_cloud, *ctrl_pts_cloud, correspondences, src2tar);

	std::string outpath = Utility::get_parent (ctrlpointspath);
	saveLAS2 (outpath + "/ctrlpts.las", ctrl_pts_cloud, ctrl_offset);
	saveLAS2 (outpath + "/ctrlpts_homo.las", homo_ctrl_pts_cloud, ctrl_offset);

	std::cout<< "Absolute transform matrix: \n" << src2tar;

	//transform points
	std::string laspaths = "E:/codefiles\\PG-Code\\small_pointcloud\\pointcloud";
	std::vector<std::string> files;
	Utility::get_files (laspaths, ".las", files);
	for (int lasi = 0; lasi < files.size (); ++lasi) {
		pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZRGB>> ();
		Utility::Offset offset (0, 0, 0);
		parseLAS (files [lasi], cloud, offset);
		pcl::transformPointCloud (*cloud, *cloud, src2tar);
		std::string outpath = Utility::get_parent (files [lasi]) + "/" + Utility::get_name_without_ext (files [lasi]) + "_transformed.las";
		std::cout << std::endl;
		saveLAS2 (outpath, cloud, offset);
	}
	system ("pause");
	return 1;
}

bool saveLAS2 (const std::string& filepath, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud,
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
			pt.SetColor (liblas::Color (cloud->points [i].r, cloud->points [i].g, cloud->points [i].b));

			writer.WritePoint (pt);
		}
		ofs.flush ();
		ofs.close ();
	}

	std::cout << "Save file: " << filepath;
	return true;
}

int parseLAS (std::string filename, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud, Utility::Offset& offset) {

	//open file
	std::ifstream ifs;
	ifs.open (filename, std::ios::in | std::ios::binary);
	if (!ifs)
	{
		std::cerr << "open file failed" << std::endl;
		return 0;
	}

	liblas::ReaderFactory f;
	liblas::Reader reader = f.CreateWithStream (ifs);
	const liblas::Header header = reader.GetHeader ();
	uint8_t major_version = header.GetVersionMajor ();
	uint8_t minor_version = header.GetVersionMinor ();
	if (major_version > 1 || minor_version > 3)
	{
		std::cerr << "Currently this app doesn't support version newer than 1.4";
		return 0;
	}

	//num of points
	uint32_t pts_count = header.GetPointRecordsCount ();
	uint32_t pt_length = header.GetDataRecordLength ();
	uint32_t las_offset = header.GetDataOffset ();
	liblas::PointFormatName las_format = header.GetDataFormatId ();
	double offset_x = header.GetOffsetX ();
	double offset_y = header.GetOffsetY ();
	double offset_z = header.GetOffsetZ ();
	offset = Utility::Offset (offset_x, offset_y, offset_z);

	while (reader.ReadNextPoint ())
	{
		const liblas::Point& p = reader.GetPoint ();
		pcl::PointXYZRGB pt;
		pt.x = p.GetX () - offset_x;
		pt.y = p.GetY () - offset_y;
		pt.z = p.GetZ () - offset_z;
		pt.r = p.GetColor ().GetRed () >> 8; //liblas uint16 to store r color
		pt.g = p.GetColor ().GetGreen () >> 8;
		pt.b = p.GetColor ().GetBlue () >> 8;

		cloud->push_back (pt);
	}

	return true;
}