cmake_minimum_required(VERSION 2.8)
project( Counting_Dots )
find_package( OpenCV REQUIRED )
add_executable( Counting_Dots Counting_Dots.cpp )
target_link_libraries( Counting_Dots ${OpenCV_LIBS} )



