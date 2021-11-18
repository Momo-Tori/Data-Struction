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
extern const int PIOE;           //乘客进出电梯时间
extern const int MovingT;        //电梯移动一楼所用时间
extern const int Doortime;
extern const int CloseDoorWaitingtime;
extern const int FinishTime;

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
    if (!e->EvtTraverse(EvaMoveDone, eva) && !E[eva].IfIO) //n号电梯没有移动&&未开门
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
    if (E[eva].floor > 0)
        std::cout << e->time() << "t:\t " << E[eva].floor << "楼\t" << eva + 1 << "号电梯\t到达\n";
    else
        std::cout << e->time() << "t:\t"
                  << "-1楼\t" << eva + 1 << "号电梯\t到达\n";

    if ((E[eva].floor == E[eva].targetfloor) || (E[eva].S == elevator::Up && Callup[E[eva].floor]) || (E[eva].S == elevator::Down && Calldown[E[eva].floor]))
    {
        E[eva].IFDO = 1;
        e->AddEvt(e->time() + Doortime, FBIOpenTheDoor, eva);
    }
    else
        EvaMove(eva, E[eva].S);
}
void State::FBIOpenTheDoor(int eva)
{
    if (E[eva].S != elevator::Stop && E[eva].targetfloor == E[eva].floor)
    {
        people *temp = E[eva].passenger;
        while (temp && temp->target == E[eva].targetfloor)
            temp = temp->next;
        if (temp)
        {
            E[eva].targetfloor = temp->target;
        }
        //调度算法
        //可优化
        else if (((E[eva].S == elevator::Up && Callup[E[eva].floor]) || (E[eva].S == elevator::Down && Calldown[E[eva].floor])))
        {
            if (E[eva].S == elevator::Up)
                Callup[E[eva].floor] = 0;
            else if (E[eva].S == elevator::Down)
                Calldown[E[eva].floor] = 0;
        }
        else if ((E[eva].S == elevator::Up && Calldown[E[eva].floor]) || (E[eva].S == elevator::Down && Callup[E[eva].floor]))
        {
            if (E[eva].S == elevator::Up)
            {
                int i;
                for (i = E[eva].floor + 1; i < FLOOR; i++)
                {
                    if (Callup[i])
                    {
                        E[eva].targetfloor = i;
                        break;
                    }
                    if (Calldown[i])
                        E[eva].targetfloor = i;
                }
                if (E[eva].targetfloor == E[eva].floor)
                {
                    E[eva].S = elevator::Down;
                    Calldown[E[eva].floor] = 0;
                }
            }
            else if (E[eva].S == elevator::Down)
            {
                int i;
                for (i = E[eva].floor - 1; i >= 0; i--)
                {
                    if (Calldown[i])
                    {
                        E[eva].targetfloor = i;
                        break;
                    }
                    if (Callup[i])
                        E[eva].targetfloor = i;
                }
                if (E[eva].targetfloor == E[eva].floor)
                {
                    E[eva].S = elevator::Up;
                    Callup[E[eva].floor] = 0;
                }
            }
        }
        else
        {
            if (E[eva].S == elevator::Up)
            {
                int i;
                for (i = E[eva].floor + 1; i < FLOOR; i++)
                {
                    if (Callup[i])
                    {
                        E[eva].targetfloor = i;
                        break;
                    }
                    if (Calldown[i])
                        E[eva].targetfloor = i;
                }
                if (E[eva].targetfloor == E[eva].floor)
                {
                    for (i = E[eva].floor - 1; i >= 0; i--)
                    {
                        if (Calldown[i])
                        {
                            E[eva].targetfloor = i;
                            break;
                        }
                        if (Callup[i])
                            E[eva].targetfloor = i;
                    }
                }
            }
            else if (E[eva].S == elevator::Down)
            {
                int i;
                for (i = E[eva].floor - 1; i >= 0; i--)
                {
                    if (Calldown[i])
                    {
                        E[eva].targetfloor = i;
                        break;
                    }
                    if (Callup[i])
                        E[eva].targetfloor = i;
                }
                if (E[eva].targetfloor == E[eva].floor)
                    for (i = E[eva].floor + 1; i < FLOOR; i++)
                    {
                        if (Callup[i])
                        {
                            E[eva].targetfloor = i;
                            break;
                        }
                        if (Calldown[i])
                            E[eva].targetfloor = i;
                    }
            }
            if (E[eva].targetfloor == E[eva].floor)
            {
                E[eva].S = elevator::Stop;
                e->AddEvt(e->time() + EWT, HomeSweetHome, eva);
            }
            else if (E[eva].targetfloor > E[eva].floor)
                E[eva].S = elevator::Up;
            else if (E[eva].targetfloor < E[eva].floor)
                E[eva].S = elevator::Down;
            if (!E[eva].passenger)
            {
                if (E[eva].targetfloor > E[eva].floor)
                {
                    EvaMove(eva, elevator::Up);
                    return; //不需要安排开门事件
                }
                else if (E[eva].targetfloor < E[eva].floor)
                {
                    EvaMove(eva, elevator::Down);
                    return; //不需要安排开门事件
                }
            }
        }
    }
    if (E[eva].floor)
        std::cout << e->time() << "t:\t " << E[eva].floor << "楼\t" << eva + 1 << "号电梯\t开门.\n";
    else
        std::cout << e->time() << "t:\t-1楼\t" << eva + 1 << "号电梯\t开门.\n";

    e->AddEvt(e->time() + PIOE, IOEva, eva);
    e->AddEvt(e->time() + CloseDoorWaitingtime, CloseDoor, eva);
}
void State::IOEva(int eva)
{
    //根据电梯状态进人
    people *temp, *p;
    if (E[eva].passenger && (E[eva].passenger->target == E[eva].floor))
    { //乘客出门判断
        temp = E[eva].passenger;
        E[eva].passenger = temp->next;
        delete temp;
        e->AddEvt(e->time() + PIOE, IOEva, eva);
        E[eva].IfIO = 1;

        if (E[eva].floor)
            std::cout << e->time() << "t:\t " << E[eva].floor << "楼\t" << eva + 1 << "号电梯\t一个乘客离开电梯.\n";
        else
            std::cout << e->time() << "t:\t-1楼\t" << eva + 1 << "号电梯\t一个乘客离开电梯.\n";
    }
    else
    { //乘客进门判断
        temp = (PInF + E[eva].floor);
        if (E[eva].S == elevator::Up)
            while (temp->next && temp->next->target < E[eva].floor)
                temp = temp->next;
        if (E[eva].S == elevator::Down)
            while (temp->next && temp->next->target > E[eva].floor)
                temp = temp->next;
        if (temp->next)
        {
            p = temp->next;
            temp->next = p->next;
            temp = E[eva].passenger;
            if (E[eva].S == elevator::Up)
            {
                if (E[eva].passenger && p->target >= E[eva].passenger->target)
                {
                    while (temp->next && p->target > temp->next->target)
                        temp = temp->next;
                    p->next = temp->next;
                    temp->next = p;
                }
                else
                {
                    E[eva].passenger = p;
                    p->next = temp;
                }
            }
            if (E[eva].S == elevator::Down)
            {
                if (E[eva].passenger && p->target <= E[eva].passenger->target)
                {
                    while (temp->next && p->target < temp->next->target)
                        temp = temp->next;
                    p->next = temp->next;
                    temp->next = p;
                }
                else
                {
                    E[eva].passenger = p;
                    p->next = temp;
                }
            }
            e->AddEvt(e->time() + PIOE, IOEva, eva);
            E[eva].IfIO = 1;
            if (E[eva].floor)
                std::cout << e->time() << "t:\t " << E[eva].floor << "楼\t" << eva + 1 << "号电梯\t一个乘客进入电梯.\n";
            else
                std::cout << e->time() << "t:\t-1楼\t" << eva + 1 << "号电梯\t一个乘客进入电梯.\n";
        }
        else
        {
            if (E[eva].S == elevator::Up)
                Callup[E[eva].floor] = 0;
            if (E[eva].S == elevator::Down)
                Calldown[E[eva].floor] = 0;
        }
    }
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
    else if (E[eva].IFDO)
    {
        e->AddEvt(e->time() + Doortime, CloseDoor, eva); //设置关门完成事件
        E[eva].IFDO = 0;
        if (E[eva].floor)
            std::cout << e->time() << "t:\t " << E[eva].floor << "楼\t" << eva + 1 << "号电梯\t关门.\n";
        else
            std::cout << e->time() << "t:\t-1楼\t" << eva + 1 << "号电梯\t关门.\n";
        return;
    }
    else
    {
        if (E[eva].passenger)
            E[eva].targetfloor = E[eva].passenger->target;
        if (E[eva].targetfloor > E[eva].floor)
        {
            E[eva].S = elevator::Up;
            if (Callup[E[eva].floor]) //若当楼call有同方向则再次呼叫FBIOpenTheDoor
            {
                Callup[E[eva].floor] = 0;
                e->AddEvt(e->time() + Doortime, FBIOpenTheDoor, eva);
            }
            else
                EvaMove(eva, elevator::Up);
        }
        else if (E[eva].targetfloor < E[eva].floor)
        {
            E[eva].S = elevator::Down;
            if (Calldown[E[eva].floor])
            {
                Calldown[E[eva].floor] = 0;
                e->AddEvt(e->time() + Doortime, FBIOpenTheDoor, eva);
            }
            else
                EvaMove(eva, elevator::Down);
        }
        else
        {
            E[eva].S = elevator::Stop;
            if (e->EvtTraverse(HomeSweetHome, eva))
                e->AddEvt(e->time() + EWT, HomeSweetHome, eva);
        }
    }
}

void State::HomeSweetHome(int eva)
{
    E[eva].targetfloor = 1;
    if (E[eva].floor > 1)
    {
        E[eva].S = elevator::Down;
        EvaMove(eva, elevator::Down);
    }
    else if (E[eva].floor < 1)
    {
        E[eva].S = elevator::Up;
        EvaMove(eva, elevator::Up);
    }
}
void State::CallEva(int floor, elevator::state flag) //根据呼叫楼层和上下楼标识呼叫电梯
{
    //若存在电梯现楼层到target楼层有呼叫楼层，则不做任何动作
    //若有停止电梯，改变其target为该层
    //若无不做任何动作
    int stopE = -1, i = 0, min;
    while (i < ElevatorNumber)
    {
        if (E[i].S == flag && (E[i].floor - floor) * (floor - E[i].targetfloor) >= 0)
            return;
        if (E[i].S == elevator::Stop)
            if (stopE == -1)
            {
                stopE = i;
                min = (E[i].floor - floor) * (E[i].floor - floor); //取正数
            }
            else if ((E[i].floor - floor) * (E[i].floor - floor) < min)
            {
                stopE = i;
                min = (E[i].floor - floor) * (E[i].floor - floor);
            }
        i++;
    }
    if (stopE != -1)
    {
        if (floor > E[stopE].floor)
        {
            E[stopE].S = elevator::Up;
            E[stopE].targetfloor = floor;
            EvaMove(stopE, elevator::Up);
        }
        else if (floor < E[stopE].floor)
        {
            E[stopE].S = elevator::Down;
            E[stopE].targetfloor = floor;
            EvaMove(stopE, elevator::Down);
        }
        else if (floor == E[stopE].floor)
        {
            E[stopE].IFDO = 1;
            E[stopE].S = flag;
            e->AddEvt(e->time() + Doortime, FBIOpenTheDoor, stopE);
        }
        e->EvtDelete(HomeSweetHome, stopE);
    }
}
void State::PIF(int i = 0)
{
    e->AddEvt(e->time() + rand() % T_Into_F + 1, PIF); //下一个人进入事件加入事件表，其中加1牺牲一个时间精度消除同时有人进楼的可能
    int floor = rand() % FLOOR;
    int target;
    while ((target = rand() % FLOOR) == floor)
        ; //target!=floor

    //下面是信息输出
    if (floor)
        std::cout << e->time() << "t:\t " << floor << "楼\t\t进入了1个人,ta要去";
    else
        std::cout << e->time() << "t:\t"
                  << "-1楼\t\t进入了1个人,ta要去";
    if (target)
        std::cout << " " << target << "楼.\n";
    else
        std::cout << "-1楼.\n";

    auto temp = PInF + floor;
    while (temp->next)
        temp = temp->next;
    temp = temp->next = new people();
    temp->next = NULL;
    temp->target = target;
    temp->giveuptime = e->time() + WaitingTime; //人链表节点建立完成

    if (target > floor && !Callup[floor]) //上楼且未按按钮
    {
        for (i = 0; i < ElevatorNumber; i++)
        {
            if (E[i].floor == floor && E[i].S == elevator::Up)
                break;
        }
        if (i == ElevatorNumber)
        {
            Callup[floor] = 1;
            CallEva(floor, elevator::Up);
        }
    }
    else if (target < floor && !Calldown[floor])
    {
        for (i = 0; i < ElevatorNumber; i++)
        {
            if (E[i].floor == floor && E[i].S == elevator::Down)
                break;
        }
        if (i == ElevatorNumber)
        {
            Calldown[floor] = 1;
            CallEva(floor, elevator::Down);
        }
    }
    if (!e->EvtTraverse(RushB, floor)) //第n层楼没有人想离开，即这是第一个人
        e->AddEvt(temp->giveuptime, RushB, floor);
}
void State::RushB(int floor)
{
    if (PInF[floor].next && e->time() == PInF[floor].next->giveuptime) //存在要放弃的人仍然未进入电梯
    {
        auto temp = (PInF + floor)->next;
        PInF[floor].next = temp->next;
        delete temp;
        if (floor)
            std::cout << e->time() << "t:\t " << floor << "楼\t\t因时间太长离开了1个人.\n";
        else
            std::cout << e->time() << "t:\t"
                      << "-1楼\t\t因时间太长离开了1个人.\n";
    }
    if (PInF[floor].next)
        e->AddEvt(PInF[floor].next->giveuptime, RushB, floor);
}

void Evt::AddEvt(int t, void (State::*foo)(int), int n) //添加事件
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
bool Evt::EvtTraverse(void (State::*foo)(int), int i)
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
void Evt::EvtDelete(void (State::*foo)(int), int i)
{
    auto pt = &Head;
    while (pt->next)
        if (pt->next->Finish == foo && (i == -1 || pt->next->n == i))
            break;
        else
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
    std::cout << "开始模拟\n时间\t楼层\t电梯序号\t事件\n"
              << std::endl;
    s.PIF();
    while (e.time() < FinishTime)
        e.EvtHappen();
}
//       //=====================|
//      ‹‹To be continued...\\//|
//       \\=====================|