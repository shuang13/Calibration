int list[5][4] = {
		{0, 0, 0, 255},
		{64, 0, 255, 255},
		{127, 0, 255, 0},
		{191, 255, 255, 0},
		{255, 255, 0, 0}
	};

	int grad[256][3] = {};
	for (int i = 0; i < 256; i ++) {
		for (int j = 0; j < 4; j ++) {
			if (i >= list[j][0] && i <= list[j + 1][0]){
				grad[i][0] = list[j][1] + double(list[j + 1][1] - list[j][1]) * i/ (double(list[j + 1][0] - list[j][0]));
				grad[i][1] = list[j][2] + double(list[j + 1][2] - list[j][2]) * i/ (double(list[j + 1][0] - list[j][0]));
				grad[i][2] = list[j][3] + double(list[j + 1][3] - list[j][3]) * i/ (double(list[j + 1][0] - list[j][0]));
			}

		}
	}
	// 渲染
	for(int i = 0; i < w; i++) {
		for(int j = 0; j < h; j++) {
			int L = pixelMatrix[i][j];
			img->setPixel(NMODiv_gridDiv.x + i, NMODiv_gridDiv.y + j, qRgb(grad[L][0], grad[L][1], grad[L][2]));
		}
	}