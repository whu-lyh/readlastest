
//test las read

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <liblas/liblas.hpp>
#include <boost/filesystem.hpp>
#include "Utility.h"

uint32_t parsesinglelas (std::string filename);

int main2 ()
{
	//read las files from a directory
	std::string filenpaths = "D:/data/wuhan/iScan-00000000-20191212152907";

	std::vector<std::string> lasfiles;
	Utility::get_files ( filenpaths, ".las", lasfiles );

	uint32_t pointNum = 0;
#pragma omp parallel for
	for (int i =0; i<lasfiles.size();++i)
	{
		std::cout << "current file name:	" << lasfiles [i] << std::endl;
		//pointNum += parsesinglelas ( lasfiles [i] );
	}
	std::cout << "current las directory's point number is:	" << pointNum << std::endl;

	parsesinglelas ( lasfiles [0] );

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

	//different way to load point using for loop
	reader.ReadPointAt ( pts_count - 3 );
	liblas::Point const& plast3 = reader.GetPoint ();
	std::cout << plast3 << std::endl;
	reader.ReadPointAt ( pts_count - 2 );
	liblas::Point const& plast2 = reader.GetPoint ();
	std::cout << plast2 << std::endl;
	reader.ReadPointAt ( pts_count - 1 );
	liblas::Point const& plast1 = reader.GetPoint ();
	std::cout << plast1 << std::endl;

	/*while ( reader.ReadNextPoint () )
	{

		liblas::Point const& p = reader.GetPoint ();
		std::cout << p << std::endl;
		std::cout << std::endl;
	}*/

	return pts_count;
}