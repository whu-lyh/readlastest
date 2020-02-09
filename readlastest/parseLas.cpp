
//test las read

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <liblas/liblas.hpp>
#include <boost/filesystem.hpp>
#include "Utility.h"

uint32_t parsesinglelas (std::string filename);

int main ()
{
	//read las files from a directory
	std::string filenpaths = "F:/wuhan/iScan-00000000-20191211103207";

	std::vector<std::string> lasfiles;
	Utility::get_files ( filenpaths, ".las", lasfiles );

	uint32_t pointNum = 0;
#pragma parallel for
	for (int i =0; i<lasfiles.size();++i)
	{
		std::cout << "current file name:	" << lasfiles [i] << std::endl;
		pointNum += parsesinglelas ( lasfiles [i] );
	}
	std::cout << "current las directory's point number is:	" << pointNum << std::endl;

	system ( "pause" );
	return 0;
}

uint32_t parsesinglelas ( std::string filename )
{
	//open file
	std::ifstream ifs;
	ifs.open ( filename, std::ios::in | std::ios::binary );
	if ( !ifs )
	{
		std::cerr << "open file failed" << std::endl;
		return 0;
	}	

	liblas::ReaderFactory f;
	liblas::Reader reader = f.CreateWithStream ( ifs );
	const liblas::Header header = reader.GetHeader ();
	uint8_t major_version = header.GetVersionMajor ();
	uint8_t minor_version = header.GetVersionMinor ();
	if ( major_version > 1 || minor_version > 3 )
	{
		std::cerr << "Currently this app doesn't support version newer than 1.4";
		return 0;
	}

	//num of points
	uint32_t pts_count = header.GetPointRecordsCount ();
	uint32_t offset = header.GetDataOffset ();
	uint32_t pt_length = header.GetDataRecordLength ();
	liblas::PointFormatName las_format = header.GetDataFormatId ();
	double offset_x = header.GetOffsetX ();
	double offset_y = header.GetOffsetY ();
	double offset_z = header.GetOffsetZ ();
	//las_offset = Offset (offset_x, offset_y, offset_z);

	//bounding box
	liblas::Bounds<double> bound = header.GetExtent ();

	//while ( reader.ReadNextPoint () )
	//{
	//	liblas::Point const& p = reader.GetPoint ();
	//	//std::cout << p.GetX () << ", " << p.GetY () << ", " << p.GetZ () << "\n";
	//}

	return pts_count;
}