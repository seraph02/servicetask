mount -o remount,rw /system

vi /etc/init/deploy.rc
on property:sys.boot_completed=1
        start deploy_autorun


on late-init
    user root
    group root
    nohup /data/data/ru.meefik.linuxdeploy/files/bin/linuxdeploy -p linux start -m >/dev/null 2>&1 &


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

mount -o remount,rw /system
echo "service.adb.tcp.port=5555" >> /system/build.prop
mount -o remount,ro /system





#连接成功后，依次输入下面命令以获取修改权限。

    mount -o remount,rw /system

    setenforce 0

复制代码

#使用vi修改webpadinit.sh文件。

    vi /system/bin/webpadinit.sh

复制代码

#操作方法：按i键进行编辑。找到下面这一行并修改，让Linux Deploy能够获得root权限。最新的版本可能不用改，大家可以试试。

    /system/xbin/supolicy --live "permissive init;permissive kernel;permissive untrusted_app;permissive toolbox;permissive shell;permissive priv_app;"

复制代码

#顺便加入linux deploy的自启动。

    sleep 5

    svc power stayon true

    nohup /data/data/ru.meefik.linuxdeploy/bin/linuxdeploy -p linux start -m >/dev/null 2>&1 &

复制代码

#修改完成后按一下ESC键，然后输入:wq!保存并退出。用es文件管理器也可以修改。
