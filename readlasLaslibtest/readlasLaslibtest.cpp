﻿#if _DEBUG
#pragma comment(lib,"LASlib_d.lib")
#else
#pragma comment(lib,"LASlib.lib")
#endif

#include <iostream>
#include <time.h>

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

	//some info read from header file
	size_t point_count = lasreader->header.number_of_point_records;
	std::cout << "Point numbers:	" << point_count << std::endl;
	size_t record_length = lasreader->header.point_data_record_length;
	std::cout << "Record length:	" << record_length << std::endl;
	size_t header_size = lasreader->header.header_size;
	std::cout << "Header size:	" << lasreader->header.header_size << std::endl;
	size_t offset_to_point_data = lasreader->header.offset_to_point_data;
	std::cout << "Offset_to_point_data" << lasreader->header.offset_to_point_data << std::endl;
	double x_offset = lasreader->header.x_offset;
	std::cout << "X offset" << x_offset << std::endl;
	double x_scale_factor = lasreader->header.x_scale_factor;
	std::cout << "X scale factor" << x_scale_factor << std::endl;

	//lasreader->header.set_bounding_box ();

	std::string file_path_out ("./laslibOutputPointCloud.las");
	LASwriteOpener laswriteopener;
	laswriteopener.set_file_name (file_path_out.c_str ());

	//header file for output
	LASheader header;
	strncpy (header.system_identifier, "Group.Yang", 11);
	header.system_identifier [10] = '\0';
	strncpy (header.generating_software, "0.1", 4);
	header.generating_software [3] = '\0';

	//set creation date ==> maybe useless
	time_t now=time (0);
	tm *ltm = localtime (&now);
	header.file_creation_year = 1900 + ltm->tm_year;
	header.file_creation_day = 1+ltm->tm_mon;
	std::cout << ltm->tm_year << std::endl;
	std::cout << ltm->tm_mon << std::endl;
	std::cout << ltm->tm_mday << std::endl;

	// zero the pointers of the other header so they don't get deallocated twice
	lasreader->header.unlink ();

	// we need a new LAS point type for adding RGB
	U8 point_type = header.point_data_format;
	//xyz->0,xyzrgb->2,xyzt->1,xyztrgb->3
	U16 point_size = header.point_data_record_length;
	//xyz->20,xyzrgb->26,xyzt->28,xyztrgb->34

	if (point_type > 5)
	{
		header.version_minor = 4;
		header.header_size = 375;
		header.offset_to_point_data = 375;
	}
	else
	{
		header.version_minor = 2;
		header.header_size = 227;
		header.offset_to_point_data = 227;
	}

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
	header.x_scale_factor = 0.1;
	header.y_scale_factor = 0.1;
	header.z_scale_factor = 0.1;
	header.x_offset = 50000;
	header.y_offset = 50000;
	header.z_offset = 200;
	//点云精度怎么设置？
	//for instance
	//std::ofstream ofs;
	//ofs.open (filepath, std::ios::out | std::ios::binary);
	//ofs.setf (std::ios::fixed, std::ios::floatfield);
	//ofs.precision (6);

	LASwriter* laswriter = laswriteopener.open (&header);
	if (laswriter == 0)
	{
		std::cerr << "ERROR: could not open laswriter\n";
	}

	LASpoint *point = new LASpoint();
	//LASpoint p;//??

	//whether acivate compression mode?
	if (header.laszip)
	{
		LASzip* laszip = new LASzip();
		laszip->setup(header.point_data_format, header.point_data_record_length);
		point->init(&header, laszip->num_items, laszip->items, &header);
		delete header.laszip;
		header.laszip = laszip;
	}
	else
	{
		point->init (&header, header.point_data_format, header.point_data_record_length, 0);
	}
	
	// where there is a point to read
	while (lasreader->read_point ())
	{
		// copy the point
		//*point = lasreader->point;
		point->set_x (lasreader->point.get_x ());
		point->set_y (lasreader->point.get_y ());
		point->set_z (lasreader->point.get_z ());
		point->rgb [0] = U16_QUANTIZE(255);
		point->rgb [1] = U16_QUANTIZE (0);
		point->rgb [2] = U16_QUANTIZE(120);

		point->set_number_of_returns (15);

		//before write a las the init should be done 
		//point.quantizer = lasreader->point.quantizer;

		//std::cout << std::fixed << lasreader->point.get_x () << std::endl;
		//std::cout << std::fixed << point.get_X () << std::endl;
		//std::cout << point.quantizer->get_x (point.get_X ()) << std::endl;

		// change RGB
		//point->rgb [0] = point->rgb [1] = point->rgb [2] = U16_QUANTIZE (((point->get_z () - header.min_z)*65535.0) / (header.max_z - header.min_z));
		//if (lasreader->point.get_classification () == 12) lasreader->point.set_classification (1);
		// write the modified point
		laswriter->write_point (point);
		laswriter->update_inventory (point);
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