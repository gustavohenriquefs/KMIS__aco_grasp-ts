#ifndef REPORT_H
#define REPORT_H

#include <format>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "../bibliotecas/roaring.hh"

typedef roaring::Roaring Subset;

struct ReportExecData {
  std::set<int> best_ans;
  float duration_ms;

  ReportExecData(std::set<int> best_ans, float duration_ms) : best_ans(best_ans), duration_ms(duration_ms) {
  }
};

struct Report {
 private:
  std::vector<Subset> connections;
  std::string instance_name;
  int k;

  std::vector<ReportExecData> reports_data;

 public:
  Report(
      std::vector<Subset> connections,
      std::string instance_name,
      int k,
      std::vector<ReportExecData>& reports_data) : connections(connections),
                                                   instance_name(instance_name),
                                                   k(k),
                                                   reports_data(reports_data) {
  }

  // void add_data(std::set<int>& best, float duration) {
  //   this->reports_data.push_back({best, duration});
  // }

  int get_ans(std::set<int>& ans_idx) {
    std::vector<Subset> ans;

    for (auto it : ans_idx) {
      ans.push_back(this->connections[it]);
    }

    return ans.capacity();
  }

  std::vector<std::vector<std::pair<std::string, std::string>>> get_data() {
    std::vector<std::vector<std::pair<std::string, std::string>>> reports_data_map;

    for (int i = 0; i < (int)this->reports_data.size(); ++i) {
      reports_data_map.push_back({});
      reports_data_map[i].push_back({"instance", this->instance_name});
      reports_data_map[i].push_back({"k", std::to_string(this->k)});
      reports_data_map[i].push_back({"best_ans", std::to_string(this->get_ans(this->reports_data[i].best_ans))});
      reports_data_map[i].push_back({"duration_ms", std::to_string(this->reports_data[i].duration_ms)});
    }

    return reports_data_map;
  }
};

#endif