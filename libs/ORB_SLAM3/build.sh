echo "Configuring and building Thirdparty/DBoW2 ..."

cd Thirdparty/DBoW2
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_IGNORE_PATH="/usr/local"
make -j

cd ../../g2o

echo "Configuring and building Thirdparty/g2o ..."

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_IGNORE_PATH="/usr/local"
make -j

cd ../../Sophus

echo "Configuring and building Thirdparty/Sophus ..."

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_IGNORE_PATH="/usr/local"
make -j

cd ../../../

echo "Uncompress vocabulary ..."

cd Vocabulary
tar -xf ORBvoc.txt.tar.gz
cd ..

echo "Configuring and building ORB_SLAM3 ..."

mkdir build
cd build
# Force use of system OpenCV 4.5.4 (compatible with ROS2) by ignoring /usr/local
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=../../install \
  -DCMAKE_IGNORE_PATH="/usr/local"
make -j4
