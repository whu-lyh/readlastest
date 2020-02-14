#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <liblas/liblas.hpp>

enum index { underflow, overflow };

int main1 ()
{
	std::string outputpath = "E:/PY-Group/Tallinn-Estonia/testDir/las/run4_xyz_gpstime.las";
	std::string filename = "E:/PY-Group/Tallinn-Estonia/testDir/txt/Run4_xyzit_az_id_XYZ_ypr2.8_-37_0_0.1_0.23_0.535_18-10-13.txt";

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
	header.SetDataFormatId (liblas::ePointFormat3); //不同格式之间的区别是什么？反正2是无法settime的
	header.SetOffset (0.0, 0.0, 0.0);

	liblas::Writer writer (ofs, header);

	long int pointcount = 0;
	double maxX = 0;
	double maxY = 0;
	double maxZ = 0;
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

		try
		{
			ifs >> singlelinestring;

			std::vector<std::string> datavec;
			std::stringstream input (singlelinestring);

			while (getline (input, tmp, ',')) if(tmp != "") datavec.emplace_back (tmp);

			if (datavec.size () > 1) {
				x = stod (datavec [0]);
				y = stod (datavec [1]);
				z = stoi (datavec [2]);
				unixtime = stod (datavec [4]) - 1539413300; //this parameter could be changed when needed
				unixtime *= 1000000;
				unixtime = static_cast<int>(unixtime) / 1000000.0;
				maxX = x > maxX ? x : maxX;
				maxY = y > maxY ? y : maxY;
				maxZ = z > maxZ ? z : maxZ;
				/*std::cout.precision (7);
				std::cout << std::fixed << stod (datavec [4]) << std::endl;
				std::cout << unixtime << std::endl;*/
			}
			else
			{
				std::cout << "The point txt file hasn't parsed correctly! " << filename;
				//return 0;
			}

			liblas::Point pt (&header);
			double px = static_cast<double>(x);
			double py = static_cast<double>(y);
			double pz = static_cast<double>(z);
			pt.SetCoordinates (px, py, pz);
			header.SetOffset (maxX,maxY,maxZ);
			pt.SetTime (unixtime);//unixtime,太大了，以致于64位的都溢出，所以，暂时先搁置，等爱沙尼亚反馈。

			writer.WritePoint (pt);
		}
		catch (std::exception e)
		{				
			std::cout << e.what() << std::endl;
		}
		++pointcount;
	}
	
	std::cout << pointcount << std::endl;
	ifs.close ();
	ofs.flush ();
	ofs.close ();

	system ("pause");
	return 0;
}