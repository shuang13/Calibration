#include <QVector>
/***
** 功能：直方图均衡化
** 参数：
** 			input: 像素矩阵input[width][height]
** 返回：无
***/

void equalize(QVector<QVector<int>> & input)
{
	int gray[256] = { 0 };  //记录每个灰度级别下的像素个数
	double gray_prob[256] = { 0 };  //记录灰度分布密度
	double gray_distribution[256] = { 0 };  //记录累计密度
	int gray_equal[256] = { 0 };  //均衡化后的灰度值

	int gray_sum = 0;  //像素总数
	int width = input.size();
	int height = input[0].size();

	gray_sum = width * height;

	//统计每个灰度下的像素个数
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int vaule = input[i][j];
			gray[vaule]++;
		}
	}

	//统计灰度频率
	for (int i = 0; i < 256; i++)
	{
		gray_prob[i] = ((double)gray[i] / gray_sum);
	}

	//计算累计密度
	gray_distribution[0] = gray_prob[0];
	for (int i = 1; i < 256; i++)
	{
		gray_distribution[i] = gray_distribution[i-1] +gray_prob[i];
	}

	//重新计算均衡化后的灰度值，四舍五入。参考公式：(N-1)*T+0.5
	for (int i = 0; i < 256; i++)
	{
		gray_equal[i] = (uchar)(255 * gray_distribution[i] + 0.5);
	}


	//直方图均衡化,更新原图每个点的像素值
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			input[i][j] = gray_equal[input[i][j]];
		}
	}
}
