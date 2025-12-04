#include <iostream>
#include "ACO/acokmis.cpp"
#include "Intances/instances.cpp"

#define get_current_time() std::chrono::high_resolution_clock::now()
#define TIME_DIFF(start, end) std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()

// Function to process ACO for a given instance
// @param instance The instance to process
void processeACO(const Instance& instance) {
  ACOKMIS aco_kmis = ACOKMIS(
      instance.get_connections(),
      instance.get_num_elements_l(),
      instance.get_num_elements_r());

  aco_kmis.init();

  auto ans = aco_kmis.solve_kMIS(instance.get_k());  

  std::cout << "Tamanho da solução encontrada: " << ans.size() << std::endl;
  for (int node : ans) {
    std::cout << node << " ";
  }
  std::cout << std::endl;
}

// Function to execute ACO and measure time
// @param instance The instance to process
void executeAlgorithmACO(const Instance& instance) {
  const auto start_time = get_current_time();

  processeACO(instance);

  const auto end_time = get_current_time();
  const auto duration = TIME_DIFF(start_time, end_time);
}

int main() {
  std::cout << "-----------------começou-----------------------" << std::endl;
  IntancesReader reader = IntancesReader();
  const auto& instances = reader.get_instances();
  std::cout << "-----------------leu instancia-----------------------" << std::endl;

  std::cout << "Instancia 1 " << instances[0].to_string() << std::endl;


  // Execute ACO for each instance
  executeAlgorithmACO(instances[0]);
  std::cout << "-----------------acabou-----------------------" << std::endl;
}