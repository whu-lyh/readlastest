#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <liblas/liblas.hpp>

int main ()
{
	std::string outputpath = "E:/PY-Group/Tallinn-Estonia/testDir/las/run1_xyz_gpstime.las";
	std::string filename = "E:/PY-Group/Tallinn-Estonia/testDir/txt/Run1_xyzit_az_id_XYZ_ypr2.8_-37_0_0.1_0.23_0.535_18-10-13.txt";

	std::ifstream ifs;
	ifs.open (filename, std::ios::in);
	if (!ifs)
		return 0;

	//rewrite the las file
	std::ofstream ofs;
	std::ios::openmode m = std::ios::out | std::ios::binary | std::ios::ate;
	ofs.open (outputpath, m);
	ofs.setf (std::ios::fixed, std::ios::floatfield);
	ofs.precision (6);

	liblas::Header header;
	header.SetVersionMajor (1);
	header.SetVersionMinor (2);
	header.SetDataFormatId (liblas::ePointFormat2);
	header.SetOffset (0.0, 0.0, 0.0);

	liblas::Writer writer (ofs, header);

	int pointcount = 0;
	while (!ifs.eof ())
	{
		if (pointcount < 1)
		{
			std::string s;
			std::getline (ifs, s);
			++pointcount;
			continue;
		}

		std::string singlelinestring, tmp;
		int z;
		double x, y, intensity, unixtime, ScanAngle, LaserID;

		ifs >> singlelinestring;

		std::vector<std::string> datavec;
		std::stringstream input (singlelinestring);

		while (getline (input, tmp, ',')) datavec.emplace_back (tmp);

		if (datavec.size () > 1) {
			x = stod (datavec [0]);
			y = stod (datavec [1]);
			z = stoi (datavec [2]);
			unixtime = stod (datavec [4]) - 1539413000; //this parameter could be changed when needed
			unixtime *= 10000;
			unixtime = std::floor (unixtime);
			/*std::cout << std::fixed << stod (datavec [4]) << std::endl;
			std::cout << unixtime << std::endl;*/
		}
		else
		{
			std::cout << "The point txt file hasn't parsed correctly! " << filename;
			return 0;
		}

		liblas::Point pt (&header);
		double px = static_cast<double>(x);
		double py = static_cast<double>(y);
		double pz = static_cast<double>(z);

		pt.SetCoordinates (px, py, pz);
		pt.SetTime (unixtime);//unixtime,太大了，以致于64位的都溢出，所以，暂时先搁置，等爱沙尼亚反馈。

		writer.WritePoint (pt);

		++pointcount;
	}
	ifs.close ();
	ofs.flush ();
	ofs.close ();

	system ("pause");
	return 0;
}