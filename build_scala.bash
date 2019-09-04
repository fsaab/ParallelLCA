#move compiled c++ to jar location, build jar
root=/app/
module=$1

rm -rf /tmp/LCAKernel_0.1/

calculatorproject=UBUBI-LCA-Calculator-Native 
echo " compiling .jar based on .so from:" 
echo $root/$calculatorproject/src/main/c++/Native/$module/libLCAKernel.so 
cp $root/$calculatorproject/src/main/c++/Native/$module/libLCAKernel.so  $root/$calculatorproject/src/main/resources/lib/Linux/amd64/ 

sbt clean package 