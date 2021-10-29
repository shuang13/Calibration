#include <QVector>
/***
** 功能：基于prewitt算子边缘检测生成浮雕
** 参数：
** 			input: 像素矩阵input[width][height]
** 返回：无
***/
void prewittRelief(QVector<QVector<int>> & input)
{
	int w = input.size();
	int h = input[0].size();
	QVector<QVector<int>> result;
	QVector<int> t;
	//左光源
	/*int b[5][5] = {
		{-1,-1,0,1,1},
		{-1,-1,0,1,1},
		{-1,-1,0,1,1},
		{-1,-1,0,1,1},
		{-1,-1,0,1,1}
	};
	//上光源
	int b2[5][5] = {
		{-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1},
		{0,0,0,0,0},
		{1,1,1,1,1},
		{1,1,1,1,1},

	};
	//斜射光源
	int b3[5][5] = {
		{-1,-1,-1,-1,0},
		{-1,-1,-1, 0,1},
		{-1,-1,0,1,1},
		{-1,0,1,1,1},
		{0,1,1,1,1}
	};*/
	// 3阶Prewitt 算子
	// int corner[3][3] = {
	// 	{-3, -2, 0},
	// 	{-2, 0, 2},
	// 	{0, 2, 3}
	// };
	
	// 5阶Prewitt 算子
	int corner[5][5] = {
		{-3, -3, -2, -1, 0},
		{-3, -3, -2, 0, 1},
		{-2, -2, 0, 2, 2},
		{-1, 0, 2, 3, 3},
		{0, 1, 2, 3, 3}
	};
	for(int j = 0; j < h; j ++) {
		t.push_back(0);
	}
	for(int i = 0; i < w; i ++) {
		result.push_back(t);
	}
	int cornerLen = corner.length;
	int half = cornerLen / 2;
	int x = 0;
	int y = 0;
	int r = 0;
	// 梯度检测法
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			r = result[i][j];
			for (int z = 0; z <  cornerLen; z++) {
				x = i + z - half;
				for (int q = 0; q <  cornerLen; q++) {
					y = j + q  - half;
					if((x < 0) || (y < 0) || (x >= w) || (y >= h)) continue;
					r += input[x][y] * corner[z][q];
				}
			}
			result[i][j] = 128 -  r / (cornerLen * cornerLen * 2);
		}
	}
	input = result;
}