#ifndef _EIGENFACE_H
#define _EIGENFACE_H

#include <cv.h>
#include <utility>
#include <string>

void learn();
void recognize();
void doPCA();
void storeTrainingData();
int  loadTrainingData(CvMat ** pTrainPersonNumMat);
std::pair< int, double >  findNearestNeighbor(float * projectedTestFace);
int  loadFaceImgArrayLearn(std::string filename);
int  loadFaceImgArrayTest(std::string filename, bool select_face = false );
void printUsage();

#endif
