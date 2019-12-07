#include "pch.h"

template<class T>
inline T sqr(const T &val) {
  return val * val;
}

template<class T>
inline T deg2rad(const T &val) {
  return (val * M_PI) / (T) 180;
}

template<class T>
inline T rad2deg(const T &val) {
  return (val * static_cast<T>(180)) / static_cast<T>(M_PI);
}


struct RLDUserData {
  cv::Mat &src_8uc3_img;
  cv::Mat &undistorted_8uc3_img;
  int T1;
  int T2;
  
  RLDUserData(const int T1, const int T2, cv::Mat &src_8uc3_img, cv::Mat &undistorted_8uc3_img) :
      T1(T1), T2(T2),
      src_8uc3_img(
          src_8uc3_img),
      undistorted_8uc3_img(
          undistorted_8uc3_img) {
    
  }
};

void geom_dist(const cv::Mat &src_8uc3_img, cv::Mat &dst_8uc3_img, int& T1, int& T2) {
  if(T2 <= T1){
    T2 = T1 - 1;
  }
  for (int row = 0; row < src_8uc3_img.rows; row++) {
    for (int col = 0; col < src_8uc3_img.cols; col++) {
      const uint8_t loc_pix = src_8uc3_img.at<uint8_t>(row, col);
      
      const float f_x = (src_8uc3_img.at<uint8_t>(row, col - 1) - src_8uc3_img.at<uint8_t>(row, col + 1)) / 2.;
      const float f_y = (src_8uc3_img.at<uint8_t>(row - 1, col) - src_8uc3_img.at<uint8_t>(row + 1, col)) / 2.;
      const float alpha = ((f_x != 0.0) || (f_y != 0.0)) ? rad2deg(atan2f(f_y, f_x)) : 0;
      
      
      if (((alpha > -22.5) && (alpha <= 22.5)) ||
          ((alpha > 157.5) && (alpha <= -157.5))) {
        dst_8uc3_img.at<uint8_t>(row, col) = 0;
      } else if (((alpha > 22.5) && (alpha <= 67.5)) ||
                 ((alpha > -157.5) && (alpha <= -112.5))) {
        dst_8uc3_img.at<uint8_t>(row, col) = 45;
      } else if (((alpha > 67.5) && (alpha <= 112.5)) ||
                 ((alpha > -112.5) && (alpha <= -67.5))) {
        dst_8uc3_img.at<uint8_t>(row, col) = 90;
      } else if (((alpha > 112.5) && (alpha <= 157.5)) ||
                 ((alpha > -67.5) && (alpha <= -22.5))) {
        dst_8uc3_img.at<uint8_t>(row, col) = 135;
      }
    }
  }
}


void apply_rld(int id, void *user_data) {
  RLDUserData *rld_user_data = (RLDUserData *) user_data;
  
  geom_dist(rld_user_data->src_8uc3_img, rld_user_data->undistorted_8uc3_img, rld_user_data->T1,
            rld_user_data->T2);
  cv::imshow("Edge thinning", rld_user_data->undistorted_8uc3_img);
}

int main() {
  cv::Mat src_8uc3_img, geom_8uc3_img;
  RLDUserData rld_user_data(3.0, 1.0, src_8uc3_img, geom_8uc3_img);
  
  src_8uc3_img = cv::imread("images/valve.png", cv::IMREAD_GRAYSCALE);
  if (src_8uc3_img.empty()) {
    printf("Unable to load image!\n");
    exit(-1);
  }
  
  geom_8uc3_img = src_8uc3_img.clone();
  apply_rld(0, (void *) &rld_user_data);
  
  cv::namedWindow("Edge thinning");
  cv::imshow("Edge thinning", geom_8uc3_img);
  
  cv::createTrackbar("T1", "Edge thinning", &rld_user_data.T1, 255, apply_rld, &rld_user_data);
  cv::createTrackbar("T2", "Edge thinning", &rld_user_data.T2, 255, apply_rld, &rld_user_data);
  
  cv::waitKey(0);
  
  return 0;
}

