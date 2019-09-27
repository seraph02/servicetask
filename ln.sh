rm -rf a* f* i* m* q* s* t* v* w* y*
runname="../POC/Control.jar"
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > fbmcontrol
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > imocontrol
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > voxercontrol
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > telegramcontrol

runname="../POC/NetFace.jar"
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > telegram
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > whatsapp
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > wechat
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > viber
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > line
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > imo

echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > voxer
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > yixin
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > meetup
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > qq
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > quora
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > skype
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > tumblr
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > twitter
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > firechat
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > messenger
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > alipay

echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > dropbox
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > instagram
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > taobao
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > hotmail
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > gmail
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > yahoo
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > outlook
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > shenxun
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > facebook

runname="../POC/jxlsexport.jar"
echo 'java -Dfile.encoding=utf-8 -jar  '$runname' $*' > export