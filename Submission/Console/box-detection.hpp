// for convenience since i'm changing the prototypes a lot...
std::vector<std::vector<cv::Point> > getBoxes(Mat input, int minLineLength = 30, int houghLineThresh = 230, int houghMergeThresh = 20
  , double slopeThresh = 1e-2);
vector<Point> getRectangularContour(vector<Point> largest);
// experimental, not yet done. will need to finish this (or modify getRectangularContouur) because does not wokr
// whjen there is grid.
vector<Point> getRectangularContour2(vector<Point> largest);
vector<Point> shrinkContour(vector<Point> contour, double pix) ;