#include "pch.h"

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */
void fillEquationSides(cv::Mat &leftEqMat, cv::Mat &rightEqMat, const double (&transformData)[8][9]);

void fillPerspectiveTransformMatrix(const cv::Mat &in, cv::Mat &out);

void fillTransformDataMatrix(const cv::Vec2d (&flagCoords)[4], const cv::Vec2d (&buildingCoords)[4],
                             double (&transformData)[8][9]);

int main(void) {
  /**
   * Load image.
   */
  const cv::Mat srcVsb = cv::imread("./images/vsb.jpg", cv::IMREAD_COLOR);
  const cv::Mat srcFlag = cv::imread("./images/flag.png", cv::IMREAD_COLOR);
  
  cv::Mat dstResult = srcVsb.clone();
  /**
   * Check, if the image is empty.
   */
  if (srcVsb.empty() || srcFlag.empty() || dstResult.empty()) {
    std::cout << "WTF" << std::endl;
    return -1;
  }
  
  // To implement the perspective transform, you can use the following
  // coordinates to define the correspondence between each point in the
  // transform (staring from the top left corner in clockwise order):
  // Flag:  (0,0), (323,0), (323,215), (0,215)
  // Building:  (69,107), (227,76), (228,122), (66,134)
  const cv::Vec2d flagCoords[4] = {{0,   0},
                                   {323, 0},
                                   {323, 215},
                                   {0,   215}};
  
  const cv::Vec2d buildingCoords[4] = {{69,  107},
                                       {227, 76},
                                       {228, 122},
                                       {66,  134}};
  
  
  cv::Mat leftEqMat,              /// @brief Left equation mat
      rightEqMat,                 /// @brief Right equation mat
      resultEqMat,                /// @brief Result equation mat
      perspectiveTransformMat;    /// @brief Perspective transform mat
  
  double transformData[8][9];
  fillTransformDataMatrix(flagCoords, buildingCoords, transformData);
  
  fillEquationSides(leftEqMat, rightEqMat, transformData);
  cv::solve(leftEqMat, rightEqMat, resultEqMat);
  
  fillPerspectiveTransformMatrix(resultEqMat, perspectiveTransformMat);
  
  cv::Mat XYMatrixBuildingMat(3, 1, CV_64F);
  cv::Mat perspTransformFlagMat(3, 1, CV_64F);
  
  for (int row = 0; row < dstResult.rows; row++) {
    for (int col = 0; col < dstResult.cols; col++) {
      XYMatrixBuildingMat.at<double>(0) = col;
      XYMatrixBuildingMat.at<double>(1) = row;
      XYMatrixBuildingMat.at<double>(2) = 1;
      
      perspTransformFlagMat = perspectiveTransformMat * XYMatrixBuildingMat;
      perspTransformFlagMat /= perspTransformFlagMat.at<double>(2); // divide by "z"
      
      int transformedX = static_cast<int>(perspTransformFlagMat.at<double>(0));
      int transformedY = static_cast<int>(perspTransformFlagMat.at<double>(1));
      
      if (transformedX > 0 && transformedY > 0 && transformedX < srcFlag.cols && transformedY < srcFlag.rows) {
        dstResult.at<cv::Vec3b>(row, col) = srcFlag.at<cv::Vec3b>(transformedY, transformedX);
      }
    }
  }
  
  cv::imshow("Vsb original", srcVsb);
  cv::imshow("Flag original", srcFlag);
  cv::imshow("Result image", dstResult);
  cv::waitKey();
}

void fillTransformDataMatrix(const cv::Vec2d (&flagCoords)[4], const cv::Vec2d (&buildingCoords)[4],
                             double (&transformData)[8][9]) {
  //Fill this crazy matrix according to the guide at DZO webpage.
  for (int I = 0; I < 4; I++) {
    transformData[I * 2][0] = buildingCoords[I][1];   //yI
    transformData[I * 2][1] = 1;
    transformData[I * 2][2] = 0;
    transformData[I * 2][3] = 0;
    transformData[I * 2][4] = 0;
    transformData[I * 2][5] = -flagCoords[I][0] *     //-x~I *
                              buildingCoords[I][0];   // xI
    transformData[I * 2][6] = -flagCoords[I][0] *     //-x~I *
                              buildingCoords[I][1];   // yI
    transformData[I * 2][7] = -flagCoords[I][0];      // -x~I
    transformData[I * 2][8] = buildingCoords[I][0];    // -xI
    
    
    transformData[I * 2 + 1][0] = 0;
    transformData[I * 2 + 1][1] = 0;
    transformData[I * 2 + 1][2] = buildingCoords[I][0];   //xI
    transformData[I * 2 + 1][3] = buildingCoords[I][1];   //yI
    transformData[I * 2 + 1][4] = 1;
    transformData[I * 2 + 1][5] = -flagCoords[I][1] *     //-y~I *
                                  buildingCoords[I][0];   //xI
    transformData[I * 2 + 1][6] = -flagCoords[I][1] *     //-y~I *
                                  buildingCoords[I][1];   //yI
    transformData[I * 2 + 1][7] = -flagCoords[I][1];      //-y~I
    transformData[I * 2 + 1][8] = 0;
    
  }
}


void fillEquationSides(cv::Mat &leftEqMat, cv::Mat &rightEqMat, const double (&transformData)[8][9]) {
  //Split array of doubles into mats, to be filled into solve method.
  //Copy 8x9 matrix into 8x8 and 8x1 matrix
  leftEqMat = cv::Mat(8, 8, CV_64F);
  rightEqMat = cv::Mat(8, 1, CV_64F);
  
  for (int y = 0; y < 8; ++y) {
    for (int x = 0; x < 8; ++x) {
      leftEqMat.at<double>(y, x) = transformData[y][x];
    }
    
    rightEqMat.at<double>(y) = -transformData[y][8];
  }
}

void fillPerspectiveTransformMatrix(const cv::Mat &in, cv::Mat &out) {
  out = cv::Mat(3, 3, CV_64F);
  for (int i = 0; i < 9; i++) {
    if (i == 0) {
      out.at<double>(i) = 1;
      continue;
    }
    out.at<double>(i) = in.at<double>(i - 1);
  }
}

