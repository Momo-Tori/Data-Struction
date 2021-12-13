#include "Huffman.h"
#include <iomanip>

inline string Int2cBit(Tpointer data)
{
    string s;
    Tpointer i = 1 << (TreeNode::iBits - 1);
    while (i != 0)
    {
        if (data & i)
            s = s + '1';
        else
            s = s + '0';
        i >>= 1;
    }
    return s;
}

int a;
string sOut;

void DFS(TreeNode *list[], Tpointer i)
{
    Tpointer l = list[i]->lch();
    Tpointer r = list[i]->rch();
    cout << "w:" << list[i]->W() << '\t';
    if (l == -1 && r == -1)
    {
        list[i]->sets(sOut);
        cout << "d:" << Int2cBit(list[i]->d()) << " " << list[i]->sout() << '\n';
    }
    else
    {
        a++;
        if (l != -1)
        {
            sOut = sOut + '0';
            DFS(list, l);
            sOut.pop_back();
        }
        if (r != -1)
        {
            sOut = sOut + '1';
            int j = a;
            while (j-- > 0)
                cout << '\t';
            DFS(list, r);
            sOut.pop_back();
        }
        a--;
    }
}

void TreeNode::print(TreeNode *list[])
{
    a = 0;
    DFS(list, 0);
}

bool compare(TreeNode *&t1, TreeNode *&t2)
{
    return t1->W() > t2->W();
}

//outfile格式：iBits,num,(TreeNode->l,TreeNode->r,TreeNode->data),data,NumberOfFilling.

void Huffman::Compression(fstream &input, fstream &output)
{
    bool tag = false;
    Tpointer i, j = 0, temp, Maxnum = (1 << (TreeNode::iBits + 1)) - 1;
    Tpointer TPIO;
    TreeNode *list[Maxnum];
    Tpointer MaxUnitNum = 1 << (TreeNode::iBits);
    TreeNode *table[MaxUnitNum];
    for (i = 0; i < Maxnum; i++)
        list[i] = new TreeNode;
    for (i = 0; i < MaxUnitNum; i++)
        table[i] = list[i];
    char c1 = 0, cIO;
    input.read(&c1, sizeof(char));
    while (!tag)
    {
        i = TreeNode::iBits;
        temp = 0;
        while (i > 0)
        {
            temp <<= 1;
            temp += (c1 >> 7) & 0x01;
            c1 <<= 1;
            i--;
            if (j == 7)
            {
                j = 0;
                input.read(&c1, sizeof(char));
                if (tag = input.eof())
                {
                    temp <<= i;
                    break;
                }
            }
            else
                j++;
        }
        ++(*list[temp]);     //权加一
    }                        //最后一个单元放temp左边，右边填充0
    Tpointer k = 0, num = 0; //num为除去weight=0后的单元个数.
    while (k < MaxUnitNum)
    {
        if (list[k]->W() != 0)
        {
            list[k]->SetData(k);
            TreeNode *ptemp;
            if (k != num)
            {
                ptemp = list[num];
                list[num] = list[k];
                list[k] = ptemp;
            }
            num++;
        }
        k++;
    }
    Tpointer last = num;
    while (last > 1)
    {
        sort(list, list + last, compare);
        TreeNode *ptemp;
        ptemp = list[num];
        list[num] = list[last - 2];
        list[last - 2] = ptemp;
        ptemp->setChild(last - 1, num);
        ptemp->setW(list[last - 1]->W() + list[num]->W());
        last--;
        num++;
    }
    TreeNode::print(list);
    TPIO = num;
    output.write((char *)&TPIO, sizeof(Tpointer));
    i = 0;
    while (i < num)
    {
        TPIO = list[i]->lch();
        output.write((char *)&TPIO, sizeof(Tpointer));
        TPIO = list[i]->rch();
        output.write((char *)&TPIO, sizeof(Tpointer));
        TPIO = list[i]->d();
        output.write((char *)&TPIO, sizeof(Tpointer));
        i++;
    }
    input.clear();
    input.seekg(0);
    tag=false;
    j = 0;
    c1 = 0;
    char c2 = 0;
    input.read(&c1, sizeof(char));

    int l = 0;
    string s;
    while (!tag)
    {
        i = TreeNode::iBits;
        temp = 0;
        while (i > 0)
        {
            temp <<= 1;
            temp += (c1 >> 7) & 0x01;
            c1 <<= 1;
            i--;
            if (j == 7)
            {
                j = 0;
                input.read(&c1, sizeof(char));

                if (tag = input.eof())
                {
                    temp <<= i;
                    break;
                }
            }
            else
                j++;
        }
        s = table[temp]->sout();
        while (s.length())
        {
            c2 <<= 1;
            c2 += s[0] - '0';
            l++;
            if (l == 8)
            {
                output.write(&c2, sizeof(char));
                c2 = 0;
                l = 0;
            }
            s.erase(s.begin());
        }
        if (tag)
        {
            if (l != 0)
            {
                c2 <<= (8 - l);
                output.write(&c2, sizeof(char));
                cIO = 8 - l;
                output.write(&cIO, sizeof(char));
            } //处理结尾的填充个数.
            else
            {
                cIO = l;
                output.write(&cIO, sizeof(char));
            }
        }
    }
    cout << "Compression succeeded." << endl;
}
void Huffman::DeCompression(fstream &input, fstream &output)
{
    Tpointer num, i = 0, l, r, data;
    int j, m;
    Tpointer k;
    input.read((char *)&num, sizeof(Tpointer));
    bool tag = false; //文件读取结束的tag
    /////////////////////////////////////
    TreeNode **list=new TreeNode* [num];
    /////////////////////////////////////
    while (i < num)
    {
        input.read((char *)&l, sizeof(Tpointer));
        input.read((char *)&r, sizeof(Tpointer));
        input.read((char *)&data, sizeof(Tpointer));
        list[i] = new TreeNode(l, r, data);
        i++;
    }
    char c1, c2, c3, co = c3 = c2 = c1 = 0;
    input.read(&c1, sizeof(char));
    input.read(&c2, sizeof(char));
    input.read(&c3, sizeof(char));
    TreeNode *pTemp;
    tag = input.eof();
    j = m = 0;
    k = 0;
    pTemp = list[0];
    l = pTemp->lch();
    r = pTemp->rch();
    while (!tag)
    {
        while (j < 8)
        {
            pTemp = (c1 < 0) ? list[r] : list[l];
            c1 <<= 1;
            j++;
            l = pTemp->lch();
            r = pTemp->rch();
            if (l == -1)
            {
                k = (1 << (TreeNode::iBits - 1));
                data = pTemp->d();
                while (k != 0)
                {
                    co <<= 1;
                    co = (data & k) ? co + 1 : co;
                    m++;
                    if (m == 8)
                    {
                        output.write(&co, sizeof(char));
                        m = 0;
                    }
                    k >>= 1;
                }
                pTemp = list[0];
                l = pTemp->lch();
                r = pTemp->rch();
            }
        }
        c1 = c2;
        c2 = c3;
        j = 0;
        input.read(&c3, sizeof(char));

        tag = input.eof();
    }
    j = 8 - (int)c2;
    while (j > 0)
    {
        pTemp = (c1 < 0) ? list[r] : list[l];
        c1 <<= 1;
        j--;
        l = pTemp->lch();
        r = pTemp->rch();
        if (l == -1)
        {
            k = (1 << (TreeNode::iBits - 1));
            data = pTemp->d();
            while (k != 0)
            {
                co <<= 1;
                co = (data & k) ? co + 1 : co;
                m++;
                if (m == 8)
                {
                    output.write(&co, sizeof(char));
                    m = 0;
                }
                k >>= 1;
            }
            pTemp = list[0];
            l = pTemp->lch();
            r = pTemp->rch();
        }
    }
    cout << "Decompression succeeded." << endl;
}