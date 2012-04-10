#include "TakeTestPrincipal.h"

char* trainingPath = "D:/CameraCalibrations/colorTraining/";
int imgId = 1;

XnPoint3D initPoint;

TakeTestPrincipal::TakeTestPrincipal(void)
{
}


TakeTestPrincipal::~TakeTestPrincipal(void)
{
}


void selectROI_callBack(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN) 
	{
		IplImage* img = (IplImage*)param;
		if (initPoint.X == 0)
		{
			initPoint.X = x;
			initPoint.Y = y;
		}
		else
		{
			int width = x - initPoint.X;
			int height = y - initPoint.Y;
			cvSetImageROI(img, cvRect(initPoint.X, initPoint.Y, width, height));

			char filePath[80];
			strcpy(filePath,trainingPath);
			strcat(filePath, "img");
			char* strNimg = new char[20];
			itoa(imgId, strNimg, 10);
			strcat(filePath, strNimg);
			strcat(filePath, ".jpg");

			cvSaveImage(filePath, img);
			cvResetImageROI(img);
			imgId++;
			initPoint.X = 0;


		}

	}
}



int main()
{
	initPoint.X = 0;
	CameraProperties cam1, cam2;
	Utils::rgbdInit(&cam1, &cam2);

	IplImage *rgbImg;
	const XnRGB24Pixel *rgbMap;	

	int option;
	cout << "Which camera will be used (1/2)? ";
	cin >> option;
	Sleep(2000);
	if (option == 1)
	{
		cam1.getContext()->StartGeneratingAll();
		cam1.getContext()->WaitAndUpdateAll();	 
		rgbMap = cam1.getImageNode()->GetRGB24ImageMap();
		cam1.getContext()->StopGeneratingAll();
	}
	else if (option == 2)
	{
		cam2.getContext()->StartGeneratingAll();
		cam2.getContext()->WaitAndUpdateAll();	
		rgbMap = cam2.getImageNode()->GetRGB24ImageMap();
		cam2.getContext()->StopGeneratingAll();
	}
	else
	{
		cout << "Error: You should choose 1 or 2" << endl;
		exit(1);
	}
///////////////////////////////////////////////////////////////////////
	directory_iterator end ;
    for( directory_iterator iter(trainingPath) ; iter != end ; ++iter )
      if ( !is_directory( *iter ) )
	  {
		  cout << "File " << imgId << ": " << iter->path().filename() << endl;
		  imgId++;
	  }

///////////////////////////////////////////////////////////////////////
	
	rgbImg = cvCreateImage(cvSize(640,480), 8, 3);

	for (int y=0; y<XN_VGA_Y_RES; y++)
	{
		uchar *ptr = (uchar*)rgbImg->imageData + y*rgbImg->widthStep;
		for (int x=0; x<XN_VGA_X_RES; x++)
		{
			ptr[3*x] = rgbMap->nBlue;
			ptr[3*x + 1] = rgbMap->nGreen;
			ptr[3*x + 2] = rgbMap->nRed;
			rgbMap++;
		}
	}
	cvNamedWindow("rgb",1);
	cvSetMouseCallback("rgb", selectROI_callBack, (IplImage*)rgbImg);
		

	cvShowImage("rgb", rgbImg);
	cvWaitKey(0);

	


}