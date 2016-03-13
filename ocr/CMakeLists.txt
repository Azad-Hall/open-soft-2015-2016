cmake_minimum_required(VERSION 2.8)
project( test )
SET(CMAKE_CXX_FLAGS "-std=c++0x")
set(CMAKE_BUILD_TYPE Debug)
find_package( OpenCV  REQUIRED )
#find_package(GSL REQUIRED)
add_executable( test test.cpp pugixml.cpp)
target_link_libraries( test ${OpenCV_LIBS} )

add_executable(remove-text remove-text.cpp pugixml.cpp)
target_link_libraries( remove-text ${OpenCV_LIBS} )

add_executable(ComputerVision ComputerVision.cpp pugixml.cpp)
target_link_libraries( ComputerVision ${OpenCV_LIBS} )

add_executable(graph-candidates  graph-candidates.cpp box-detection.cpp pugixml.cpp)
target_link_libraries( graph-candidates ${OpenCV_LIBS} )

add_executable(graph-box  graph-box.cpp box-detection.cpp pugixml.cpp)
target_link_libraries( graph-box ${OpenCV_LIBS} )

add_executable(legend-box  legend-box.cpp box-detection.cpp)
target_link_libraries( legend-box ${OpenCV_LIBS} )

# add_executable(box-detection  box-detection.cpp pugixml.cpp)
# target_link_libraries( box-detection ${OpenCV_LIBS} )

add_executable(preprocessing  preprocessing.cpp pugixml.cpp)
target_link_libraries( preprocessing ${OpenCV_LIBS} )


add_executable(orientation  orientation.cpp)
target_link_libraries( orientation ${OpenCV_LIBS} )

add_executable(xaxis-granularity  xaxis-granularity.cpp)
target_link_libraries( xaxis-granularity ${OpenCV_LIBS} )

add_executable(text_seg  text_seg.cpp box-detection.cpp pugixml.cpp)
target_link_libraries( text_seg ${OpenCV_LIBS} )

add_executable(color-segmentation peakdetect.cpp color-segmentation.cpp )
target_link_libraries( color-segmentation ${OpenCV_LIBS} )

add_executable(helper  helper.cpp box-detection.cpp pugixml.cpp)
target_link_libraries( helper ${OpenCV_LIBS} )

add_executable(scale  scale.cpp pugixml.cpp)
target_link_libraries( scale  )


add_executable(gen-table  gen-table.cpp pugixml.cpp)

target_link_libraries( gen-table ${OpenCV_LIBS} -lgsl -lgslcblas -lm )
#add_executable(skew-detection  skew-detection.cpp)
#target_link_libraries( skew-detection ${OpenCV_LIBS} -lgsl -lgslcblas -lm  )
