﻿// readlas.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


//test las read

#include <iostream>
#include <fstream>
#include <string>
#include <liblas/liblas.hpp>

int readlas ()
{
	//read las file
	std::string filename = "E:/PY-Group/车载点云数据质量改善-总结/test/testdata/lu-2019-0327/all-in-one-lu/190327_031549-11-16-10-313.las";

	//open file
	std::ifstream ifs;
	ifs.open (filename, std::ios::in | std::ios::binary);
	if (!ifs)
		return false;

	liblas::ReaderFactory f;
	liblas::Reader reader = f.CreateWithStream (ifs);
	const liblas::Header header = reader.GetHeader ();
	uint8_t major_version = header.GetVersionMajor ();
	uint8_t minor_version = header.GetVersionMinor ();
	if (major_version > 1 || minor_version > 3)
	{
		std::cout << "Currently this app doesn't support version newer than 1.4";
		return false;
	}

	//num of points
	uint32_t pts_count = header.GetPointRecordsCount ();
	uint32_t offset = header.GetDataOffset ();
	uint32_t pt_length = header.GetDataRecordLength ();
	liblas::PointFormatName las_format = header.GetDataFormatId ();
	double offset_x = header.GetOffsetX ();
	double offset_y = header.GetOffsetY ();
	double offset_z = header.GetOffsetZ ();

	//bounding box
	liblas::Bounds<double> bound = header.GetExtent ();
	if (bound.empty ())
	{
		std::cout << "The header of this las doesn't contain extent. The cache cannot be built.";
		return false;
	}

	while (reader.ReadNextPoint ())
	{
		liblas::Point const& p = reader.GetPoint ();

		//std::cout << p.GetX () << ", " << p.GetY () << ", " << p.GetZ () << "\n";
	}
	ifs.close ();

	//rewrite the las file
	/*std::ofstream ofs;
	ofs.open ("file.las", std::ios::out | std::ios::binary);

	liblas::Header oheader = reader.GetHeader ();*/

	/*std::ios::openmode m = std::ios::out | std::ios::in | std::ios::binary | std::ios::ate;
	ofs.open ("file.las", m);
	liblas::Writer writer (ofs, oheader);*/

	//liblas::Header oheader;
	//oheader.SetDataFormatId (liblas::ePointFormat1); // Time only

	//// Set coordinate system using GDAL support
	//liblas::SpatialReference srs;
	//srs.SetFromUserInput ("EPSG:4326");

	//oheader.SetSRS (srs);

	//liblas::Writer writer (ofs, oheader);

	//liblas::Point point (&oheader);
	//point.SetCoordinates (10, 20, 30);
	//// fill other properties of point record

	//writer.WritePoint (point);

	//ofs.close ();

	// fill other header members

	system ("pause");
	return 0;
}
