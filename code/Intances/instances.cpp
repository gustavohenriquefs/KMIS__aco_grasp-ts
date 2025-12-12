#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../bibliotecas/roaring.hh"
#include "instance.model.cpp"

namespace fs = std::filesystem;
using namespace std;

const string INSTANCES_DIR = "./Dataset/";

class IntancesReader {
 private:
  vector<struct Instance> instances;

  vector<string> get_instances_file_names(const string& instance_folder) {
    vector<string> instance_file_names;

    for (const auto& entry : fs::directory_iterator(instance_folder)) {
      if (entry.is_regular_file() && entry.path().extension() == ".txt") {
        instance_file_names.push_back(entry.path().string());
      }
    }

    return instance_file_names;
  }

  void read_instances(const vector<string>& instance_folder_names) {
    for (const auto& name : instance_folder_names) {
      const string folder_path = INSTANCES_DIR + name;

      if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
        cerr << "[faild]: path for instances invalid or is not a directory " << folder_path << endl;
        continue;
      }

      const vector<string> instance_file_names = get_instances_file_names(folder_path);

      for (const auto& instance_path : instance_file_names) {
        Instance instance(instance_path);
        instances.push_back(instance);
      }
    }
  }

 public:
  IntancesReader(vector<string> instance_folder_names = { "type1/", "type2/", "type3/", "type4/" }) {
    read_instances(instance_folder_names);
  }

  const vector<Instance>& get_instances() const {
    return instances;
  }
};