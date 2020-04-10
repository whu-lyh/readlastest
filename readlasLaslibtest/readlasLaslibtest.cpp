#ifndef LASLIB_DLL
#define LASLIB_DLL
#endif

#if _DEBUG
#pragma comment(lib,"LASlib_d.lib")
#else
#pragma comment(lib,"LASlib.lib")
#endif

#include <iostream>

#include "lasreader.hpp"
#include "laswriter.hpp"

int main()
{
	// 点云路径
	std::string file_path ("F:/Data/EstoniaNew/N6mme_testarea/20190717_082048_LD5_grab_log_VLP-16_XYZ_ypr-1_-38_-0.5_0.1_0.23_0.535_19-07-17.las");

	// 打开las文件
	LASreadOpener lasreadopener;
	lasreadopener.set_file_name (file_path.c_str ());
	if (!lasreadopener.active ())
	{
		std::cerr<<"ERROR: no input specified\n";
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
	//while (lasreader->read_point ()) {
	//	std::cout << std::fixed << lasreader->point.get_x () << "  "
	//		<< lasreader->point.get_y () << "  "
	//		<< lasreader->point.get_z () << "  "
	//		<<lasreader->point.get_R ()<<"  "
	//		<<lasreader->point.get_intensity ()<<" "
	//		<<lasreader->point.get_gps_time ()<<"  "
	//		<<std::endl;
	//}

	std::string file_path_out ("F:/Data/laslib.las");
	LASwriteOpener laswriteopener;
	laswriteopener.set_file_name (file_path_out.c_str ());

	LASwriter* laswriter = laswriteopener.open (&lasreader->header);
	if (laswriter == 0)
	{
		std::cerr << "ERROR: could not open laswriter\n";
	}

	//// where there is a point to read
	//while (lasreader->read_point ())
	//{
	//	// modify the point
	//	lasreader->point.set_point_source_ID (1020);
	//	lasreader->point.set_user_data (42);
	//	if (lasreader->point.get_classification () == 12) lasreader->point.set_classification (1);
	//	lasreader->point.set_Z (lasreader->point.get_Z () + 10);
	//	// write the modified point
	//	laswriter->write_point (&lasreader->point);
	//	// add it to the inventory
	//	laswriter->update_inventory (&lasreader->point);
	//}

	// copy header for output

	LASheader header = lasreader->header;

	// zero the pointers of the other header so they don't get deallocated twice

	lasreader->header.unlink ();

	// we need a new LAS point type for adding RGB

	U8 point_type = header.point_data_format;
	U16 point_size = header.point_data_record_length;

	switch (point_type)
	{
	case 0:
		point_type = 2;
		point_size += 6;
		break;
	case 1:
		point_type = 3;
		point_size += 6;
		break;
	case 4:
		point_type = 5;
		point_size += 6;
		break;
	default:
		std::cerr << "ERROR: point type %d not supported\n" << (I32)point_type;
	}

	header.point_data_format = point_type;
	header.point_data_record_length = point_size;

	LASpoint point;
	// where there is a point to read
	while (lasreader->read_point ())
	{
		// copy the point
		point = lasreader->point;
		// change RGB
		point.rgb [0] = point.rgb [1] = point.rgb [2] = U16_QUANTIZE (((point.get_z () - header.min_z)*65535.0) / (header.max_z - header.min_z));
		if (lasreader->point.get_classification () == 12) lasreader->point.set_classification (1);
		// write the modified point
		laswriter->write_point (&point);
	}

	laswriter->update_header (&lasreader->header, TRUE);

	I64 total_bytes = laswriter->close ();
	delete laswriter;

	lasreader->close ();
	delete lasreader;

    std::cout << "Hello World!\n"; 
	return 0;
}