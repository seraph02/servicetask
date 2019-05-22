
sudo su
sudo apt install gcc g++ libgoogle-glog-dev libcurl4-openssl-dev libjsoncpp-dev libprotobuf-dev autoconf automake libtool cmake pkg-config libmicrohttpd-dev libssl-dev adb openjdk-11-jdk libopencv-dev apache2 libboost-all-dev protobuf-compiler -y
sudo apt install gcc g++ libgoogle-glog-dev libcurl4-openssl-dev libjsoncpp-dev libprotobuf-dev openjdk-11-jdk libopencv-dev apache2 -y
echo "* soft nofile 65536" >> /etc/security/limits.conf
echo "* hard nofile 65536" >> /etc/security/limits.conf
echo "* soft nproc 4096" >> /etc/security/limits.conf
echo "* hard nproc 4096" >> /etc/security/limits.conf
echo "root soft nofile 65536" >> /etc/security/limits.conf
echo "root hard nofile 65536" >> /etc/security/limits.conf
echo "root soft nproc 4096" >> /etc/security/limits.conf
echo "root hard nproc 4096" >> /etc/security/limits.conf

echo "vm.max_map_count=262144" >> /etc/sysctl.conf


echo '#!/bin/sh
sysctl -p
service supervisor start' > /etc/rc.local

ln -s /home/twd/source/build /var/www/html/r
ln -s /home/twd/source/POC /var/www/html/s
