mount -o remount,rw /system

vi /etc/init/deploy.rc
on property:sys.boot_completed=1
        start deploy_autorun


service deploy_autorun /vendor/bin/sh /vendor/bin/vendor_deploy.sh
        class main
        user root
        group root
        disabled
        oneshot
vi /system/bin/deploystart.sh
#!/bin/sh
sleep 5
svc power stayon true
nohup /data/data/ru.meefik.linuxdeploy/files/bin/linuxdeploy -p linux start -m >/dev/null 2>&1 &

vi /system/vendor/bin/vendor_deploy.sh
#!/vendor/bin/sh
/system/bin/sh /system/bin/deploystart.sh


echo "service.adb.tcp.port=5555" >> /system/build.prop



mount -o remount,ro /system

