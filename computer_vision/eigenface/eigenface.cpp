// eigenface.c, by Robin Hewitt, 2007
//
// Example program showing how to implement eigenface with OpenCV

// Usage:
//
// First, you need some face images. I used the ORL face database.
// You can download it for free at
//    www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html
//
// List the training and test face images you want to use in the
// input files train.txt and test.txt. (Example input files are provided
// in the download.) To use these input files exactly as provided, unzip
// the ORL face database, and place train.txt, test.txt, and eigenface.exe
// at the root of the unzipped database.
//
// To run the learning phase of eigenface, enter
//    eigenface train
// at the command prompt. To run the recognition phase, enter
//    eigenface test

#include <vector>
#include <map>
#include <iostream>
#include <stdio.h>
#include <cvaux.h>
#include <highgui.h>
#include <image_raii.hpp>
#include <window_raii.hpp>
#include <face_detect.hpp>
#include <eigenface.hpp>

using namespace std;

//// Constants
const int SCALE_SIZE = 100;
const int MIN_NEIGHBOR = 2;
const int FLAG = 0;
const int Y_VALUE = 121;
const string TEST_FILE = "test.txt";
const string TRAIN_FILE = "train.txt";
const string TRAIN_DATA = "facedata.xml";

//// Global variables
IplImage ** faceImgArr        = 0; // array of face images
CvMat    *  personNumTruthMat = 0; // array of person numbers
int nTrainFaces               = 0; // the number of training images
int nEigens                   = 0; // the number of eigenvalues
IplImage * pAvgTrainImg       = 0; // the average image
IplImage ** eigenVectArr      = 0; // eigenvectors
CvMat * eigenValMat           = 0; // eigenvalues
CvMat * projectedTrainFaceMat = 0; // projected training faces
vector< pair< int, int > > mapping;// mapping of face indexes to image/face pair
vector< vector< int > > backward_mapping;// mapping of face indexes to image/face pair
vector< pair< IplImage *, string > > original_images;// keyed by original image
vector< CvRect > rectangles;     // keyed by face indexes
vector< int > numFaces;            // how many faces in each image
int nImages = 0;

//////////////////////////////////
// main()
//
int main( int argc, char** argv )
{
	// validate that an input was specified
	if( argc != 2 )
	{
		printUsage();
		return -1;
	}

	if( !strcmp(argv[1], "train") ) learn();
	else if( !strcmp(argv[1], "test") ) recognize();
	else
	{
		printf("Unknown command: %s\n", argv[1]);
		printUsage();
	}

	return 0;
}


//////////////////////////////////
// learn()
//
void learn()
{
	int i, offset;

	// load training data
	nTrainFaces = loadFaceImgArrayLearn( TRAIN_FILE );
	if( nTrainFaces < 2 )
	{
		fprintf(stderr,
		        "Need 2 or more training faces\n"
		        "Input file contains only %d\n", nTrainFaces);
		return;
	}

	// do PCA on the training faces
	doPCA();

	// project the training images onto the PCA subspace
	projectedTrainFaceMat = cvCreateMat( nTrainFaces, nEigens, CV_32FC1 );
	offset = projectedTrainFaceMat->step / sizeof(float);
	for(i=0; i<nTrainFaces; i++)
	{
		//int offset = i * nEigens;
		cvEigenDecomposite(
			faceImgArr[i],
			nEigens,
			eigenVectArr,
			0, 0,
			pAvgTrainImg,
			//projectedTrainFaceMat->data.fl + i*nEigens);
			projectedTrainFaceMat->data.fl + i*offset);
	}

	// store the recognition data as an xml file
	storeTrainingData();
}


//////////////////////////////////
// recognize()
//
void recognize()
{
	int i, nTestFaces  = 0;         // the number of test images
	CvMat * trainPersonNumMat = 0;  // the person numbers during training
	float * projectedTestFace = 0;

	// load test images and ground truth for person number
	nTestFaces = loadFaceImgArrayTest( TEST_FILE );
	printf("%d test faces loaded\n", nTestFaces);

	// load the saved training data
	if( !loadTrainingData( &trainPersonNumMat ) ) return;

    std::vector< int > labels;
	// project the test images onto the PCA subspace
	projectedTestFace = (float *)cvAlloc( nEigens*sizeof(float) );
	for(i=0; i<nTestFaces; i++)
	{
		int iNearest, nearest, truth, picture_index, face_index;
		double distance;

		// project the test image onto the PCA subspace
		cvEigenDecomposite(
			faceImgArr[i],
			nEigens,
			eigenVectArr,
			0, 0,
			pAvgTrainImg,
			projectedTestFace);

		pair< int, int > nearest_pair = findNearestNeighbor(projectedTestFace);
		iNearest = nearest_pair.first;
		distance = nearest_pair.second;
		truth    = personNumTruthMat->data.i[i];
		nearest  = trainPersonNumMat->data.i[iNearest];
		picture_index = mapping[i].first;
		face_index = mapping[i].second;

        labels.push_back( nearest );

		cout << distance << endl;
        
        printf( "pindex = %d, findex = %d, nearest = %d, Truth = %d, Distance = %f\n", picture_index, face_index, nearest, truth, distance );
	}

    cout << "before initialization\n";
    cout << "mapping size : " << mapping.size() << endl;

    vector< vector< pair< int, CvRect > > > label_rect_pairs;
    cout << "after initialization\n";
    FaceDetect fd;
    // initialize map
    int face_counter = 0;
    for( i = 0; i < nImages; i++ )
    {
        vector< pair< int, CvRect > > lr;
        for( int j = 0; j < numFaces[i]; j++ )
        {
            lr.push_back( make_pair( labels[face_counter], rectangles[face_counter] ) );
            face_counter++;
        }

        label_rect_pairs.push_back( lr );
    }

    /*
    for( i = 0; i < nTestFaces; i++ )
    {
        int image_index = mapping[i].first;
        cout << image_index << endl;
        label_rect_pairs[image_index].push_back( make_pair( labels[i], rectangles[i] ) );
    }
    */

    for( int image_index = 0; image_index < label_rect_pairs.size(); image_index++ )
    {
        std::vector< string > dir_parts = tokenize_str( original_images[image_index].second, "/" );
        std::vector< string > filename_parts = tokenize_str( dir_parts[dir_parts.size() - 1], "." );
        string filename_output = "images/" + filename_parts[0] + "_label." + filename_parts[1];
        cout << filename_output << endl;

        fd.detect_and_label( original_images[image_index].first, label_rect_pairs[image_index], filename_output );
    }
}


//////////////////////////////////
// loadTrainingData()
//
int loadTrainingData(CvMat ** pTrainPersonNumMat)
{
	CvFileStorage * fileStorage;
	int i;

	// create a file-storage interface
	fileStorage = cvOpenFileStorage( TRAIN_DATA.c_str(), 0, CV_STORAGE_READ );
	if( !fileStorage )
	{
		fprintf(stderr, "Can't open facedata.xml\n");
		return 0;
	}

	nEigens = cvReadIntByName(fileStorage, 0, "nEigens", 0);
	nTrainFaces = cvReadIntByName(fileStorage, 0, "nTrainFaces", 0);
	*pTrainPersonNumMat = (CvMat *)cvReadByName(fileStorage, 0, "trainPersonNumMat", 0);
	eigenValMat  = (CvMat *)cvReadByName(fileStorage, 0, "eigenValMat", 0);
	projectedTrainFaceMat = (CvMat *)cvReadByName(fileStorage, 0, "projectedTrainFaceMat", 0);
	pAvgTrainImg = (IplImage *)cvReadByName(fileStorage, 0, "avgTrainImg", 0);
	eigenVectArr = (IplImage **)cvAlloc(nTrainFaces*sizeof(IplImage *));
	for(i=0; i<nEigens; i++)
	{
		char varname[200];
		sprintf( varname, "eigenVect_%d", i );
		eigenVectArr[i] = (IplImage *)cvReadByName(fileStorage, 0, varname, 0);
	}

	// release the file-storage interface
	cvReleaseFileStorage( &fileStorage );

	return 1;
}


//////////////////////////////////
// storeTrainingData()
//
void storeTrainingData()
{
	CvFileStorage * fileStorage;
	int i;

	// create a file-storage interface
	fileStorage = cvOpenFileStorage( TRAIN_DATA.c_str(), 0, CV_STORAGE_WRITE );

	// store all the data
	cvWriteInt( fileStorage, "nEigens", nEigens );
	cvWriteInt( fileStorage, "nTrainFaces", nTrainFaces );
	cvWrite(fileStorage, "trainPersonNumMat", personNumTruthMat, cvAttrList(0,0));
	cvWrite(fileStorage, "eigenValMat", eigenValMat, cvAttrList(0,0));
	cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0,0));
	cvWrite(fileStorage, "avgTrainImg", pAvgTrainImg, cvAttrList(0,0));
	for(i=0; i<nEigens; i++)
	{
		char varname[200];
		sprintf( varname, "eigenVect_%d", i );
		cvWrite(fileStorage, varname, eigenVectArr[i], cvAttrList(0,0));
	}

	// release the file-storage interface
	cvReleaseFileStorage( &fileStorage );
}


//////////////////////////////////
// findNearestNeighbor()
//
pair< int, double > findNearestNeighbor(float * projectedTestFace)
{
	//double leastDistSq = 1e12;
	double leastDistSq = DBL_MAX;
	int i, iTrain, iNearest = 0;

	for(iTrain=0; iTrain<nTrainFaces; iTrain++)
	{
		double distSq=0;

		for(i=0; i<nEigens; i++)
		{
			float d_i =
				projectedTestFace[i] -
				projectedTrainFaceMat->data.fl[iTrain*nEigens + i];
			distSq += d_i*d_i / eigenValMat->data.fl[i];  // Mahalanobis
			//distSq += d_i*d_i; // Euclidean
		}

		if(distSq < leastDistSq)
		{
			leastDistSq = distSq;
			iNearest = iTrain;
		}
	}

	return make_pair( iNearest, leastDistSq );
}


//////////////////////////////////
// doPCA()
//
void doPCA()
{
	int i;
	CvTermCriteria calcLimit;
	CvSize faceImgSize;

	// set the number of eigenvalues to use
	nEigens = nTrainFaces-1;

	// allocate the eigenvector images
	faceImgSize.width  = faceImgArr[0]->width;
	faceImgSize.height = faceImgArr[0]->height;
	eigenVectArr = (IplImage**)cvAlloc(sizeof(IplImage*) * nEigens);
	for(i=0; i<nEigens; i++)
		eigenVectArr[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

	// allocate the eigenvalue array
	eigenValMat = cvCreateMat( 1, nEigens, CV_32FC1 );

	// allocate the averaged image
	pAvgTrainImg = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

	// set the PCA termination criterion
	calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, nEigens, 1);

	// compute average image, eigenvalues, and eigenvectors
	cvCalcEigenObjects(
		nTrainFaces,
		(void*)faceImgArr,
		(void*)eigenVectArr,
		CV_EIGOBJ_NO_CALLBACK,
		0,
		0,
		&calcLimit,
		pAvgTrainImg,
		eigenValMat->data.fl);

	cvNormalize(eigenValMat, eigenValMat, 1, 0, CV_L1, 0);
}

//////////////////////////////////
// loadFaceImgArray()
//
int loadFaceImgArrayLearn(string filename)
{
	FILE * imgListFile = 0;
	char imgFilename[512];
	int iFace, nFaces=0;

	// open the input file
	if( !(imgListFile = fopen(filename.c_str(), "r")) )
	{
		fprintf(stderr, "Can\'t open file %s\n", filename.c_str());
		return 0;
	}

	// count the number of images
	while( fgets(imgFilename, 512, imgListFile) ) ++nFaces;
	rewind(imgListFile);

	// allocate the face-image array and person number matrix
	faceImgArr        = (IplImage **)cvAlloc( nFaces*sizeof(IplImage *) );
	personNumTruthMat = cvCreateMat( 1, nFaces, CV_32SC1 );

	// store the face images in an array
	for( iFace=0; iFace<nFaces; iFace++ )
	{
		// read person number and name of image file
		fscanf(imgListFile,	"%d %s", personNumTruthMat->data.i+iFace, imgFilename);

		// load the image
		faceImgArr[iFace] = cvLoadImage(imgFilename, CV_LOAD_IMAGE_GRAYSCALE);

		if( !faceImgArr[iFace] )
		{
			fprintf(stderr, "Can\'t load image from %s\n", imgFilename);
			return 0;
		}
	}

	fclose(imgListFile);

	return nFaces;
}

int loadFaceImgArrayTest(string filename, bool select_face )
{
	FILE * imgListFile = 0;
	char imgFilename[512];
	int iFace, nFaces=0;
	int iImage = 0;

	// open the input file
	if( !(imgListFile = fopen(filename.c_str(), "r")) )
	{
		fprintf(stderr, "Can\'t open file %s\n", filename.c_str());
		return 0;
	}

	// count the number of images
	while( fgets(imgFilename, 512, imgListFile) ) ++nImages;
	rewind(imgListFile);

	FaceDetect fd;
	vector< vector< pair< IplImage *, CvRect > > > faces;
	vector< int > truth;
	int * truthNum = new int;

	// store the face images in an array
	for( iImage=0; iImage<nImages; iImage++ )
	{
		// read person number and name of image file
		fscanf(imgListFile, "%d %s", truthNum, imgFilename);

		truth.push_back( *truthNum );

		// load the image
		ImageRAII image(imgFilename);
		vector< pair< IplImage *, CvRect > > image_faces = fd.extract_faces( image.image, SCALE_SIZE, MIN_NEIGHBOR, FLAG );
		if( select_face )
			nFaces++;
		else
			nFaces += image_faces.size();
		faces.push_back( image_faces );
        original_images.push_back( make_pair( cvCloneImage( image.image ), string( imgFilename ) ) );
	}
	delete truthNum;
	truthNum = NULL;

	fclose(imgListFile);

	// allocate the face-image array and person number matrix
	faceImgArr        = (IplImage **)cvAlloc( nFaces*sizeof(IplImage *) );
	personNumTruthMat = cvCreateMat( 1, nFaces, CV_32SC1 );

	int face_counter = 0;
	for( iImage = 0; iImage < nImages; iImage++ )
	{
		int num_faces = faces[iImage].size();
        numFaces.push_back( num_faces );
        backward_mapping.push_back( vector< int >() );
		cout << "New image\n";

		for( iFace = 0; iFace < num_faces; iFace++ )
		{
            faceImgArr[face_counter] = faces[iImage][iFace].first;
            *(personNumTruthMat->data.i+face_counter) = truth[iImage];
            mapping.push_back( make_pair( iImage, iFace ) );
            backward_mapping[iImage].push_back( face_counter );
            rectangles.push_back( faces[iImage][iFace].second );

            face_counter++;
		}
	}

	return nFaces;
}

//////////////////////////////////
// printUsage()
//
void printUsage()
{
	printf("Usage: eigenface <command>\n",
	       "  Valid commands are\n"
	       "    train\n"
	       "    test\n");
}
