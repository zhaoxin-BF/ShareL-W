#!/bin/sh
#mountusb.sh
#退出程序函数
quit()
{
	clear
	echo "*********************************************"
	echo "*         Thank you to use,Good Bye!        *"
	echo "*********************************************"
	exit 0
}
#加载U盘函数
mountusb()
{
	clear
	#在/mnt下创建usb目录
	mkdir /mnt/usb
	#查看U盘设备名称
	/sbin/fdisk -1 | grep /dev/sd
	echo -e "Please Enter teh device name of usb as shown above:\c"
	read PARAMETER
	mount /dev/$ PARAMETER /mnt/usb
}
#卸载U盘函数
umountusb()
{
	clear
	umount /mnt/usb
}
#显示U盘信息函数
display()
{
	clear
	ls -l /mnt/usb
}
#拷贝硬盘文件到U盘函数
cpdisktousb()
{
	clear
	echo -e "Please Enter teh filename to be Copied(under Current directory):\c"
	read FILE
	echo "Copying,Please wait!..."
	cp $FILE /mnt/usb
}
#拷贝U盘文件到硬盘函数
cpusbtodisk()
{
	clear
	echo -e "Please Enter teh filename to be Copied in USB:\c"
	read FILE
	echo "Copying,Please wait!..."
	cp /mnt/usb/$ FILE .
}

clear
while true
do
	echo "============================================="
	echo "***        UNIX USB MANAGE PROGRAM        ***"
	echo "============================================="
	echo "           1-MOUNT USB                       "
	echo "           2-UMOUNT USB                      "
	echo "           3-DISPLAY USB INFORMATION         "
	echo "           4-COPY FILE IN DISK TO USB        "
	echo "           5-COPY FILE IN USB TO DISK        "
	echo "           0-EXIT                            "
	echo "============================================="
	echo -e "Please Enter a Choice (0--5):\c"
	read CHOICE
	case $CHOICE in
	1) mountusb;;
	2) umountusb;;
	3) display;;
	4) cpdisktousb;;
	5) cpusbtodisk;;
	0) quit;;
	*) echo "Invalid Choice! Correct Choice is (0--5)"
	   sleep 4
	   clear;;
	esac
done