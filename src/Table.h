#ifndef TABLE_H
#define TABLE_H

#include <vector>

template<typename A, typename B>
class Table {
  public:
    struct List {
      public:
        B& operator[](A index){
          std::size_t i = static_cast<std::size_t>(index);
          if(i < list.size()){
            list.resize(i + 1);
          }
          return list[static_cast<std::size_t>(index)];
        }
        const B& operator[](A index) const {
          return list[static_cast<std::size_t>(index)];
        }
      private:
        std::vector<B> list;
    };
    List& operator[](std::size_t index){
      return table[index];
    }
    const List& operator[](std::size_t index) const {
      return table[index];
    }
    void resize(std::size_t size){
      table.resize(size);
    }
    void expand(std::size_t size){
      table.resize(table.size() + size);
    }
    std::size_t size(){
      return table.size();
    }
    Table() = default;
    explicit Table(std::size_t size){
      table = std::vector<Table::List>(size);
    }

  private:
    std::vector<List> table;
};

#endif
