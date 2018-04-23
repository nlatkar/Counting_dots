
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string>  
#include <iostream> 
#include <sstream>

using namespace cv;
using namespace std;

/*Global Variables*/
Mat src; Mat src_gray;
cv::Mat diceROI;
cv::Mat diceROI2;
cv::Mat diceROI3;
int value1, value2, value3 , sum1 =0;


/* Function headers*/
void bounding_crop (void );
void display(Mat image, Mat image2);
int count_dots(Mat image);

/* @function main */
int main( int argc, char** argv )
{
  
  src = imread( "dice6.png", 1 );     // load the input image

  
  cvtColor( src, src_gray, CV_BGR2GRAY );  // Convert image to gray and blur it
  blur( src_gray, src_gray, Size(3,3) );

  
  
  char* source_window = "Source";			// Create Window for source image
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  imshow( source_window, src );			// Display source image	

 /* Function to detect Dice */
 bounding_crop();     
 
 /* Function to Dispaly Output */
 display(src,src_gray);
 
   


  waitKey(0);
  return(0);
}

/* @function thresh_callback */
void bounding_crop()
{
  Mat threshold_output;      // threshold image
  
  int count =0;

 
  vector<vector<Point> > contours;    
  vector<Vec4i> hierarchy;

  // Detect edges using Threshold
  threshold( src_gray, threshold_output, 220, 255, THRESH_BINARY );
  // Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  // Approximate contours to polygons + get bounding rects 
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
 
 
 
 
 /* Loop 1 to detect Rectangles of Dice 1 */
  for( int i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = boundingRect( Mat(contours_poly[i]) );
       
       
       if (cv::contourArea(contours[i]) > 1000 && count ==0 ){
       
      count =1;
      
      
      cv::Rect diceBoundsRect = boundRect[i];
        diceROI = src_gray(diceBoundsRect);
     
        std::ostringstream ss;
 
     }

  }
	/* Loop 2 to detect Rectangles of Dice 2 */
 for( int i = 1; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = boundingRect( Mat(contours_poly[i]) );
       
       if (cv::contourArea(contours[i]) > 1000 && count ==1 ){
       
      count =2;
      cv::Rect diceBoundsRect2 = boundRect[i];
        diceROI2 = src_gray(diceBoundsRect2);
  
     }
     
   }
    
    /* Stroe values of dots in dice 1 and 2 */
     value1 = count_dots(diceROI);
     value2 = count_dots(diceROI2);

 
}

/* count the number of dots */
int count_dots(Mat image){
		
		
// Setup SimpleBlobDetector parameters.
SimpleBlobDetector::Params params; 

	
	params.minThreshold = 10;    
	params.maxThreshold = 200;
	 
	params.filterByCircularity = true;
    params.minCircularity = 0.6;
 

    params.filterByInertia = true;
    params.minInertiaRatio = 0.7;

 SimpleBlobDetector detector(params);
	 
	 // Storage for blobs
	std::vector<KeyPoint> keypoints;
	
	// Detect blobs
	detector.detect( image, keypoints);

	// return number of pips
	return keypoints.size();
	 
}
		 
/* Dispaly and store the output */	 
void display(Mat image, Mat image2){
	
	// output image
	 Mat output;   
	 
			
	// invert the image for better results		 
	double alpha = 0.5; double beta;
	cv::Mat invSrc =  cv::Scalar::all(255) - image2;
	cv::threshold(invSrc, invSrc, 150, 255, cv::THRESH_BINARY | CV_THRESH_OTSU );

	 // taking canny output and drawing the contours
	 Mat canny_output;
	 vector<vector<Point> > contours;
	 vector<Vec4i> hierarchy;
	  
	 Canny( invSrc, canny_output, 255, 255*2, 3 );
	  
	 findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
	
	 Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	 
	 // loop to browse through the contours and draw them
	 for( size_t i = 0; i< contours.size(); i++ )
	     {
			
	       Scalar color = Scalar( 0, 128, 0 );
	       drawContours( drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
	       
	     }

	
	 beta = ( 1.0 - alpha );
	 addWeighted( image, alpha, drawing, beta, 0.0, output);
	
	   std::ostringstream ss;
	   std::ostringstream val1;
	   std::ostringstream val2;
	   std::ostringstream val3;
	   
	   val1 << value1;
	   val2 << value2;
	  
	   int sum =  count_dots(image2);
	   ss << sum;
	 
	 // Logic for finding dots in Dice 3
	 if ((value1 + value2) != sum){
		 
		 value3 = sum - (value1 + value2); 
		 }
		 
		 val3 << value3;
		 
	 // Putting text on the image
	 cv::putText(output, 
	            "Sum :" + ss.str() + " Dice1 :" + val1.str() + " Dice2 :" + val2.str() + " Dice3 :" + val3.str() ,
	            cv::Point(50,50), // Coordinates
	            cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
	            1.0, // Scale. 2.0 = 2x bigger
	            cv::Scalar(0,128,0), // Color
	            1, // Thickness
	            8); // Anti-alias
	
	 // Displaying the image
	 imshow( "Contours", output );
	 
	 // Stroing the output image
	 vector<int> compression_params;
	 compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
     compression_params.push_back(9);
     imwrite("Output.png", output, compression_params);
    
   
		
		 
			 }
	 
		 
		 
 
