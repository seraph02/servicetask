sudo apt install gcc g++ libgoogle-glog-dev libcurl4-openssl-dev libjsoncpp-dev libprotobuf-dev autoconf automake libtool cmake pkg-config libmicrohttpd-dev libssl-dev adb -y
cd elasticlient/external/cpr
mkdir build
cd build
cmake ..
make -j24
sudo make install
cd ../../../.../






sudo apt install gcc g++ libgoogle-glog-dev libcurl4-openssl-dev libjsoncpp-dev libprotobuf-dev openjdk-11-jdk
echo "* soft nofile 65536" >> /etc/security/limits.conf
echo "* hard nofile 65536" >> /etc/security/limits.conf
echo "* soft nproc 4096" >> /etc/security/limits.conf
echo "* hard nproc 4096" >> /etc/security/limits.conf
echo "root soft nofile 65536" >> /etc/security/limits.conf
echo "root hard nofile 65536" >> /etc/security/limits.conf
echo "root soft nproc 4096" >> /etc/security/limits.conf
echo "root hard nproc 4096" >> /etc/security/limits.conf

echo "vm.max_map_count=262144" >> /etc/sysctl.conf

/etc/rc.local
#!/bin/sh
sysctl -p
service supervisor start


