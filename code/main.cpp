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
  std::cerr << "  [LOG] Iniciando ACO para: " << instance.get_file_name() << std::endl;

  ACOKMIS aco_kmis = ACOKMIS(
      instance.get_connections(),
      instance.get_num_elements_l(),
      instance.get_num_elements_r());

  std::cerr << "  [LOG] Executando solve_kMIS com k=" << instance.get_k() << std::endl;
  auto exec_reports = aco_kmis.solve_kMIS(instance.get_k());
  std::cerr << "  [LOG] solve_kMIS finalizado. Reports gerados: " << exec_reports.size() << std::endl;

  Report report_instance(instance.get_connections(),
                         instance.get_file_name(),
                         instance.get_k(),
                         exec_reports);

  std::cerr << "  [LOG] Adicionando report ao manager..." << std::endl;
  report_manager.add_reports(report_instance);
  std::cerr << "  [LOG] Report adicionado com sucesso!" << std::endl;
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
  std::cerr << "  [LOG] Iniciando GRASP+TS para: " << instance.get_file_name() << std::endl;

  InstanceI I = mapACOInstanceToGRASPTsInstance(instance);

  vector<ReportExecData> results;

  for (int iter = 0; iter < 10; iter++) {
    GRASPTs graspts = GRASPTs(I);
    std::cerr << "    [LOG] GRASP iteracao " << (iter + 1) << "/10" << std::endl;
    auto result = graspts.solve_kMIS();
    results.insert(results.end(), result.begin(), result.end());
  }

  std::cerr << "  [LOG] GRASP finalizado. Reports gerados: " << results.size() << std::endl;

  Report report_instance(instance.get_connections(),
                         instance.get_file_name(),
                         instance.get_k(),
                         results);
  std::cerr << "  [LOG] Adicionando report ao manager..." << std::endl;
  report_manager.add_reports(report_instance);
  std::cerr << "  [LOG] Report adicionado com sucesso!" << std::endl;
}

int main() {
#ifndef DEBUG
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
#endif

  std::cerr << "======================================" << std::endl;
  std::cerr << "[LOG] Iniciando programa..." << std::endl;
  std::cerr << "======================================" << std::endl;

  IntancesReader reader = IntancesReader();
  const auto& instances = reader.get_instances();

  std::cerr << "[LOG] Total de instancias carregadas: " << instances.size() << std::endl;

  ReportManager report_manager_graspts = ReportManager("graspts");

  int instance_idx = 0;
  for (auto instance : instances) {
    instance_idx++;
    std::cerr << "[LOG] Processando instancia GRASP " << instance_idx << "/" << instances.size()
              << ": " << instance.get_file_name() << std::endl;

    processGRASPTs(instance, report_manager_graspts);
  }

  std::cerr << "-----------------acabou graspts-----------------------" << std::endl;

  ReportManager report_manager_aco = ReportManager("aco_kmis");

  instance_idx = 0;
  for (auto instance : instances) {
    instance_idx++;
    std::cerr << "[LOG] Processando instancia ACO " << instance_idx << "/" << instances.size()
              << ": " << instance.get_file_name() << std::endl;

    processACO(instance, report_manager_aco);
  }
  std::cerr << "-----------------acabou-----------------------" << std::endl;
  std::cerr << "[LOG] Programa finalizado com sucesso!" << std::endl;
}