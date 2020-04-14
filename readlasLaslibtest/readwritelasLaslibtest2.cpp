#pragma once

#if _DEBUG
#pragma comment(lib,"LASlib_d.lib")
#else
#pragma comment(lib,"LASlib.lib")
#endif

// c/c++
#include <iostream>
#include <stdlib.h>
#include <string.h>

// Laslib
#include "laswriter.hpp"
#include "lasreader.hpp"

int main1 () {

	// 写出流
	LASwriteOpener laswriteopener_;
	laswriteopener_.set_file_name ("./youruser.las");

	// 是否创建成功
	if (!laswriteopener_.active ()) {
		std::cout << "Failed!\n";
		return 0;
	}

	//// 初始化头
	LASheader *lasheader_ = new LASheader ();
	lasheader_->x_scale_factor = 1.0;
	lasheader_->y_scale_factor = 1.0;
	lasheader_->z_scale_factor = 1.0;
	lasheader_->x_offset = 0.0;
	lasheader_->y_offset = 0.0;
	lasheader_->z_offset = 0.0;
	lasheader_->point_data_format = 1;
	lasheader_->point_data_record_length = 28;

	// 初始化点
	LASpoint *laspoint_ = new LASpoint ();
	laspoint_->init (lasheader_, lasheader_->point_data_format, lasheader_->point_data_record_length, 0);

	// 写点云
	LASwriter* laswriter_ = laswriteopener_.open (lasheader_);
	if (laswriter_ == 0) {
		std::cout << "Failed!\n";
		return 0;
	}

	for (int i = 0; i < 1000; i++) {
		laspoint_->set_X (i);
		laspoint_->set_Y (i);
		laspoint_->set_Z (i);
		laspoint_->set_intensity ((U16)i);
		laspoint_->set_gps_time (0.0006*i);
		laspoint_->rgb [0] = laspoint_->rgb [1] = laspoint_->rgb [2] = U16_QUANTIZE (((laspoint_->get_z () - lasheader_->min_z)*65535.0) / (lasheader_->max_z - lasheader_->min_z));

		laswriter_->write_point (laspoint_);
		laswriter_->update_inventory (laspoint_);
	}

	// 关闭流
	laswriter_->update_header (lasheader_, TRUE);
	laswriter_->close ();
	//delete laswriter_;

	LASreadOpener lasreadopener;
	lasreadopener.set_file_name ("F:/Data/EstoniaNew/N6mme_testarea/20190717_082048_LD5_grab_log_VLP-16_XYZ_ypr-1_-38_-0.5_0.1_0.23_0.535_19-07-17.las");
	LASreader* lasreader = lasreadopener.open ();

	LASwriteOpener laswriteopener;
	laswriteopener.set_file_name ("compressed.las");

	LASwriter* laswriter = laswriteopener.open (&lasreader->header);

	while (lasreader->read_point ())
	{
		laswriter->write_point (&lasreader->point);
	}

	laswriter->close ();
	delete laswriter;

	lasreader->close ();
	delete lasreader;

	system ("pause");
	return 0;
}