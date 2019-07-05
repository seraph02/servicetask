src="192.168.1.49"
dst="192.168.1.86"
for index in $(curl -s 'http://'$src':9200/_cat/indices' | awk '{print $3}' | sort | uniq); do
echo $index
./esm -s 'http://'$src':9200' -x "$index" -y "$index"  -d 'http://'$dst':9200'
done
#./esm -s 'http://'$src':9200/' -x "src_index" -y "dest_index"  -d 'http://'$dst':9201'
