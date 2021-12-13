#include "Huffman.h"

int TreeNode::iBits;

// A simple arguments parser
std::pair<bool, string> getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return std::make_pair(true, *itr);
    }
    return std::make_pair(false, "");
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

int main(int argc, char **argv)
{

    auto input_info = getCmdOption(argv, argv + argc, "-f");
    fstream infile;
    auto output_info = getCmdOption(argv, argv + argc, "-o");
    fstream outfile;
    auto Bits = getCmdOption(argv, argv + argc, "-b");
    int iBits = 8;

    if (input_info.first)
        infile.open(input_info.second, ios::in | ios::binary);
    else
        infile.open("in", ios::in | ios::binary);
    if (!infile)
        exit(-1); //打开文件失败.

    if (output_info.first)
        outfile.open(output_info.second, ios::out | ios::trunc | ios::binary);
    else
        outfile.open("out", ios::out | ios::trunc | ios::binary);
    if (!outfile)
        exit(-1); //打开文件失败.

    if (cmdOptionExists(argv, argv + argc, "-c"))
    {

        if (Bits.first)
            iBits = stoi(Bits.second);

        outfile.write((char *)&iBits, sizeof(Tpointer));

        Huffman *pH = new Huffman(iBits);

        pH->Compression(infile, outfile);

        delete pH;
    }
    else if (cmdOptionExists(argv, argv + argc, "-d"))
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
