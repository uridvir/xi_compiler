#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char* argv[]){
  std::vector<char> buffer(2,0);
  std::ifstream fin("test.txt");
  while(true){
    fin.read(&buffer[0], buffer.size());
    if(fin.eof()){
      for(int i = 0; i < fin.gcount(); i++){
        std::cout << buffer[i];
      }
      std::cout << std::endl;
      break;
    }
    else {
      for(char c : buffer){
        std::cout << c;
      }
      std::cout << std::endl;
    }
  }
}
