#include <iostream>
#include <fstream>
#include <string>
#include <liblas/liblas.hpp>

using namespace std;

int main2 ()
{
	/**************liblas��д�����ļ�����*****************/
 //�����洢�ļ�
	ofstream outPt ("mylas.las", ios::out | ios::binary);
	if (!outPt.is_open ())
	{
		return 0;
	}
	// �����ļ�ͷ����������ʽ���������ӡ�ƫ����
	liblas::Header header;
	header.SetVersionMajor (1);
	header.SetVersionMinor (2);
	header.SetDataFormatId (liblas::PointFormatName::ePointFormat3);
	header.SetScale (0.001, 0.001, 0.001);
	liblas::Writer writer (outPt, header);
	liblas::Point point (&header);
	// �м���д����Ƽ���¼��ĸ���
	for (int j = 0; j < 100; ++j)
	{
		double x = 100 * rand () / (RAND_MAX + 1.0f);
		double y = 100 * rand () / (RAND_MAX + 1.0f);
		double z = 100 * rand () / (RAND_MAX + 1.0f);
		point.SetCoordinates (x, y, z);
		writer.WritePoint (point);
	}
	double minPt [3] = { 9999999, 9999999, 9999999 };
	double maxPt [3] = { 0, 0, 0 };
	header.SetPointRecordsCount (100);
	header.SetPointRecordsByReturnCount (0, 100);
	header.SetMax (maxPt [0], maxPt [1], maxPt [2]);
	header.SetMin (minPt [0], minPt [1], minPt [2]);
	writer.SetHeader (header);
	//outPt.close();

	system ("pause");
	return 1;
}