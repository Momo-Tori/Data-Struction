#include "Huffman.h"
#include "Huffman.cpp"

int TreeNode::iBits;
int main(int argc, char **argv)
{

    fstream infile;

    fstream outfile;

    int iBits = 8;

    infile.open("input.txt", ios::in | ios::binary);

    if (!infile)
        exit(-1); //打开文件失败.

    outfile.open("out", ios::out | ios::trunc | ios::binary);
    if (!outfile)
        exit(-1); //打开文件失败.

    if (1)
    {
        outfile.write((char *)&iBits, sizeof(Tpointer));

        Huffman *pH = new Huffman(iBits);

        pH->Compression(infile, outfile);

        delete pH;
    }
    else
    {
        infile.read((char *)&iBits, sizeof(iBits));

        Huffman *pH = new Huffman(iBits);

        pH->DeCompression(infile, outfile);

        delete pH;
    }
    infile.close();
    outfile.close();
    return 0;
}
