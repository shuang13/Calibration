#include <QVector>
/***
** 功能：gama校正
** 参数：
** 			input: 像素矩阵input[width][height]
**      gama： 伽马值，值越大越暗
** 返回：无
***/
void gamaJustifiy(QVector<QVector<int>> & input, float gama) 
{
	int width = input.size();
	int height = input[0].size();
	for(int i = 0; i < height; i ++) {
		for(int j = 0; j < width; j ++) {
			float L = input[j][i] / 255.0;
			input[j][i] = std::pow(L, gama) * 255;
		}
	}
}