
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

void parseLAS(liblas::Reader& reader, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud, const Utility::Offset& offset);
bool saveLAS(const std::string& filepath, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud,
	const Utility::Offset& offset);
void parseLASmmf(const char* pFile, uint32_t pts_num, const liblas::Header& header, uint32_t pt_length,
	const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud, const Utility::Offset& offset);

// argv[1], point cloud path
// argv[2], vacuate ratio
int main(int argc, char* argv[])
{
	//read las files from a directory
	//std::string filenpaths = "D:/las/RIEGL";
	std::string filenpaths = argv[1];// "F:/Data/20210904_TianRunTech_data/GeoReferencedPC/DATA20210712060828/coordinate";
	//std::string filenpaths = "D:/data/wuhan/vacuatedpc";

	std::vector<std::string> lasfiles;
	Utility::get_files(filenpaths, ".las", lasfiles);

	uint32_t pointNum = 0;
//#pragma omp parallel for
	for (int i = 0; i < lasfiles.size(); ++i)
	{
		std::cout << "current file name:	" << lasfiles[i] << std::endl;

		//open file
		std::ifstream ifs;
		ifs.open(lasfiles[i], std::ios::in | std::ios::binary);
		if (!ifs)
			return false;

		liblas::ReaderFactory f;
		liblas::Reader reader = f.CreateWithStream(ifs);

		const liblas::Header header = reader.GetHeader();
		uint8_t major_version = header.GetVersionMajor();
		uint8_t minor_version = header.GetVersionMinor();

		if (major_version > 1 || minor_version > 3)
		{
			std::cerr << "Currently this app doesn't support version newer than 1.4";
			return 0;
		}

		//num of points
		uint32_t pts_count = header.GetPointRecordsCount();
		uint32_t pt_length = header.GetDataRecordLength();
		uint32_t las_offset = header.GetDataOffset();
		liblas::PointFormatName las_format = header.GetDataFormatId();
		double offset_x = header.GetOffsetX();
		double offset_y = header.GetOffsetY();
		double offset_z = header.GetOffsetZ();

		pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudin = boost::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
		pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudout = boost::make_shared<pcl::PointCloud<pcl::PointXYZRGB>>();
		Utility::Offset offset(offset_x, offset_y, offset_z);

		//file size
		uintmax_t sz = boost::filesystem::file_size(lasfiles[i]);
		//memory mapping to load point file
		if (sz >= 400 * 1024 * 1024) //larger than 400M
		{
			HANDLE file_handle = CreateFile(lasfiles[i].c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == file_handle)
			{
				std::cout << "Failed because of err.";
				return false;
			}

			LARGE_INTEGER file_sz;
			GetFileSizeEx(file_handle, &file_sz);
			HANDLE mapping_handle = CreateFileMapping(file_handle, NULL, PAGE_READONLY, 0, 0, "LAS FILE MAPPING");
			if (INVALID_HANDLE_VALUE == mapping_handle)
			{
				std::cout << "Mapping file failed.";
				return false;
			}

			//system info
			SYSTEM_INFO sys_info;
			GetSystemInfo(&sys_info);
			DWORD allocation_granularity = sys_info.dwAllocationGranularity;

			LARGE_INTEGER cur_size;
			cur_size.QuadPart = 0;
			const uint32_t EACH_POINT_NUM = allocation_granularity * 200;
			const uint32_t EACH_SIZE = EACH_POINT_NUM * pt_length;
			char *pFile = nullptr;
			pFile = (char*)MapViewOfFile(mapping_handle, FILE_MAP_READ, 0, 0, EACH_SIZE * 1.1);

			pFile += las_offset;
			parseLASmmf(pFile, EACH_POINT_NUM, header, pt_length, cloudin, offset);
			UnmapViewOfFile(pFile - las_offset);
			cur_size.QuadPart += EACH_SIZE;
			while (true)
			{
				if (cur_size.QuadPart + EACH_SIZE * 1.1 < file_sz.QuadPart)
				{
					pFile = (char*)MapViewOfFile(mapping_handle, FILE_MAP_READ, cur_size.HighPart, cur_size.LowPart, EACH_SIZE * 1.1);
					pFile += las_offset;
					parseLASmmf(pFile, EACH_POINT_NUM, header, pt_length, cloudin, offset);
					UnmapViewOfFile(pFile - las_offset);
					cur_size.QuadPart += EACH_SIZE;
				}
				else //the last section
				{
					uint32_t rest_sz = file_sz.QuadPart - cur_size.QuadPart;
					pFile = (char*)MapViewOfFile(mapping_handle, FILE_MAP_READ, cur_size.HighPart, cur_size.LowPart, rest_sz);
					pFile += las_offset;
					uint32_t rest_count = pts_count - cur_size.QuadPart / pt_length;
					parseLASmmf(pFile, rest_count, header, pt_length, cloudin, offset);
					UnmapViewOfFile(pFile);
					break;
				}
			}
			CloseHandle(mapping_handle);
			CloseHandle(file_handle);
		}
		else //less than 400M
		{
			parseLAS(reader, cloudin, offset);
		}

		ifs.close();

		pcl::VoxelGrid<pcl::PointXYZRGB> down_filter;
		float leaf = std::stof(argv[2]);
		down_filter.setLeafSize(leaf, leaf, leaf);
		down_filter.setInputCloud(cloudin);
		down_filter.filter(*cloudout);

		std::string newdir = Utility::get_parent(filenpaths) + "/small_" + Utility::get_name(filenpaths);
		Utility::ensure_dir(newdir);
		std::string outputpath = newdir + "/small_" + Utility::get_name(lasfiles[i]);

		saveLAS(outputpath, cloudout, offset);
		//close input file
		ifs.close();
		std::cout << "current las name is:	" << cloudout->points.size() << std::endl;
	}

	system("pause");
	return 0;
}

void parseLAS(liblas::Reader& reader, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud, const Utility::Offset& offset)
{
	while (reader.ReadNextPoint())
	{
		const liblas::Point& p = reader.GetPoint();
		pcl::PointXYZRGB pt;
		pt.x = p.GetX() - offset.x;
		pt.y = p.GetY() - offset.y;
		pt.z = p.GetZ() - offset.z;
		pt.r = p.GetColor().GetRed() >> 8;
		pt.g = p.GetColor().GetGreen() >> 8;
		pt.b = p.GetColor().GetBlue() >> 8;

		cloud->push_back(pt);
	}
}

bool saveLAS(const std::string& filepath, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud,
	const Utility::Offset& offset)
{
	if (cloud == nullptr)
	{
		std::cout << "Pointer 'cloud' is nullptr!";
		return false;
	}

	if (cloud->empty())
	{
		std::cout << "Point cloud is empty!";
		return false;
	}

	std::ofstream ofs;
	ofs.open(filepath, std::ios::out | std::ios::binary);
	ofs.setf(std::ios::fixed, std::ios::floatfield);
	ofs.precision(6);

	if (ofs.is_open())
	{
		liblas::Header header;
		header.SetDataFormatId(liblas::ePointFormat2);
		header.SetVersionMajor(1);
		header.SetVersionMinor(2);
		header.SetOffset(offset.x, offset.y, offset.z);
		header.SetScale(0.0001, 0.0001, 0.0001);
		header.SetPointRecordsCount(cloud->size());

		liblas::Writer writer(ofs, header);
		liblas::Point pt(&header);

		for (int i = 0; i < cloud->size(); ++i)
		{
			double x = static_cast<double>(cloud->points[i].x) + offset.x;
			double y = static_cast<double>(cloud->points[i].y) + offset.y;
			double z = static_cast<double>(cloud->points[i].z) + offset.z;

			pt.SetCoordinates(x, y, z);
			pt.SetIntensity(10);
			pt.SetColor(liblas::Color(cloud->points[i].r, cloud->points[i].g, cloud->points[i].b));

			writer.WritePoint(pt);
		}
		ofs.flush();
		ofs.close();
	}

	std::cout << "Save file: " << filepath;
	return true;
}

void parseLASmmf(const char* pFile, uint32_t pts_num, const liblas::Header& header, uint32_t pt_length,
	const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud, const Utility::Offset& offset)
{
	switch (header.GetDataFormatId())
	{
	case liblas::ePointFormat0:
	case liblas::ePointFormat1:
	case liblas::ePointFormat2:
	case liblas::ePointFormat3:
	{
		for (int i = 0; i < pts_num; i++)
		{
			liblas::Point p(&header);
			memcpy(reinterpret_cast<char*>(&(p.GetData().front())), pFile + i * pt_length, pt_length);
			LIBLAS_SWAP_BYTES_N(p.GetData().front(), pt_length);
			pcl::PointXYZRGB pt;
			pt.x = p.GetX() - offset.x;
			pt.y = p.GetY() - offset.y;
			pt.z = p.GetZ() - offset.z;
			pt.r = p.GetColor().GetRed() >> 8;
			pt.g = p.GetColor().GetGreen() >> 8;
			pt.b = p.GetColor().GetBlue() >> 8;
			cloud->push_back(pt);
		}
		break;
	}
	default:
	{
		std::cout << "暂不支持Point Data Record Format 4及以上";
		return;
	}
	}
}