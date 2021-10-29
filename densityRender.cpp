
/***
** 功能：变密度图
** 参数：
** 			pixelMatrix: 像素矩阵pixelMatrix[width][height]
**			timeStart、timeEnd： 数据时域范围
**			depthStart、depthEnd： 数据深度域范围
**			vData： 道集数据
**			vTraceDepth: 每一道的深度
**			timeInterval： 时间采样率ms
**			gain: 增益
** 返回：无
***/
void densityRender(
	QVector<QVector<int>> & pixelMatrix, 
	int timeStart, 
	int timeEnd, 
	int depthStart, 
	int depthEnd, 
	QVector<QVector<float>> & vData, 
	QVector<float> & vTraceDepth, 
	float timeInterval, 
	float gain){
	int depthLen = depthEnd - depthStart;
	int timeLen = timeEnd - timeStart;
	int traceNum = vTraceDepth.size();
	int width = pixelMatrix.size();
	int height = pixelMatrix[0].size();
	width = width % 2 == 1 ? width - 1 : width;
	height = height % 2 == 1 ? height - 1 : height;

	QVector<float> vTraceY;
	vTraceY.resize(traceNum);

	for(int i = 0; i < traceNum; i ++)
	{
		vTraceY[i] = height * (vTraceDepth[i] - depthStart) / depthLen;
	}

	int traceIndexStart = 0;
	int traceIndexEnd = traceNum - 1;

	for(int i = traceIndexStart; i < traceNum; i++) {
		if(vTraceDepth[i] > depthStart) {
			traceIndexStart = i;
			break;
		}
	}
	for(int i = traceIndexEnd; i >= 0; i--) {
		if(vTraceDepth[i] < depthEnd) {
			traceIndexEnd = i;
			break;
		}
	}
	int showTraceNum = traceIndexEnd - traceIndexStart + 1;
	float traceInterval = gain * height / (float)showTraceNum;
	int timeIndexStart = timeStart / timeInterval;
	int timeIndexEnd = timeEnd / timeInterval;

	float maxValue = 0;
	float minValue = 0;
	for(int i = 0; i < vData.size(); i ++)
	{
		for(int j = 0; j < vData[i].size(); j ++)
		{
			if(maxValue < vData[i][j]) {
				maxValue = vData[i][j];
			}
			if(minValue > vData[i][j]) {
				minValue = vData[i][j];
			}
		}
	}

	for(int i = traceIndexStart; i < traceIndexEnd - 1; i ++)
	{
		for(int j = timeIndexStart; j < timeIndexEnd - 1; j ++)
		{	 
			CPoint3 p1 = CPoint3(vTraceY[i], width * (j * timeInterval - timeStart) / (float) timeLen, vData[i][j]);
			CPoint3 p4 = CPoint3(vTraceY[i], width * ((j + 1) * timeInterval - timeStart) / (float) timeLen, vData[i][j + 1]);
			CPoint3 p2 = CPoint3(vTraceY[i + 1], width * (j * timeInterval - timeStart) / (float) timeLen, vData[i + 1][j]);
			CPoint3 p3 = CPoint3(vTraceY[i + 1], width * ((j + 1) * timeInterval - timeStart) / (float) timeLen, vData[i + 1][j + 1]);
			if(p1.y() < 0) continue;
			if(p2.y() < 0) continue;
			

			if(p4.y() > width) continue;
			if(p3.y() > width) continue;

			for(int m = ceil(p1.y()); m <= floor(p4.y()); m ++)
			{
 				double tv = p1.z() + (m - p1.y()) * (p4.z() - p1.z())/(p4.y() - p1.y());
				double th = p1.x();
				double tv_1 = p2.z() + (m - p2.y()) * (p3.z() - p2.z())/(p3.y() - p2.y());
				double th_1 = p2.x();

				if((th_1 - th) == 0) break;
				for(int n = floor(th); n <= ceil(th_1); n ++)
				{
					double v = tv + (n - th) *(tv_1 - tv)/(th_1 - th);
					int L = (v-minValue)/(maxValue - minValue)*255;
					if(m < 0 || m >= width || n < 0 || n >= height) continue;
					if(L < 0) {
						L = 0;
					} else if(L > 255) {
						L = 255;
					}
					pixelMatrix[m][n] = L;
				}
			}	
		}
	}
}