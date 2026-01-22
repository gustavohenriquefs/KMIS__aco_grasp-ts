#include <iostream>

#include "./GRASPTS/instance_i.cpp"
#include "./Report/report-manager.cpp"
#include "ACO/acokmis.cpp"
#include "GRASPTS/graspts.cpp"
#include "Intances/instances.cpp"
#include "common.hpp"

// Function to process ACO for a given instance
// @param instance The instance to process
void processACO(const Instance& instance, ReportManager& report_manager) {
  ACOKMIS aco_kmis = ACOKMIS(
      instance.get_connections(),
      instance.get_num_elements_l(),
      instance.get_num_elements_r());

  auto exec_reports = aco_kmis.solve_kMIS(instance.get_k());

  Report report_instance(instance.get_connections(),
                         instance.get_file_name(),
                         instance.get_k(),
                         exec_reports);

  report_manager.add_reports(report_instance);
}

InstanceI mapACOInstanceToGRASPTsInstance(const Instance& i) {
  InstanceI ni;

  ni.k = i.get_k();

  ni.featuresF = i.get_connections();

  for (int i = 0; i < sz(ni.featuresF); ++i) {
    ni.indicesE.push_back(i);
  }

  return ni;
}

// Function to process GRASP+Tabu Search for a given instance
// @param instance The instance to process
void processGRASPTs(const Instance& instance, ReportManager& report_manager) {
  InstanceI I = mapACOInstanceToGRASPTsInstance(instance);

  vector<ReportExecData> results;

  for (int iter = 0; iter < 10; iter++) {
    GRASPTs graspts = GRASPTs(I);
    auto result = graspts.solve_kMIS();
    results.insert(results.end(), result.begin(), result.end());
  }

  Report report_instance(instance.get_connections(),
                         instance.get_file_name(),
                         instance.get_k(),
                         results);

  report_manager.add_reports(report_instance);
}

int main() {
#ifndef DEBUG
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
#endif

  IntancesReader reader = IntancesReader();
  const auto& instances = reader.get_instances();


  ReportManager report_manager_aco = ReportManager("aco_kmis");

  int instance_idx = 0;
  for (auto instance : instances) {
    instance_idx++;
    processACO(instance, report_manager_aco);
  }
  
  ReportManager report_manager_graspts = ReportManager("graspts");
  
  instance_idx = 0;
  for (auto instance : instances) {
    instance_idx++;

    processGRASPTs(instance, report_manager_graspts);
  }
}