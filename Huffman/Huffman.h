#include <iostream>
#include <fstream>
using namespace std;
typedef unsigned int cnt; //计算出现次数所用的单位

class TreeNode
{
private:
    TreeNode *lchild;
    TreeNode *rchild;
    bool *data; //所对应的数据
public:
    static int iBits;
    TreeNode(TreeNode *l, TreeNode *r)
    {
        lchild = l;
        rchild = r;
        data = NULL;
    }
    ~TreeNode()
    {
        if (data)
            delete[] data;
        if (lchild)
            delete lchild;
        if (rchild)
            delete rchild;
    }
    void SetData(bool *bData)
    {
        bool *temp = data = new bool[iBits], *finish = temp + iBits;
        while (temp != finish)
        {
            *(temp++) = *(bData++);
        }
    }
    ///////////////用一些库函数来复制会不会更好.//////////////
};

class HuffmanTree
{
private:
    TreeNode *Data; //二叉树的头指针.

public:
    HuffmanTree() { Data = NULL; };
    ~HuffmanTree()
    {
        if (Data)
            delete Data;
    };
    void sethead(TreeNode *H) { Data = H; };
    void print();
};

class Huffman
{
private:
    HuffmanTree HT;

public:
    Huffman(int n) : HT(HuffmanTree()) { TreeNode::iBits = n; };
    void Compression(fstream &input, fstream &output);
    void DeCompression(fstream &input, fstream &output);
};