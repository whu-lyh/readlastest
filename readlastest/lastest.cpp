
//test las read

#include <iostream>
#include <fstream>
#include <string>
#include <liblas/liblas.hpp>

int main () 
{
	//read las file
	std::string filename = "F:/Data/EstoniaNew/N6mme_testarea/20190717_082048_LD5_grab_log_VLP-16_XYZ_ypr-1_-38_-0.5_0.1_0.23_0.535_19-07-17.las";

	//open file
	std::ifstream ifs;
	ifs.open (filename, std::ios::in | std::ios::binary);
	if (!ifs)
		return 0;

	liblas::ReaderFactory f;
	liblas::Reader reader = f.CreateWithStream (ifs);
	const liblas::Header header = reader.GetHeader ();
	uint8_t major_version = header.GetVersionMajor ();
	uint8_t minor_version = header.GetVersionMinor ();
	if (major_version > 1 || minor_version > 3)
	{
		std::cout << "Currently this app doesn't support version newer than 1.4";
		return 0;
	}

	//num of points
	uint32_t pts_count = header.GetPointRecordsCount ();
	uint32_t offset = header.GetDataOffset ();
	uint32_t pt_length = header.GetDataRecordLength ();
	liblas::PointFormatName las_format = header.GetDataFormatId ();
	std::cout << las_format << std::endl;
	double offset_x = header.GetOffsetX ();
	double offset_y = header.GetOffsetY ();
	double offset_z = header.GetOffsetZ ();

	//bounding box
	liblas::Bounds<double> bound = header.GetExtent ();
	if (bound.empty ())
	{
		std::cout << "The header of this las doesn't contain extent. The cache cannot be built.";
		return 0;
	}

	//rewrite the las file
	std::ofstream ofs;
	ofs.open ("./filelas.las", std::ios::out | std::ios::binary);
	ofs.setf (std::ios::fixed, std::ios::floatfield);
	ofs.precision (6);
	std::cout << "done" << std::endl;

	if (ofs.is_open ())
	{
		liblas::Header oheader;
		oheader.SetDataFormatId (liblas::ePointFormat1); // Time only
		//oheader.SetDataFormatId (liblas::ePointFormat2); //color and intensity
		oheader.SetVersionMajor (1);
		oheader.SetVersionMinor (2);
		oheader.SetOffset (offset_x, offset_y, offset_z);
		oheader.SetScale (0.0001, 0.0001, 0.0001);
		oheader.SetPointRecordsCount (pts_count);

		/* Set coordinate system using GDAL support£¬ only for gdal2.0+
		liblas::SpatialReference srs;
		srs.SetFromUserInput ("EPSG:4326");
		oheader.SetSRS (srs);*/

		liblas::Writer writer (ofs, oheader);
		liblas::Point point (&oheader);

		while (reader.ReadNextPoint ())
		{
			liblas::Point const& p = reader.GetPoint ();

			//std::cout << p.GetX () << ", " << p.GetY () << ", " << p.GetZ () << "\n";
			point.SetCoordinates (p.GetX (), p.GetY (), p.GetZ ());
			/*point.SetIntensity (10);
			point.SetColor (liblas::Color (255, 255, 0));*/
			writer.WritePoint (point);
		}

	}
	ifs.close ();

	ofs.flush ();
	ofs.close ();

	system ("pause");
	return 0;
}