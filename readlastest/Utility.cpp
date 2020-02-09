#define NOMINMAX
#include <windows.h>
#include <omp.h>
#include <iostream>
#include <boost/version.hpp>
#include <boost/filesystem.hpp>
#include "Utility.h"

namespace Utility
{

#ifdef WIN32
#define DIR_INTERVAL '\\'
#else
#define DIR_INTERVAL '/'
#endif

	//ȷ����ǰ·������
	void ensure_dir (const std::string & dir)
	{
		boost::filesystem::path fullpath (dir);
		if (!boost::filesystem::exists (fullpath))
		{
			boost::filesystem::create_directory (fullpath);
		}
	}

	//·���ַ������/��\��β;
	void format_path (std::string & path)
	{
		size_t size = path.size ();
		if (size == 0 || path [size - 1] != DIR_INTERVAL)
			path.push_back (DIR_INTERVAL);
	}

	//����·�����ļ�����������·��;
	std::string make_path (const std::string & dir, const std::string & file)
	{
		std::string path = dir;
		format_path (path);
		return path + file;
	}

	//�滻�ļ���չ��;
	std::string replace_ext (const std::string & file, const std::string & ext)
	{
		size_t pos = file.rfind ('.');
		return file.substr (0, pos) + ext;
	}

	//�ļ���չ��;
	std::string get_ext (const std::string & file)
	{
		size_t pos = file.rfind ('.');
		return (pos != std::string::npos) ? file.substr (pos) : std::string ();
	}

	//�ļ���;
	std::string get_name (const std::string & file)
	{
		boost::filesystem::path p (file);
#if BOOST_VERSION > 104000
		return p.filename ().string ();
#else
		return p.filename ();
#endif
	}

	std::string get_name_without_ext (const std::string &file)
	{
		std::string name = get_name (file);
		std::string ext = get_ext (file);
		return name.substr (0, name.length () - ext.length ());
	}

	//�ļ���Ŀ¼;
	std::string get_parent (const std::string &path)
	{
		boost::filesystem::path p (path);
#if BOOST_VERSION > 104000
		return p.parent_path ().string ();
#else
		return p.parent_path ();
#endif
	}

	//����ļ��������е��ļ�;
	void get_files (const std::string& dir, const std::string& ext, filelist& list)
	{
		boost::filesystem::path fullPath (boost::filesystem::initial_path ());
		fullPath = boost::filesystem::system_complete (boost::filesystem::path (dir));

		if (!boost::filesystem::exists (fullPath) || !boost::filesystem::is_directory (fullPath))
		{
			return;
		}

		boost::filesystem::directory_iterator end_iter;
		for (boost::filesystem::directory_iterator file_itr (fullPath); file_itr != end_iter; ++file_itr)
		{
			if (!boost::filesystem::is_directory (*file_itr) && (boost::filesystem::extension (*file_itr) == ext || ext == ""))
			{
#if BOOST_VERSION > 104000
				std::string str = make_path (dir, file_itr->path ().filename ().string ());
#else
				std::string str = make_path (dir, file_itr->path ().filename ());
#endif
				list.emplace_back (str);
			}
		}
	}
	//�������أ�����vector
	void get_files (const std::string& dir, const std::string& ext, std::vector<std::string>& files)
	{
		boost::filesystem::path fullPath (boost::filesystem::initial_path ());
		fullPath = boost::filesystem::system_complete (boost::filesystem::path (dir));

		if (!boost::filesystem::exists (fullPath) || !boost::filesystem::is_directory (fullPath))
		{
			return;
		}

		boost::filesystem::directory_iterator end_iter;
		for (boost::filesystem::directory_iterator file_itr (fullPath); file_itr != end_iter; ++file_itr)
		{
			if (!boost::filesystem::is_directory (*file_itr) && (boost::filesystem::extension (*file_itr) == ext || ext == ""))
			{
#if BOOST_VERSION > 104000
				std::string str = make_path (dir, file_itr->path ().filename ().string ());
#else
				std::string str = make_path (dir, file_itr->path ().filename ());
#endif
				files.emplace_back (str);
			}
		}
	}

	//�ж��ļ��Ƿ����
	bool file_exist (const std::string& file)
	{
		boost::filesystem::path fullpath (file);
		return boost::filesystem::exists (fullpath);
	}

	//�ж��Ƿ�ΪĿ¼
	bool is_directory (const std::string &dir)
	{
		boost::filesystem::path fullPath (boost::filesystem::initial_path ());
		fullPath = boost::filesystem::system_complete (boost::filesystem::path (dir));

		if (boost::filesystem::exists (fullPath) && boost::filesystem::is_directory (fullPath))
		{
			return true;
		}
		return false;
	}

	bool copy_file (const std::string& to_dir, const std::string& file, bool overlay)
	{
		boost::filesystem::path fullpath (to_dir);
		if (!boost::filesystem::exists (fullpath))
			return false;

		if (file_exist (file))
		{
			std::string file_name = get_name (file);
			if (!overlay)
			{
				if (file_exist (to_dir + "/" + file_name))
					return false;
			}
			boost::filesystem::copy_file (boost::filesystem::path (file), boost::filesystem::path (to_dir + "/" + file_name));
			return true;
		}
		return false;
	}

	//������
	void rename (const std::string& new_name, const std::string& old_name)
	{
		boost::filesystem::rename (boost::filesystem::path (old_name), boost::filesystem::path (new_name));
	}

	//ɾ���ļ�
	void remove_file (const std::string& filename)
	{
		if (file_exist (filename))
		{
			boost::filesystem::remove (boost::filesystem::path (filename));
			std::cout << "'" << filename << "' is removed!";
		}
		else
		{
			std::cout << filename << " doesn't exist!";
		}
	}

	//ɾ��Ŀ¼
	void remove_dir (const std::string& dir)
	{
		if (is_directory (dir))
		{
			boost::filesystem::remove_all (boost::filesystem::path (dir));
			std::cout << "Directory '" << dir << "' is removed!";
		}
		else
		{
			std::cout << dir << " is not a directory!";
		}
	}

	void getRandomColor (int& r, int& g, int& b)
	{
		std::srand (int (time (0)));
		omp_lock_t lock;
		omp_init_lock (&lock);
		omp_set_lock (&lock);
		static int last_num = 0;
		int num = 0;
		while (1)
		{
			num = rand () % 12;
			if (num != last_num)
				break;
		}
		last_num = num;
		omp_unset_lock (&lock);
		omp_destroy_lock (&lock);
		switch (num)
		{
		case 0:
		{
			r = 255;
			g = 0;
			b = 0;
			break;
		}
		case 1:
		{
			r = 0;
			g = 255;
			b = 0;
			break;
		}
		case 2:
		{
			r = 0;
			g = 0;
			b = 255;
			break;
		}
		case 3:
		{
			r = 255;
			g = 255;
			b = 0;
			break;
		}
		case 4:
		{
			r = 255;
			g = 0;
			b = 255;
			break;
		}
		case 5:
		{
			r = 0;
			g = 255;
			b = 255;
			break;
		}
		case 6:
		{
			r = 72;
			g = 61;
			b = 139;
			break;
		}
		case 7:
		{
			r = 0;
			g = 191;
			b = 255;
			break;
		}
		case 8:
		{
			r = 0;
			g = 100;
			b = 0;
			break;
		}
		case 9:
		{
			r = 85;
			g = 107;
			b = 47;
			break;
		}
		case 10:
		{
			r = 255;
			g = 215;
			b = 0;
			break;
		}
		case 11:
		{
			r = 139;
			g = 69;
			b = 19;
			break;
		}
		case 12:
		{
			r = 160;
			g = 32;
			b = 240;
			break;
		}
		default:
		{
			r = 255;
			g = 255;
			b = 255;
			break;
		}
		}
	}
}