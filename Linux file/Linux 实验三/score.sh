# score.sh
# !/bin/s

#colour 函数
colour(){
	#格式为背景;前景m，主要实现屏幕颜色显示
	case $1 in
		black_white) #背景为黑色,前景为白色
			echo -e "\033[40;37m"
			;;
		black_green) #背景为黑色，前景为绿色
			echo -e "\033[40;32m"
			;;
		black_cyan) #背景为黑色，前景为青色
			echo -e "\033[40;36m"
			;;
		red_yellow) #背景为红色，前景为黄色
			echo -e "\033[41;33m"
			;;
	esac
}

#查询函数
search(){
	colour black_white
	clear
	echo -e "Please Enter Name >>>\c"
	read NAME
	#如果无记录时
	if [ ! -f ./record ]; then
		echo "You must have some scores before you can search"
		sleep 2
		clear
		return
	fi

	grep -i "$NAME" ./record 2> /dev/null

	case "$?" in
		1) echo "Name not in record."
			;;
		2) echo "You did not enter a name to search"
			sleep 2;
			search;;
	esac
}

#增加函数
add(){
	clear
	echo "Enter name and score of record."
	echo -e "\c"

	if [ ! -f ./record ]; then
		touch record
	fi

	read NEWNAME
	echo "$NEWNAME" >>./record
	sort -o ./record ./record
}

#删除记录函数
delete(){
	clear
	echo -e "Please Enter Name>>>c"
	read NAME

	if [ ! -f ./record ]; then
		echo "This Name is not in record"
	else
		cp record record.bak
		rm -f record
		grep -v "$NAME" ./record.bak > record
		rm -f record.bak
	fi
}

#显示所有的记录的函数
display(){
	colour black_white
	more ./record
}

#利用vi编辑器编辑函数
edit(){
	vi ./record
}

#屏幕帮助函数
help(){
	clear
	colour black_cyan
	echo "This is a student's record program by UNIX shell language!"
}

#推出程序函数
quit(){
	clear
	colour black_white
	exit
}

clear
while true #利用while语句实现屏幕输出菜单
do
	colour red_yellow
	echo "****************************************"
	echo "*       STUDENT'S RECORD MENU 	     *"
	echo "****************************************"
	colour yellow_blue
	echo "########################################"
	echo "#          1:SEARCH A RECORD           #"
	echo "#      	 2:ADD A RECORD		     #"
	echo "#		 3:DELETE A RECORD	     #"
	echo "# 	 4:DISPLAY ALL RECORDS 	     #"
	echo "#		 5:EDIT RECORD WITH VI	     #"
	echo "#		 H:Help Screen		     #"
	echo "# 	 Q:Exit Program		     #"
	echo "########################################"
	colour black_green
	echo -e -n "\tPlease Enter Your Choice [1,2,3,4,5,H,Q]:\c "
	read CHOICE

	case $CHOICE in
		1) search;;
		2) add; clear;;
		3) delete; clear;;
		4) display;;
		5) edit; clear;;
		H|h) help;;
		Q|q) quit;;
		*) echo "Invalid Choice";
		sleep 2;
		clear;;
esac
done