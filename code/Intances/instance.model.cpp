#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "../bibliotecas/roaring.hh"

using namespace std;


struct Instance {
 private:
  string file_path;

  int num_elements_l;
  int num_elements_r;

  int num_edges;
  int num_elements_solution;

  int k;

  vector<roaring::Roaring> connections;

  void read_from_file() {
    ifstream file(file_path);
    string line;

    std::cout << "Lendo instância do arquivo: " << file_path << std::endl;
    if (file.is_open()) {
      // Primeira linha - números básicos
      getline(file, line);

      std::cout << "Primeira linha: " << line << std::endl;

      istringstream iss(line);

      iss >> num_elements_l >> num_elements_r >> num_edges >> k;

      // Linhas seguintes - conexões
      connections.resize(num_elements_l);
      
      for (int i = 0; i < num_edges; ++i) {
        int left_element, right_element;
        getline(file, line);
        istringstream edge_iss(line);

        edge_iss >> left_element >> right_element;
        connections[left_element-1].add(right_element-1);
      }

      std::cout << "Número de elementos L: " << num_elements_l << std::endl;
      std::cout << "Número de elementos R: " << num_elements_r << std::endl;

      file.close();
    } else {
      cerr << "Não foi possível abrir o arquivo: " << file_path << endl;
    }
  }

 public:
  Instance(const string& path) : file_path(path) {
    read_from_file();
  }

  int get_num_elements_l() const {
    return num_elements_l;
  }

  int get_num_elements_r() const {
    return num_elements_r;
  }

  int get_k() const {
    return k;
  } 

  vector<roaring::Roaring> get_connections() const {
    return this->connections;
  }

  string to_string() const {
    ostringstream oss;
    oss << "Instance from file: " << file_path << "\n";
    oss << "Num Elements L: " << num_elements_l << ", Num Elements R: " << num_elements_r << "\n";
    oss << "Num Edges: " << num_edges << ", Num Elements Solution: " << num_elements_solution << "\n";
    return oss.str();
  }
};
