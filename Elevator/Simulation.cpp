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

State::State(Evt *p)
{
    memset(Callup, 0, sizeof(Callup));
    memset(Calldown, 0, sizeof(Calldown));
    memset(PInF, 0, sizeof(PInF));
    E = new elevator[ElevatorNumber]();
    e = p;
    srand(time((time_t *)NULL));
}

void State::EvaMove(int n, int floor)
{
    
    /////////////////
    //Wait for make//
    /////////////////
}
void State::EvaMoveDone()
{
    //若有电梯停止事件则触发停止事件：先判断到达指定楼层，然后判断该层有同方向call
    //若已达到电梯target，则判断其新的target
    //若无则根据target设置下一个move
    //若无下一个move改变电梯状态为停止，并设置电梯返回本垒层事件
    /////////////////
    //Wait for make//
    /////////////////
}
void State::CallEva()
{
    //若有电梯现楼层到target楼层有呼叫楼层，则
    //若有停止电梯，改变其target为该层
    /////////////////
    //Wait for make//
    /////////////////
}
void State::PIF()
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
        CallEva();
    }
    if (target < floor && !Calldown[floor])
    {
        Calldown[floor] = 1;
        CallEva();
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
void State::setRushB() //空楼进人或原Rush事件结束后调用，生成rush事件
{
    int i = 0, min;
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
void State::RushB()
{
    int i = 0, min;
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

void Evt::AddEvt(int t, auto foo) //添加事件
{
    event *temp = new event(t, foo), *pt = &Head;
    while (pt->next && temp->time > pt->next->time)
        pt = pt->next;
    temp->next = pt->next;
    pt->next = temp;
}
void Evt::EvtHappen()
{
    auto temp = Head.next;
    Head.time = temp->time;
    Head.next = temp->next;
    (s->*(temp->Finish))(); //调用s对应的函数
    delete temp;
}
bool Evt::EvtTraverse(auto foo)
{
    auto pt = &Head;
    while (pt->next && pt->next->Finish != foo)
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