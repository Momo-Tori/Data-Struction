#include<cstring>
#define FLOOR 5
struct event
{
    int time;//事件发生时间
    enum type{Normal,Giveup};//Normal不需要标记的事件,Giveup乘客放弃
    type tag;//事件类型
    event* next;//组成事件链表
    void (*Finish());//事件结束的动作函数，当事件结束时调用
};

class Evt{
    private:
    int t;
    event* Head;//时间链表
    void AddEvt();
    public:
    Evt(){Head=(event*)nullptr;t=0;AddEvt();};
    int time(){return t;};
    
};
void Evt::AddEvt(){

}

struct people
{
    int target;//目标楼层
    people* next;//链表
    int giveuptime;//放弃时间
};
struct elevator
{
    enum state{Stop,Up,Down};//电梯状态
    state S;
    int floor;
    people *passenger;//按照最近的目标楼层排序
    int targetfloor;//若有乘客则前往最近的targetfloor
    bool IfDO;//door open,是否开门
    bool IfIO;//开门后是否有人进出电梯
    elevator(){}
};

class State{
    private:
    int Callup[FLOOR];//人的上楼标记
    int Calldown[FLOOR];//人的下楼标记
    people *PInF[FLOOR];//People in Floor,各楼的人的头链表，按照放弃时间排序
    elevator E[2];
    public:
    State(){}
};
