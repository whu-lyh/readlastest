﻿#if _DEBUG
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
	std::cout << point_count << std::endl;

	//lasreader->header.set_bounding_box ();
	lasreader->header.x_offset;
	lasreader->header.x_scale_factor;

	std::string file_path_out ("./laslib.las");
	LASwriteOpener laswriteopener;
	laswriteopener.set_file_name (file_path_out.c_str ());

	// copy header for output
	LASheader header;
	strncpy (header.system_identifier, "Group.Yang", 11);
	header.system_identifier [10] = '\0';
	strncpy (header.generating_software, "0.1", 4);
	header.generating_software [3] = '\0';

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

	LASwriter* laswriter = laswriteopener.open (&header);
	if (laswriter == 0)
	{
		std::cerr << "ERROR: could not open laswriter\n";
	}

	LASpoint point;
	point.init (&header, header.point_data_format, header.point_data_record_length, 0);
	// where there is a point to read
	while (lasreader->read_point ())
	{
		// copy the point
		point = lasreader->point;
		//before write a las the init should be done 
		//point.quantizer = lasreader->point.quantizer;

		//std::cout << std::fixed << lasreader->point.get_x () << std::endl;
		//std::cout << std::fixed << point.get_X () << std::endl;
		//std::cout << point.quantizer->get_x (point.get_X ()) << std::endl;

		// change RGB
		point.rgb [0] = point.rgb [1] = point.rgb [2] = U16_QUANTIZE (((point.get_z () - header.min_z)*65535.0) / (header.max_z - header.min_z));
		if (lasreader->point.get_classification () == 12) lasreader->point.set_classification (1);
		// write the modified point
		laswriter->write_point (&point);
		laswriter->update_inventory (&point);
	}

	laswriter->update_header (&header, TRUE);

	I64 total_bytes = laswriter->close ();
	if (laswriter == 0)
	{
		std::cerr << "ERROR: could not open laswriter\n";
	}
	delete laswriter;

	lasreader->close ();
	if (lasreader == 0)
	{
		std::cerr << "ERROR: could not open lasreader\n";
	}
	delete lasreader;

    std::cout << "Hello World!\n"; 
	system ("pause");
	return 0;
}