
//vacuate the las point cloud  to a small one

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <liblas/liblas.hpp>
#include <boost/filesystem.hpp>
#include "Utility.h"

int main ()
{
	//read las files from a directory
	std::string filenpaths = "D:/data/wuhan/iScan-00000000-20191212152907";

	std::vector<std::string> lasfiles;
	Utility::get_files ( filenpaths, ".las", lasfiles );

	uint32_t pointNum = 0;
//#pragma omp parallel for
	for ( int i = 0; i<lasfiles.size (); ++i )
	{
		std::cout << "current file name:	" << lasfiles [i] << std::endl;

		//open file
		std::ifstream ifs;
		ifs.open ( lasfiles [i], std::ios::in | std::ios::binary );
		if ( !ifs )
			return false;

		liblas::ReaderFactory f;
		liblas::Reader &reader = f.CreateWithStream ( ifs );

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

		//write the las file
		std::ofstream ofs;
		std::ios::openmode m = std::ios::out | std::ios::binary | std::ios::ate;

		std::string outputpath = filenpaths + "/small" + Utility::get_name ( filenpaths ) + ".las";

		ofs.open ( outputpath, m );
		ofs.setf ( std::ios::fixed, std::ios::floatfield );
		ofs.precision ( 6 );

		if ( ofs.is_open () )
		{
			liblas::Header outheader;
			outheader.SetVersionMajor ( major_version );
			outheader.SetVersionMinor ( minor_version );
			outheader.SetDataFormatId ( las_format ); //不同格式之间的区别是什么？反正2是无法settime的
			outheader.SetScale ( 0.0001, 0.0001, 0.0001 );
			outheader.SetOffset ( offset_x, offset_y, offset_z );

			uint32_t outpts_count=0, tmp_count=0;
			while ( reader.ReadNextPoint () )
			{
				liblas::Writer writer ( ofs, outheader );
				liblas::Point pt ( &outheader );

				liblas::Point ptread = reader.GetPoint ();
				++tmp_count;
				if ( outpts_count %500 == 0)
				{
					++outpts_count;

					double x = static_cast<double>( ptread.GetX() ) + offset_x;
					double y = static_cast<double>( ptread.GetY() ) + offset_y;
					double z = static_cast<double>( ptread.GetZ() ) + offset_z;

					pt.SetCoordinates ( x, y, z );
					pt.SetIntensity ( ptread.GetIntensity () );
					pt.SetColor ( ptread.GetColor () );

					writer.WritePoint ( pt );
				}
			}
			ofs.flush ();
			ofs.close ();
		}
		std::cout << i << std::endl;
		//close file
		ifs.close ();
		std::cout << "current las name is:	" << outputpath << std::endl;
	}

	system ( "pause" );
	return 0;
}