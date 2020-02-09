#pragma once
#pragma warning(disable: 4267)
#pragma warning(disable: 4819)
#pragma warning(disable: 4273)

namespace Utility
{
	//some usable const variable
	const float Epsilon_f = std::numeric_limits<float>::epsilon() * 10.f; //Epsilon--float
	const double Epsilon_d = std::numeric_limits<double>::epsilon() * 10.0; //Epsilon--double
	typedef std::list<std::string> filelist;

	class  Point3d
	{
	public:
		Point3d(double x0 = 0, double y0 = 0, double z0 = 0)
			: x(x0), y(y0), z(z0)
		{}

		Point3d operator +=(const Point3d& pt)
		{
			x += pt.x;
			y += pt.y;
			z += pt.z;
			return *this;
		}
	public:
		double x;
		double y;
		double z;
	};

	typedef Point3d Offset;

	extern  void ensure_dir(const std::string &dir);

	extern  void format_path(std::string &path);

	extern  std::string make_path(const std::string &dir, const std::string &file);

	extern  std::string replace_ext(const std::string &file, const std::string &ext);

	extern  std::string get_ext(const std::string &file);

	extern  std::string get_name(const std::string &file);

	extern  std::string get_name_without_ext(const std::string &file);

	extern  std::string get_parent(const std::string &path);

	extern  void get_files(const std::string& dir, const std::string& ext, filelist& list);

	extern  void get_files(const std::string& dir, const std::string& ext, std::vector<std::string>& files);

	extern  bool file_exist(const std::string& file);

	extern  bool is_directory(const std::string& dir);

	extern  bool copy_file(const std::string& to_dir, const std::string& file, bool overlay = false);

	extern  void rename(const std::string& new_name, const std::string& old_name);

	extern  void remove_file(const std::string& filename);

	extern  void remove_dir(const std::string& dir);

	extern  void getRandomColor(int& r, int& g, int& b);

}//namespace Utility