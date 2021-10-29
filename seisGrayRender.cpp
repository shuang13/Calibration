#include <QVector>
#include <QPointF>

/***
** 功能：地震道波形图灰度渐变渲染显示方法
** 参数：
** 			pixelMatrix: 像素矩阵pixelMatrix[width][height]
**			timeStart、timeEnd： 数据时域范围
**			depthStart、depthEnd： 数据深度域范围
**			vData： 道集数据
**			vTraceDepth: 每一道的深度
**			timeInterval： 时间采样率ms
**			gain: 增益
**      maxValue： vData中最大值
**      grayFlag： 灰度透明模式
**      lineFlag： 波形线显示
**      polar： 极性，true为向下，false为向上
** 返回：无
** 例子：seisGrayRender(pixelMatrix, timeStart, timeEnd, depthStart, depthEnd, m_vFilterData, vTraceDepth, timeInterval, gain, m_vFilterMaxValue,true, true, true);
***/
void CDrawWidget::seisGrayRender(QVector<QVector<int>> & pixelMatrix, 
	float timeStart, float timeEnd, 
	float depthStart, float depthEnd, 
	QVector<QVector<float>> & vData, QVector<float> & vTraceDepth, 
	float timeInterval, float gain, float maxValue, 
	bool grayFlag, bool lineFlag, bool polar)
{
  QVector<QVector<float>> data(vData); 
	QVector<float> traceDepth(vTraceDepth); 
	bool lineSteepFlag = false;
	float depthLen = depthEnd - depthStart;
	int timeLen = timeEnd - timeStart;
	int traceNum = traceDepth.size();
	int width = pixelMatrix.size();
	int height = pixelMatrix[0].size();
	width = width % 2 == 1 ? width - 1 : width;
	timeLen = timeLen % 2 == 0 ? timeLen - 1 : timeLen;
	
	QVector<float> vTraceY;
	vTraceY.resize(traceNum);
	for(int i = 0; i < traceNum; i ++)
	{
		//int traceTemp = int (vTraceDepth[i] / 5 + 0.5) * 5;
		vTraceY[i] = height * (vTraceDepth[i] - depthStart) / depthLen;
	}
	QVector<int> vTraceYPixel;
	QVector<int> vTraceD;
	vTraceYPixel.clear();
	vTraceD.clear();
	int v = 0;
	int distance = 0;
	int denseFlag = 0;
	int tempD = 0;
	for(int i = 0; i < traceNum - 1; i ++) {
		tempD = ceil(vTraceY[i + 1] - vTraceY[i]);
		if(tempD == 1) {
			denseFlag = 1;
		}
	}
	vTraceYPixel.push_back(0);
	vTraceD.push_back(ceil(vTraceY[1] - vTraceY[0]) + denseFlag);
	for(int i = 1; i < traceNum; i ++)
	{ 
		
		int trace1 = vTraceY[v] + 0.5;
		int trace2 = vTraceY[i] + 0.5;
		int d2 = trace2 - trace1;

		int d = ceil(vTraceY[i] - vTraceY[v]) + denseFlag;
		//int d = abs(vTraceY[i] - vTraceY[v] - d2) > 0.02 ? ceil(vTraceY[i] - vTraceY[v] + denseFlag) : (vTraceY[i] - vTraceY[v] + 0.5  + denseFlag);
		if(d > 5) {
			lineSteepFlag = true;
		}
		if(d2 < d) {
			
			if(distance > 0) {
				
				vTraceYPixel.push_back(i);
				vTraceD.push_back(d);
				distance = distance - (d - d2);
			} else {
				distance = distance + d2;
			}
				
		} else if(d2 == d) {
			vTraceYPixel.push_back(i);
			vTraceD.push_back(d);
		}
		v = i;
	}
	vTraceYPixel.push_back(traceNum - 1);
	vTraceD.push_back(ceil(vTraceY[traceNum - 1] - vTraceY[traceNum - 2]) + denseFlag);
	QVector<QVector<float>> tData;
	QVector<int> dTraceY;
	traceNum = vTraceYPixel.size();
	tData.resize(traceNum);
	dTraceY.resize(traceNum);
	for(int i = 0; i < traceNum; i ++) {
		tData[i] = vData[vTraceYPixel[i]];
	}
	dTraceY[0] = vTraceY[0];
	for(int i = 0; i < traceNum - 1; i ++)
	{
		dTraceY[i + 1] = dTraceY[i] + vTraceD[i];
	}

	int traceIndexStart = 0;
	int traceIndexEnd = traceNum - 1;

	for(int i = traceIndexStart; i < traceNum; i++) {
		if(dTraceY[i] >= 0) {
			traceIndexStart = i;
			break;
		}
	}
	for(int i = traceIndexEnd; i >= 0; i--) {
		if(dTraceY[i] <= height) {
			traceIndexEnd = i;
			break;
		}
	}
	int showTraceNum = traceIndexEnd - traceIndexStart + 1;
	float traceInterval = gain * height / (float)showTraceNum;
	int timeIndexStart = timeStart / timeInterval;
	int timeIndexEnd = timeEnd / timeInterval;

	
	int pixelMax = 0;
	int b = 1;
	if( width * timeInterval / (float) timeLen < 1) {
		b = ceil(timeLen / (timeInterval  * width));
	}
	
	for(int i = traceIndexStart; i < traceIndexEnd; i ++)
	{
		for(int j = timeIndexStart; j < timeIndexEnd - b; j = j + b)
		{	 
			float x1 = width * (j * timeInterval - timeStart) / (float) timeLen;
			float x2 = width * ((j + b) * timeInterval - timeStart) / (float) timeLen;
			float y1 = dTraceY[i] + traceInterval * tData[i][j] / (float) maxValue;
			float y2 = dTraceY[i] + traceInterval * tData[i][j + b] / (float) maxValue;
			/*float beforeY = 0;
			if(i >= 1) {
			beforeY = dTraceY[i - 1];
			}*/
			
			if(x1 < 0 || x2 > width || (y1 + dTraceY[i]) < 0) continue;
			for(int x = x1 + 0.5; x < x2 - 0.5; x ++)
			{
				int y = y1 + (y2 - y1) * (x - x1) / (x2 - x1) + 0.5;
				//if((y -  dTraceY[i]) < 0) continue;

				// 扫描线填充
				if(polar) {
					for(int n = dTraceY[i]; n < y; n ++)
					{
						int xi = x;
						int yi = n;
						if(xi < 0 || xi >= width || yi < 0 || yi >= height) {
							continue;
						}
						pixelMatrix[xi][yi] ++;
						int c = pixelMatrix[xi][yi];
						if(c > pixelMax) {
							pixelMax = c;
						}
					}
				} else {
					for(int n = dTraceY[i]; n > y; n --)
					{
						int xi = x;
						int yi = n;
						if(xi < 0 || xi >= width || yi < 0 || yi >= height) {
							continue;
						}
						pixelMatrix[xi][yi] ++;
						int c = pixelMatrix[xi][yi];
						if(c > pixelMax) {
							pixelMax = c;
						}
					}
				}
				
			}
			if(!lineFlag) continue;
			bool steep = false; 

			if(lineSteepFlag) {
				if (std::abs(x1-x2)<std::abs(y1-y2)) { 
					std::swap(x1, y1); 
					std::swap(x2, y2); 
					steep = true; 
				} 
				if (x1>x2) { 
					std::swap(x1, x2); 
					std::swap(y1, y2); 
				}
			}
			int dx = x2 - x1; 
			int dy = y2 - y1; 
			int derror2 = std::abs(dy)*2; 
			int error2 = 0; 
			int y = y1; 
			for (int x = x1 + 0.5; x < x2 - 0.5; x++) { 
				y = y1 + (y2 - y1) * (x - x1) / (x2 - x1) + 0.5;
				//if(y1 + (y2 - y1) * (x - x1) / (x2 - x1) < beforeY) y = beforeY + 0.5;
				int xi = x;
				int yi = y;
				if(lineSteepFlag) {
					if (steep) {
						xi = y;
						yi = x;
					}
				}
				if(xi < 0 || xi >= width || yi < 0 || yi >= height) {
					continue;
				}
				pixelMatrix[xi][yi] ++;
				if(pixelMatrix[xi][yi] > pixelMax) {
					pixelMax = pixelMatrix[xi][yi];
				}
				error2 += derror2; 
				if (error2 > dx) { 
					y += (y2 > y1 ? 1 : -1); 
					error2 -= dx * 2; 
				} 
			}
		}
	}

	// 渲染像素矩阵
	float L; // 亮度
	for(int i = 0; i < height; i ++) {
		for(int j = 0; j < width; j ++) {
			int value = pixelMatrix[j][i];
			if(value == 0) {
				L = 1;
			}  else{
				L = ((float) value - 1)/ pixelMax;

			}
			//L = 1-((float) value)/ pixelMax;

			if(grayFlag) {
				pixelMatrix[j][i] = L * 255;
			} else {
				if(L < 1) {
					pixelMatrix[j][i] = 0;
				} else {
					pixelMatrix[j][i] = 255;
				}
			}
			
		}
	}
}