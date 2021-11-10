#include <iostream>
const int FLOOR = 5;
const int ElevatorNumber = 2;        //电梯数
const int T_Into_F = 200;            //乘客进楼时间间隔上限
const int EWT = 300;                 //电梯等待时间
const int WaitingTime = 400;         //乘客等待时间
const int MovingT = 70;              //电梯移动一楼所用时间
const int Doortime = 20;             //开关门时间
const int CloseDoorWaitingtime = 40; //关门测试时间

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
    } S; //电梯状态
    int floor;
    people *passenger; //按照最近的目标楼层排序
    int targetfloor;   //乘客前往的最近的targetfloor
    bool IFDO;         //是否开门，判断是否停靠
    bool IfIO;         //开门后是否有人进出电梯
    elevator()
    {
        S = Stop;
        floor = 1;
        passenger = NULL;
        targetfloor = 1;
        IFDO = 0;
        IfIO = 0;
    }
};

class State
{
private:
    bool Callup[FLOOR];   //人的上楼标记
    bool Calldown[FLOOR]; //人的下楼标记
    people PInF[FLOOR];   //People in Floor,各楼的人的头链表，按照放弃时间排序
    elevator *E;          //电梯
    Evt *e;               //事件连接

public:
    State(Evt *p);
    void EvaMove(int n, elevator::state flag);     //电梯移动命令，对第n个电梯下达上楼或下楼的命令,n>=0
    void EvaMoveDone(int);                         //事件函数，移动结束事件
    void CallEva(int floor, elevator::state flag); //呼叫电梯命令，发起电梯调配
    void FBIOpenTheDoor(int);                      //电梯开门及之后的事件处理
    void IOEva(int);                               //乘客进出电梯，先出后进
    void CloseDoor(int);                           //关闭电梯，寻找下一个target楼层，执行EvaMove.
    void HomeSweetHome(int);                       //电梯开始返回本垒层
    void PIF(int);                                 //事件函数，People Into Floor乘客进楼事件
    void RushB(int);                               //事件函数，判断有没有人等不及润了
};

struct event
{
    int time;                   //事件发生时间
    event *next;                //组成事件链表
    void (State::*Finish)(int); //事件的动作函数，并可用于标识事件类型,若函数无参数则使用默认参数吸收保证形式相同
    int n;                      //保存传入参数
    event(int t, void (State::*p)(int))
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
    void AddEvt(int t, auto foo, int n);
    void EvtHappen();
    bool EvtTraverse(auto foo, int i); //遍历是否有foo对应事件,若有返回1
    void EvtDelete(auto foo);          //删除触发函数为foo的第一个事件.
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