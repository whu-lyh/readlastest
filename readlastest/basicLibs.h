#pragma once
/*
	Copyright (c) 2020 WuHan University. All Rights Reserved
	First create: 2020/02/01 23:25:25
	Detail: parsing trajectory basics
	Author: liyuhao
	Email: yhaoli@whu.edu.cn
*/

#pragma warning(disable: 4267)
#pragma warning(disable: 4819)
#pragma warning(disable: 4273)

#ifndef NOMINMAX
#define  NOMINMAX
#endif

#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define  GLOG_NO_ABBREVIATED_SEVERITIES
#endif


#ifdef _DEBUG
#pragma comment(lib, "liblas.lib")

#pragma comment(lib, "pcl_common_debug.lib")
#pragma comment(lib, "pcl_io_debug.lib")
#pragma comment(lib, "glogd.lib")

#pragma comment(lib, "libboost_thread-vc141-mt-gd-1_64.lib")

#else
#pragma comment(lib, "liblas.lib")

#pragma comment(lib, "pcl_common_release.lib")
#pragma comment(lib, "pcl_io_release.lib")

#pragma comment(lib, "glog.lib")

#pragma comment(lib, "libboost_thread-vc141-mt-1_64.lib")
#endif

#pragma execution_character_set("utf-8")

#include <Windows.h>
#include <glog/logging.h>
#include <list>
#include <set>
#include <omp.h>
#include <fstream>

#include <iomanip>
#include <liblas/header.hpp>
#include <liblas/reader.hpp>
#include <liblas/writer.hpp>
#include <liblas/factory.hpp>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <glog/logging.h>
#include <pcl/registration/transformation_estimation_svd.h>

#include <boost/make_shared.hpp>
