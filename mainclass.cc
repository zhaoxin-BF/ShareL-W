#include <iostream>
using namespace std;

#include "string"
#include <vecter>
#include <list>
#include "set"
#include <algorithm>
#include "functional"
#include "iterator"     //输出流迭代器的头文件
#include <numeric>
#include "map"
#include "deque"

class Speaker
{
public:
    string m_name;
    int    m_score[3];
};

//产生选手
int GenSpeaker(map<int, Speaker> &mapSpeaker, vecter<int> &v)
{
    string str="QWERTYUIOPASDFGHJKLZXCVB";
    random_shuffle(str.begin(), str.end());

    for(int i=0; i<24; i++)
    {
        Speaker tmp;
        tmp.m_name = "选手";
        tmp.m_name = tmp.m_name + str[i];
        mapSpeaker.insert(pair<int, Speaker>(100+i, tmp));
    }

    for(int i = 0;i<24;i++)
    {
        v.push_back(100+i);//参赛人员
    }

    return 0;
}

//选手抽签
int speech_contest_draw(vecter<int> &v)
{
    random_shuffle(v.begin(), v.end());
    return 0;

}

//选手比赛
int speech_contest(int index,vecter<int> &v1,map<int, Speaker> mapSpeaker,vecter<int> &v2)
{
    //小组的比赛得分给记录下来，求出前三名 和后三名
    multmap<int, int, greater<int>> multmapGroup; 小组成绩
    int tmpGroup;
    int i;
    for(vecter<int>::iterator it=v1.begin(); it!=v1.end(); it++)
    {
         //打分
        {
            deque<int> dscore;
            int j;
            for(int j = 0; j<10; j++) // 10个评委
            {
                int score = 50+ran()%50;
                dscore.push_back(score);
            }

            sort(dscore.begin(), dscore.end());
            dscore.pop_back();
            dscore.pop_fronk();//去处最高分，去除最低分
            
            //求平均分
            int scoresum = accumulate(dscore.begin(), dscore.end(), 0);
            int scoreavg = scoresum/dscore.size();

            mapSpeaker[*it].m_score[index] = scoreavg; //选手得分 存入容器

            multmapGroup.insert(pair<int, int>(scoreavg, *it));
        }

        //处理分组
        if  (tmpCount % 6 == 0)
        {
            cout << "小组的比赛成绩"<<endl;

            for(multmap<int, int, greater<int>>::iterator mit=multmapGroup.begin(); mit!=multmapGroup.end();mit++)
            {
                //编号 姓名 得分
                cout<<mit->scond <<"\t"<<mapSpeaker[mit.second].m_name<<"\t"<<
            }
        }


}

//查看比赛信息
int speech_contest_print(int index, vecter<int> v2, map<int, Speaker> mapSpeaker)
{

}


void main()
{
    //容器的设计
    map<int, Speaker> mapSpeaker; //所有参加比赛的选手
    vecter<int>       v1;//第一轮 演讲比赛名单
    vecter<int>       v2;//第二轮 演讲比赛名单
    vecter<int>       v3;//第三轮 演讲比赛名单
    vecter<int>       v4;//最后前三名 演讲比赛 名单
    
    //产生选手 得到第一轮选手的比赛名单
    GenSpeaker(mapSpeaker, v1);
    
    //第一轮选手抽签 选手比赛 查看结果
    
    speech_contest_drow(v1);
    speech_contest(0, v1, mapSpeaker, v2);
    speech_contest_print(0, v2, mapSpeaker);
    //第二轮

    speech_contest_drow(v2);
    speech_contest(1, v2, mapSpeaker, v3);
    speech_contest_print(1, v3, mapSpeaker);
    //第三轮

    speech_contest_drow(v3);
    speech_contest(2, v3, mapSpeaker, v4);
    speech_contest_print(2, v4, mapSpeaker);


    cout<<"run over!"<<endl;
    return;
}
