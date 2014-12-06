 /**
  * @file uu.cc
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
  */

#include "uu.h"
#include <fstream>
#include <iostream>
#include <iterator>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage " << argv[0] << " input_file output_file";
    exit(1);
  }
  std::ifstream ifs(argv[1]);
  if (!ifs) {
    std::cout << "Unable to open input file" << argv[1];
    exit(1);
  }
  std::ofstream ofs;
  ofs.open(argv[2], std::ios::binary | std::ios::trunc);
  if (!ofs) {
    std::cout << "Unable to open output file" << argv[1];
    exit(1);
  }
  base64::decode( std::istream_iterator<char>(ifs), std::istream_iterator<char>(), std::ostream_iterator<unsigned char>(ofs));
}
