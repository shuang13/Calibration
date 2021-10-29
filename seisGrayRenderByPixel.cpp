void seisGrayRenderByPixel(
  QVector<QVector<float>> & data,
  QVector<SDrawTraceInfo> & drawTraceInfo, 
  int width, 
  int height, 
  int pixel_x0, 
  int pixel_y0, 
  int timeStart, 
  int timeEnd, 
  int depthStart, 
  int depthEnd, 
  float timeInterval, 
  QImage * img)
{

	float timeLen = timeEnd - timeStart;
	float depthLen = depthEnd - depthStart;
	int grid_size = 16;
	float pixel_width = (float) timeLen / width;
	float pixel_height = (float) depthLen / height;

	int addNum = grid_size * width / timeLen;
	int addDepthNum = grid_size * height / depthLen;
	
	QVector<QVector<float>> addData;
	QVector<float> temp_addData;

	if(addNum % 2 == 1) {
		addNum --;
	}
	//addNum = 0;
	timeInterval = (float) timeInterval / addNum;
	for(int i = 0; i < data.size(); i ++) {
		temp_addData.clear();
		for(int j = 0; j < data[i].size() - 1; j ++) {
			for(int m = 0; m < addNum; m ++) {
				temp_addData.push_back((data[i][j + 1] - data[i][j]) / addNum * m + data[i][j]);
			}
		}
		addData.push_back(temp_addData);
	}
	//addData = data;
	// 像素矩阵初始化
	QVector<QVector<short int>> pixel_light;
	QVector<short int> temp_pixel_light;

	for(int i = 0; i < height; i ++) {
		temp_pixel_light.push_back(0);
	}
	for(int i = 0; i < width; i ++) {
		pixel_light.push_back(temp_pixel_light);
	}

	
	int max_L = 0;
	QVector<int> coverTraceIndex;

	for(int i = 0; i < drawTraceInfo.size() - 1; i ++) {
		// 判断重叠道
		float d1 = drawTraceInfo[i].depth;
		float d2 = drawTraceInfo[i + 1].depth;
		float offset = (d2 - d1) / (depthEnd - depthStart) * height;
		if(offset < 1) {
			coverTraceIndex.push_back(drawTraceInfo[i].depth);
		}
		for(int j = 0; j < addData[i].size(); j ++) {
			int len = addData[i][j] * addDepthNum;
			pair<float, float> p;
			pair<float, float> p2;
			for(int v = 0; v < (float)timeInterval / timeLen * width; v ++) {
				for(float m = 0; m < len; m ++) {
					p = make_pair(timeInterval * j + timeStart, drawTraceInfo[i].depth +  (float) m / addDepthNum);
				
					int pixel_x_index = (p.first - timeStart) / timeLen * width;

					int pixel_y_index = (p.second - depthStart) / depthLen * height;

					if(pixel_x_index < 0 || pixel_x_index >= pixel_light.size() || pixel_y_index < 0 || pixel_y_index >= pixel_light[i].size()) {
						break;
					}
				
					for(int u = 0; u < (float)1.0 / depthLen * height; u ++) {

						pixel_light[pixel_x_index + v][pixel_y_index + u] ++;
					}
				
				

					int c = pixel_light[pixel_x_index][pixel_y_index];

					if(c > max_L) {
						max_L = c;
					}
				}
			}

			//p = make_pair(timeInterval * j + timeStart, drawTraceInfo[i].depth + addData[i][j]);
			//int pixel_x_index = (p.first - timeStart) / timeLen * width;

			//int pixel_y_index = (p.second - depthStart)/ depthLen * height;
			//if(pixel_x_index < 0 || pixel_x_index >= pixel_light.size() || pixel_y_index < 0 || pixel_y_index >= pixel_light[i].size()) {
				//continue;
			//}

			//pixel_light[pixel_x_index][pixel_y_index] ++;
		}
	}

	float L;
	for(int i = 0; i < height; i ++) {
		for(int j = 0; j < width; j ++) {
			int coverLight = pixel_light[j][i];
			float k = 1 - (float) coverLight / max_L;
			float gama = pow(k, (float)2.5);
			L =  gama * 255;
			img->setPixel(pixel_x0 + j, pixel_y0 + i, qRgb(L, L, L));
		}
	}
	
}