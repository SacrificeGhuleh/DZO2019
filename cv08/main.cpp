#include "pch.h"
struct RLDUserData {
  cv::Mat & src_8uc3_img;
  cv::Mat & undistorted_8uc3_img;
  int K1;
  int K2;
  
  RLDUserData( const int K1, const int K2, cv::Mat & src_8uc3_img, cv::Mat & undistorted_8uc3_img ) : K1( K1 ), K2( K2 ), src_8uc3_img( src_8uc3_img ), undistorted_8uc3_img( undistorted_8uc3_img ) {
  
  }
};


void geom_dist( cv::Mat & src_8uc3_img, cv::Mat & dst_8uc3_img, bool bili, double K1 = 1.0, double K2 = 1.0 )
{
}



void apply_rld( int id, void * user_data )
{
  RLDUserData *rld_user_data = (RLDUserData*)user_data;
  
  geom_dist( rld_user_data->src_8uc3_img, rld_user_data->undistorted_8uc3_img, !false, rld_user_data->K1 / 100.0, rld_user_data->K2 / 100.0 );
  cv::imshow( "Geom Dist", rld_user_data->undistorted_8uc3_img );
}

int main()
{
  cv::Mat src_8uc3_img, geom_8uc3_img;
  RLDUserData rld_user_data( 3.0, 1.0, src_8uc3_img, geom_8uc3_img );
  
  src_8uc3_img = cv::imread( "images/distorted_window.jpg", cv::IMREAD_COLOR );
  if ( src_8uc3_img.empty() )
  {
    printf( "Unable to load image!\n" );
    exit( -1 );
  }
  
  cv::namedWindow( "Original Image" );
  cv::imshow( "Original Image", src_8uc3_img );
  
  geom_8uc3_img = src_8uc3_img.clone();
  apply_rld( 0, (void*)&rld_user_data );
  
  cv::namedWindow( "Geom Dist" );
  cv::imshow( "Geom Dist", geom_8uc3_img );
  
  cv::createTrackbar( "K1", "Geom Dist", &rld_user_data.K1, 100, apply_rld, &rld_user_data );
  cv::createTrackbar( "K2", "Geom Dist", &rld_user_data.K2, 100, apply_rld, &rld_user_data );
  
  cv::waitKey( 0 );
  
  return 0;
}

