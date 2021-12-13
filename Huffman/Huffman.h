#ifndef HUFFMAN
#define HUFFMAN

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
using namespace std;
typedef unsigned int cnt; //计算出现次数所用的单位
typedef int Tpointer;     //数组中的整数,注意更改时不能使用unsigned

class TreeNode
{
private:
    Tpointer lchild; //取0
    Tpointer rchild; //取1
    cnt weight;
    Tpointer data; //原压缩文件所对应的基础单元,用整数表示
    string s;
public:
    static int iBits;
    TreeNode()
    {
        lchild = -1;
        rchild = -1;
        weight = 0;
        data = 0;
    }
    cnt W() { return weight; }
    void setW(cnt wt){weight=wt;}
    Tpointer lch() { return lchild; }
    Tpointer rch() { return rchild; }
    Tpointer d() { return data; }
    void setChild(Tpointer l, Tpointer r)
    {
        lchild = l; //0为空，或者是头结点
        rchild = r;
    }
    void operator++() { weight++; }
    void SetData(Tpointer d) { data = d; }
    static void print(TreeNode *list[]); //打印子树
    const string& sout(){return s;}
    void sets(string &sin){s=sin;}
};

class Huffman
{
private:
public:
    Huffman(int n) { TreeNode::iBits = n; }
    void Compression(fstream &input, fstream &output);
    void DeCompression(fstream &input, fstream &output);
};

#endif