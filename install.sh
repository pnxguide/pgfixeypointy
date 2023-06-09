# echo 'Installing libfixeypointy';
# cd third_party/libfixeypointy;
# sh install.sh;
# cd ../..;

echo 'Installing pgfixeypointy.so';
make clean;
sudo make install;
