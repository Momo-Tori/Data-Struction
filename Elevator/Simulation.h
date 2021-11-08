#include <iostream>
const int FLOOR = 5;
const int ElevatorNumber = 2; //电梯数
const int T_Into_F = 100;     //乘客进楼时间间隔上限
const int EWT = 300;          //电梯等待时间
const int WaitingTime = 40;   //乘客等待时间
const int MovingT = 70;       //电梯移动一楼的速度

class Evt;

struct people
{
    int target;     //目标楼层
    int giveuptime; //放弃时间
    people *next;   //链表->队列
};
struct elevator
{
    enum state
    {
        Stop,
        Up,
        Down
    }; //电梯状态
    state S;
    int floor;
    people *passenger; //按照最近的目标楼层排序
    int targetfloor;   //乘客前往的最近的targetfloor
    bool IfDO;         //door open,是否开门,1开门
    bool IfIO;         //开门后是否有人进出电梯
    elevator()
    {
        S = Stop;
        floor = 1;
        passenger = NULL;
        targetfloor = 1;
        IfDO = 0;
        IfIO = 0;
    }
};

class State
{
private:
    bool Callup[FLOOR];   //人的上楼标记
    bool Calldown[FLOOR]; //人的下楼标记
    people PInF[FLOOR];   //People in Floor,各楼的人的头链表，按照放弃时间排序
    elevator *E;
    Evt *e;

public:
    State(Evt *p);
    void EvaMove(int n, int floor); //电梯移动命令，对第n个电梯下达前往Floor楼的命令,n>=0
    void EvaMoveDone();             //事件函数，移动结束事件
    void CallEva();                 //呼叫电梯命令，发起电梯调配
    void PIF();                     //事件函数，People Into Floor乘客进楼事件
    void setRushB();                //命令，设置RushB事件
    void RushB();                   //事件函数，判断有没有人等不及润了
};

struct event
{
    int time;                //事件发生时间
    event *next;             //组成事件链表
    void (State::*Finish)(); //事件的动作函数，并可用于标识事件类型
    event(int t, void (State::*p)())
    {
        time = t;
        next = NULL;
        Finish = p;
    }
};
class Evt
{
private:
    event Head; //按时间排序的事件链表头结点
    State *s;   //绑定State
public:
    Evt(State *a) : Head(event(0, NULL)) { s = a; }
    void AddEvt(int t, auto foo);
    void EvtHappen();
    bool EvtTraverse(auto foo); //遍历是否有foo对应事件,若有返回1
    void EvtDelete(auto foo);   //删除触发函数为foo的第一个事件.
    int time() { return Head.time; }
};

class Sim
{
private:
    Evt e;
    State s;

public:
    Sim() : e(&s), s(&e){};
    void begin();
};