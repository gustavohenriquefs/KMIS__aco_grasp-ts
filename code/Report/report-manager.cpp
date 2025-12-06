#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "./report.cpp"

using namespace std;

namespace fs = std::filesystem;

class ReportManager {
 protected:
  string report_directory = "../Results";
  string report_file_name = "";

  vector<Report> reports;

  // count results file into directory ${report_directory}
  int get_results_size() const {
    int file_counter = 0;

    if (!fs::exists(report_directory)) {
      fs::create_directories(report_directory);
    }

    auto folder_it = fs::directory_iterator(this->report_directory);

    for (auto it : folder_it) {
      file_counter++;
    }

    return file_counter;
  }

  string get_fullpath() const {
    return this->report_directory + "/" + this->report_file_name;
  }

  void verify_or_create_path() {
    if (!fs::exists(this->report_directory)) {
      fs::create_directories(this->report_directory);
    }

    if (!fs::exists(this->get_fullpath())) {
      ofstream(this->get_fullpath());
    }
  }

 public:
  ReportManager() {
    this->report_file_name = "result-" + std::to_string(this->get_results_size()) + ".csv";
  }
  
  ~ReportManager() = default;
  
  void add_reports(Report& new_report) {
    this->reports.push_back(new_report);
    
    this->save_reports_on_file(new_report);
  }
  
  void save_reports_on_file(Report& new_report) {
    this->verify_or_create_path();
    
    std::cout << "[log]: init save reports..." << endl;
    
    std::ofstream report_file(this->get_fullpath(), std::ios_base::app | std::ios_base::out);
    
    if (!this->reports.size()) {
      cout << "[faild]: Rerports is empty \n";
      return;
    }
    
    try {
      if (!report_file.is_open()) {
        cout << "[faild]: the file could not be opened.\n";
        return;
      }
      
      for (auto instance : new_report.get_data()) {
        int i = 0;
        for (auto [key, value] : instance) {
          if (i) {
            report_file << "," + value;
          } else {
            report_file << value;
          }
          i++;
        }
        report_file << endl;
      }
      
      report_file.close();
      
      cout << "[success] Reports saved\n";
      
    } catch (exception& e) {
      cout << "[failed]: Report is not saved";
      cerr << e.what() << endl;
      return throw exception();
    };
  }
};

#endif