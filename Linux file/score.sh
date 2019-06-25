# score.sh
# !/bin/sh

#colour 函数
colour()
{
    #格式为背景；前景m,主要实现屏幕颜色显示
    case $1 in
    black_white)
        echo -e "\033[40;37m"
        ;;
    black_green)
        echo -e "\033[40;32m"
        ;;
    black_cyan)
        echo -e "\033[40;36m"
        ;;
    red_yellow)
        echo -e "\033[40;33m"
        ;;
    yellow_blue)
        echo -e "\033[40;33m"
        ;;
    esac
}

#查询函数
search()
{
    colour black_white
    clear
    echo -e "Please Enter Name >>>\c"
    read NAME

    #如果无记录时
    if[! -f ./record]; then
        echo "You must hava some scores before you can serrch!"
        sleep 2
        clear
        return
    fi

    #没有输入名字进行查询时
    if[-z "$NAME" ]; then
        echo "You didn't enter a name!"
        echo -e "Please Enter Name >>>\c"
        read NAME
    fi

    grep -i "$NAME" ./record 2> /dev/null

    case "$?" in
        1) echo "Name didn't enter a name to search ";
        sleep 2;
        search;;
    esac
}

#增加记录函数
add()
{
    clear
    echo "Enter name and score of a record."
    echo -e "\c"

    if[! -f ./record]; then
        touch record
    fi

    read NEWNAME
    echo "$NEWNAME" >./record

    sort -o ./record ./record
}

#删除记录函数
delete()
{
    clear
    echo -e "Please Enter Name>>>c"
    read NAME

    if[! -f ./record]; then
    echo "This Name is not in record"
    else
    cp record record.bak
    rm -f record
    grep -v "$NAME" ./record.bak > record
    rm -r record.bak
    fi
}

#显示所有的记录函数
display()
{
    colour black_white
    more ./record
}

#利用vi编辑器编辑函数
edit()
{
    vi ./record
}

#屏幕帮助函数
help()
{
    clear
    colour black_cyan
    echo "This is a student's record grogram by UNIX shell language!"
}

#退出程序函数
quit()
{
    clear
    colour black_white
    exit
}

clear
while true
do
colour red_yellow
    echo "***********************************************"
    echo "*              STUDENTS RECORD MENU            "
    echo "***********************************************"
    colour yellow_blue
    echo "***********************************************"
    echo "*              1:SEARCH A RECORD              *"
    echo "*              2:SEARCH A RECORD              *"
    echo "*              3:SEARCH A RECORD              *"
    echo "*              4:SEARCH A RECORD              *"
    echo "*              5:SEARCH A RECORD              *"
    echo "*              H: Help Scoreen                *"
    echo "*              Q: EXIT Program                *"
    echo "***********************************************"
colour black_green
echo -e -n "\tPlease Enter Your Choice [1,2,3,4,5,H,Q]:\c"
read CHOICE

case $CHOICE in
    1) search;;
    2) add; clear;;
    3) delete; clear;;
    4) display;;
    5) edit; clear;
    H|h) help;;
    Q|q) quit;;
    *) echo "Invalid Choice!";
    sleep 2;
    clear;;
esac
done