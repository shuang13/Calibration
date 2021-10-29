#include <QVector>
#include <QPointF>
/***
** 功能：走廊数据镶嵌到地震数据里
** 参数：
** 			mData:  镶嵌数据
**			sData： 地震数据
**			sTraceDepth： 地震数据每一道深度
**			sTimeInterval: 地震数据时间采样率ms
**			cData： 走廊数据
**			cTraceDepth:  走廊数据每一道深度
**			cTimeInterval: 走廊数据时间采样率ms
**			cutLine： 裁切线
** 返回：无
***/
void mergeSeisFilter(
	QVector<QVector<float>> & mData, 
	QVector<QVector<float> >  & sData, 
	QVector<float> & sTraceDepth, 
	float sTimeInterval,
	QVector<QVector<float> >  & cData, 
	QVector<float> & cTraceDepth, 
	float cTimeInterval,
	QVector<QPointF> cutLine)
{
	int sTraceLen = sTraceDepth.size();
	float sDepthInterval = std::abs(sTraceDepth[1] - sTraceDepth[0]);
	int cTraceLen = cTraceDepth.size();
	QVector<QPointF> cutPointIndex;
	QVector<QPointF> cutPointIndex2;
	// 边界判断
	if(cutLine[0].x() / sTimeInterval < 0 || cutLine[cutLine.size() - 1].x() / sTimeInterval > sData[0].size()) return;

	// 裁切线首尾横向延长
	if(cutLine[0].x() / sTimeInterval > 0) {
		cutLine.push_front(QPointF(0, cutLine[0].y()));
	}
	if(cutLine[cutLine.size() - 1].x() / sTimeInterval < sData[0].size()) {
		cutLine.push_back(QPointF(sData[0].size() * sTimeInterval, cutLine[cutLine.size() - 1].y()));

	}
	// 裁切线插值
	bool insertSpaceFlag = true;
	//insertSpaceFlag = false;
	int spaceTraceLen = 0; // 空道数量

	if(insertSpaceFlag) {
		spaceTraceLen = 3;
	}

	for(int i = 0; i < cutLine.size() - 1; i ++) {

		int x1 = cutLine[i].x() / sTimeInterval + 0.5;
		int y1 = (cutLine[i].y() - sTraceDepth[0]) / sDepthInterval + 0.5;
		int x2 = cutLine[i + 1].x() / sTimeInterval + 0.5;
		int y2 = (cutLine[i + 1].y() - sTraceDepth[0]) / sDepthInterval + 0.5;
		for(int j = x1; j < x2; j ++) {
			float k = (j - x1) / (float)(x2 - x1);
			int t = k * (y2 - y1) + y1;
			cutPointIndex.push_back(QPointF(t, j));
			cutPointIndex2.push_back(QPointF(t + cTraceDepth.size() + spaceTraceLen, j));
		}

	}
	// 道集初始置零
	QVector<float> mData_temp;
	mData_temp.resize(sData[0].size());

	for(int i = 0; i < sTraceDepth.size() + cTraceDepth.size() + spaceTraceLen; i ++) {
		mData.push_back(mData_temp);
	}

	// 向下平移填充镶嵌，非覆盖
	for(int i = 0; i < mData[0].size(); i ++) {
		int k = 0;
		int n = 0;
		for(int j = 0; j < mData.size(); j ++) {
			if(j <= cutPointIndex[i].x()) {
				mData[j][i] = sData[j][i];
			} else if(j > cutPointIndex[i].x() && j <= cutPointIndex2[i].x()) {
				if(insertSpaceFlag) {
					if(k == 0 || k == (cTraceDepth.size() / 2 + 1) || k == (cTraceDepth.size() + 2)) {
						mData[j][i] = 0;
						n ++;
					} else {
						mData[j][i] = cData[k - n][i * sTimeInterval / cTimeInterval];
					}
				} else {
					mData[j][i] = cData[k][i * sTimeInterval / cTimeInterval];
				}

				k ++;
			} else {
				mData[j][i] = sData[j - cTraceDepth.size() - spaceTraceLen][i];
			}
		}
	}
 }