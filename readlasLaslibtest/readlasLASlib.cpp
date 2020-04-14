#if _DEBUG
#pragma comment(lib,"LASlib_d.lib")
#else
#pragma comment(lib,"LASlib.lib")
#endif

#include <iostream>

#include "lasreader.hpp"
#include "laswriter.hpp"

int main0 ()
{
	// 点云路径
	std::string file_path ("F:/Data/EstoniaNew/N6mme_testarea/20190717_082048_LD5_grab_log_VLP-16_XYZ_ypr-1_-38_-0.5_0.1_0.23_0.535_19-07-17.las");

	// 打开las文件
	LASreadOpener lasreadopener;
	lasreadopener.set_file_name (file_path.c_str ());
	if (!lasreadopener.active ())
	{
		std::cerr << "ERROR: no input specified\n";
	}
	LASreader* lasreader = lasreadopener.open ();
	if (lasreader == 0)
	{
		std::cerr << "ERROR: could not open lasreader\n";
	}
	size_t point_count = lasreader->header.number_of_point_records;

	//lasreader->header.set_bounding_box ();
	lasreader->header.x_offset;
	lasreader->header.x_scale_factor;
	U8 point_type_ = lasreader->header.point_data_format;
	std::cout << point_type_ << std::endl;

	// 遍历点云
	while (lasreader->read_point ()) {
		std::cout << std::fixed 
			<< lasreader->point.get_x () << "  "
			<< lasreader->point.get_y () << "  "
			<< lasreader->point.get_z () << "  "
			<< lasreader->point.get_R () << "  "
			<< lasreader->point.get_intensity () << " "
			<< lasreader->point.get_gps_time () << "  "
			<< std::endl;
	}

	lasreader->close ();
	delete lasreader;

	std::cout << "Las file read done!\n";
	system ("pause");
	return 0;
}