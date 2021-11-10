#include "Simulation.h"

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <time.h>
extern const int FLOOR;
extern const int ElevatorNumber; //电梯数
extern const int T_Into_F;       //乘客进楼时间间隔上限
extern const int EWT;            //电梯等待时间
extern const int WaitingTime;    //乘客等待时间
extern const int MovingT;        //电梯移动一楼所用时间
extern const int Doortime;
extern const int CloseDoorWaitingtime;
State::State(Evt *p)
{
    memset(Callup, 0, sizeof(Callup));
    memset(Calldown, 0, sizeof(Calldown));
    memset(PInF, 0, sizeof(PInF));
    E = new elevator[ElevatorNumber]();
    e = p;
    srand(time((time_t *)NULL));
}

void State::EvaMove(int eva, elevator::state flag)
{
    //在此时便改变电梯楼层
    //用一个整数队列存储移动的顺序
    if (e->EvtTraverse(EvaMove, eva) && !E[eva].IfIO) //n号电梯没有移动&&未开门
    {
        if (flag == elevator::Up)
            E[eva].floor++;
        if (flag == elevator::Down)
            E[eva].floor--;
        e->AddEvt(e->time() + MovingT, EvaMoveDone, eva);
    }
}
void State::EvaMoveDone(int eva)
{
    //若有电梯停止事件则触发停止事件：先判断到达指定楼层，然后判断该层有同方向call
    if (E[eva].floor == E[eva].targetfloor || (E[eva].S == elevator::Up && Callup[eva]) || (E[eva].S == elevator::Down && Calldown[eva]))
    {
        E[eva].IFDO = 1;
        e->AddEvt(e->time() + Doortime, FBIOpenTheDoor, eva);
    }
    else
        EvaMove(eva, E[eva].S);
}
void State::FBIOpenTheDoor(int eva)
{
    e->AddEvt(e->time(), IOEva, eva);
    e->AddEvt(e->time() + CloseDoorWaitingtime, CloseDoor, eva);
}
void State::IOEva(int eva)
{
    /////////////////
    //Wait for make//
    /////////////////
    E[eva].IfIO = 1;
}
void State::CloseDoor(int eva)
{
    //若已达到电梯target，则判断其新的target
    //若无则根据target设置下一个move
    //若无下一个move改变电梯状态为停止，并设置电梯返回本垒层事件
    if (E[eva].IfIO)
    {
        E[eva].IfIO = 0;
        e->AddEvt(e->time() + CloseDoorWaitingtime, CloseDoor, eva); //设置下一次检查
        return;
    }
    if (E[eva].IFDO)
    {
        e->AddEvt(e->time() + Doortime, CloseDoor, eva); //设置关门完成事件
        E[eva].IFDO = 0;
        return;
    }
    
    ///////////////////////
    //We'll be right back//
    ///////////////////////
    //若当楼call有同方向则再次呼叫FBIOpenTheDoor
}

void State::HomeSweetHome(int eva)
{
    ;
    //       //=====================|
    //      ‹‹To be continued...\\//|
    //       \\=====================|
}
void State::CallEva(int floor, elevator::state flag) //根据呼叫楼层和上下楼标识呼叫电梯
{
    //若存在电梯现楼层到target楼层有呼叫楼层，则不做任何动作
    //若有停止电梯，改变其target为该层
    //若无不做任何动作
    int stopE = -1, i = 0;
    while (i < ElevatorNumber)
    {
        if (E[i].S == flag && (E[i].floor - floor) * (floor - E[i].targetfloor) >= 0)
            return;
        if (E[i].S == elevator::Stop)
            stopE = i;
        i++;
    }
    if (i != -1)
    {
        if (floor > E[stopE].floor)
        {
            E[stopE].S = elevator::Up;
            E[stopE].targetfloor = floor;
            EvaMove(stopE, elevator::Up);
        }
        if (floor < E[stopE].floor)
        {
            E[stopE].S = elevator::Down;
            E[stopE].targetfloor = floor;
            EvaMove(stopE, elevator::Down);
        }
        if (floor == E[stopE].floor)
            FBIOpenTheDoor(floor);
    }
}
void State::PIF(int i = 0)
{
    e->AddEvt(e->time() + rand() % T_Into_F + 1, State::PIF); //下一个人进入事件加入事件表，其中加1牺牲一个时间精度消除同时有人进楼的可能
    int floor = rand() % FLOOR;
    int target;
    while ((target = rand() % FLOOR) == floor)
        ; //target!=floor
    auto temp = PInF + floor;
    while (temp->next)
        temp = temp->next;
    temp = temp->next = new people();
    temp->next = NULL;
    temp->target = target;
    temp->giveuptime = e->time() + WaitingTime; //人链表节点建立完成
    if (target > floor && !Callup[floor])       //上楼且未按按钮
    {
        Callup[floor] = 1;
        CallEva(floor, elevator::Up);
    }
    if (target < floor && !Calldown[floor])
    {
        Calldown[floor] = 1;
        CallEva(floor, elevator::Down);
    }
    if (!e->EvtTraverse(RushB)) //没有人想离开，即这是第一个人
        setRushB();
    //下面是信息输出
    if (floor)
        std::cout << e->time() << "t:\t" << floor << "楼进入了1个人.\n";
    else
        std::cout << e->time() << "t:\t"
                  << "-1楼进入了1个人.\n";
}
void State::setRushB(int i = 0) //空楼进人或原Rush事件结束后调用，生成rush事件
{
    i = 0;
    int min;
    while (i < FLOOR && !PInF[i].next) //跳过没有人的楼层
        i++;
    min = i++;
    while (i < FLOOR) //寻找楼层放弃时间最近的人
    {
        if (PInF[i].next && PInF[i].next->giveuptime < PInF[min].next->giveuptime)
            min = i;
        i++;
    }
    if (min < FLOOR)
        e->AddEvt(PInF[min].next->giveuptime, RushB); //若有人则设置rushb事件
}
void State::RushB(int i)
{
    i = 0;
    int min;
    while (i < FLOOR && !PInF[i].next) //跳过没有人的楼层
        i++;
    min = i++;
    while (i < FLOOR) //寻找楼层放弃时间最近的人
    {
        if (PInF[i].next && PInF[i].next->giveuptime < PInF[min].next->giveuptime)
            min = i;
        i++;
    }
    if (min < FLOOR && e->time() == PInF[min].next->giveuptime) //存在要放弃的人仍然未进入电梯
    {
        auto temp = (PInF + min)->next;
        PInF[min].next = temp->next;
        delete temp;

        if (min)
            std::cout << e->time() << "t:\t" << min << "楼离开了1个人.\n";
        else
            std::cout << e->time() << "t:\t"
                      << "-1楼离开了1个人.\n";
    }
    setRushB();
}

void Evt::AddEvt(int t, auto foo, int n = 0) //添加事件
{
    event *temp = new event(t, foo), *pt = &Head;
    while (pt->next && temp->time > pt->next->time)
        pt = pt->next;
    temp->next = pt->next;
    pt->next = temp;
    temp->n = n;
}
void Evt::EvtHappen()
{
    auto temp = Head.next;
    Head.time = temp->time;
    Head.next = temp->next;
    (s->*(temp->Finish))(temp->n); //调用s对应的函数
    delete temp;
}
bool Evt::EvtTraverse(auto foo, int i = -1)
{
    auto pt = &Head;
    while (pt->next)
        if (pt->next->Finish == foo && (i == -1 || pt->next->n == i))
            break;
        else
            pt = pt->next;
    if (pt->next)
        return 1;
    return 0;
}
void Evt::EvtDelete(auto foo)
{
    auto pt = &Head;
    while (pt->next && pt->next->Finish != foo)
        pt = pt->next;
    if (pt->next)
    {
        auto temp = pt->next;
        pt->next = temp->next;
        delete temp;
    }
}

void Sim::begin()
{
    std::cout << "开始模拟" << std::endl;
    s.PIF();
    while (1)
        e.EvtHappen();
}