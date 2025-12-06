#include "./acokmis.hpp"

#include <math.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

#include "../Report/report-manager.cpp"

#define get_current_time() std::chrono::high_resolution_clock::now()
#define TIME_DIFF(start, end) std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
#define sz(v) (int)v.size()

typedef std::chrono::high_resolution_clock::time_point TimePoint;
typedef roaring::Roaring Subset;

const float IMPRECISION = 0.00001;

struct ACOKMISSolution {
  std::set<int> solution_ids;
  std::vector<Subset> connections;
  roaring::Roaring solution;

  ACOKMISSolution(std::vector<Subset>& connections) : connections(connections) {}

  void add_item_idx(int idx) {
    if (solution_ids.empty()) {
      solution_ids.insert(idx);
      solution = this->connections[idx];
      return;
    }

    this->solution_ids.insert(idx);
    this->solution = this->solution & this->connections[idx];
  }

  int size() const {
    return sz(this->solution_ids);
  }

  bool exist(int idx) const {
    return this->solution_ids.find(idx) != this->solution_ids.end();
  }

  bool empty() const {
    return this->solution_ids.empty();
  }
};

class ACOKMIS : public ACO {
 private:
  // Implementações
  void init_pheromone_matrix() {
    pheromone_matrix_.assign(numUsers, std::vector<double>(numUsers, tau_0_));

// Verificação apenas em modo debug
#ifndef NDEBUG
    assert(!pheromone_matrix_.empty() && "Pheromone matrix should not be empty");
    assert(pheromone_matrix_[0][0] == tau_0_ && "Pheromone matrix not initialized correctly");
#endif
  }
  int tamanho_intersec(std::set<int> s) {
    Subset intersec = connections[*(s.begin())];  // primeiro elem
    for (int i : s) {
      intersec = intersec & this->connections[i];
    }
    return intersec.cardinality();
  }

  uint32_t get_next_element(std::vector<pair<float, int>>& j_prob) const {
    int n = sz(j_prob);
    if (n == 0) return 0;
    if (n == 1) return j_prob[0].second;

    // Criar soma acumulada
    std::vector<float> prob_acc(n);
    prob_acc[0] = j_prob[0].first;
    
    for (int it = 1; it < n; it++) {
      prob_acc[it] = prob_acc[it - 1] + j_prob[it].first;
    }

    float x;

#ifdef __linux__
    x = drand48();  // Linux
#elif _WIN32
    x = (float)rand() / RAND_MAX;  // Windows
#else
#error "OS not supported!"
#endif

    float normalized_x = x * prob_acc[n - 1];

    // Binary search
    int l = 0;
    int r = n - 1;

    while (l < r) {
      int mid = l + ((r - l) >> 1);

      if (prob_acc[mid] >= normalized_x) {
        r = mid;
      } else {
        l = mid + 1;
      }
    }

    return j_prob[l].second;
  }

  int get_next_element_by_max_p(std::vector<ACOKMISSolution>& L, std::vector<std::vector<std::vector<float>>>& p, int u, int i) const {
    int j_maxp = -1;

    for (int j = 0; j < numUsers; j++)
      if (!L[u].exist(j)) {
        if (j_maxp == -1 || p[u][i][j] > p[u][i][j_maxp]) {
          j_maxp = j;
        }
      }

    return j_maxp;
  }

  int get_next_element_by_prob(std::vector<ACOKMISSolution>& L, std::vector<std::vector<std::vector<float>>>& p, int u, int i) const {
    std::vector<pair<float, int>> p_u_j;

    for (int j = 0; j < numUsers; j++)
      if (!L[u].exist(j)) {
        p_u_j.push_back({p[u][i][j], j});
      }

    return get_next_element(p_u_j);
  }

 public:
  ACOKMIS(std::vector<Subset> connections,
          int numUsers,
          int numIterations,
          double alpha = 0.5,
          double beta = 2.0,
          double tau_0 = 1.0,
          double rho = 0.7,
          int iter_max = 50)
      : ACO(connections, numUsers, numIterations, alpha, beta, tau_0, rho, iter_max) {
  }

  std::vector<ReportExecData> solve_kMIS(int k) override {
    vector<ReportExecData> reports;

    init_pheromone_matrix();

    ACOKMISSolution best(this->connections);

    int iter = 0;

    std::vector<ACOKMISSolution> L(numUsers, ACOKMISSolution(this->connections));  // soluções de cada formiga

    auto start_time = get_current_time();

    while (40000 > TIME_DIFF(start_time, get_current_time())) {  // limite por tempo
      for (int u = 0; u < numUsers; u++) {
        L[u] = ACOKMISSolution(this->connections);  // Reset da solução
        L[u].add_item_idx(u);
      }

      std::vector<std::vector<std::vector<float>>> p(numUsers, std::vector<std::vector<float>>(numUsers, std::vector<float>(numUsers, 0)));  // probabilidade de escolher cada nó

      for (int u = 0; u < numUsers; u++) {
        // Construir cada formiga u
        int i = u;

        while (sz(L[u]) < k) {
          // calcula pontuação gulosa
          std::vector<float> mu(numUsers);

          for (int j = 0; j < numUsers; j++)
            if (!L[u].exist(j)) {
              int newAnsCard = connections[j].and_cardinality(L[u].solution);

              mu[j] = (float)newAnsCard / L[u].solution.cardinality();
            }

          // calcula probabilidade
          float sum = 0;
          for (int j = 0; j < numUsers; j++)
            if (!L[u].exist(j)) {
              p[u][i][j] = pow(pheromone_matrix_[i][j], this->alpha_) * pow(mu[j], this->beta_);
              sum += p[u][i][j];
            }
          for (int j = 0; j < numUsers; j++)
            if (!L[u].exist(j)) {
              p[u][i][j] = p[u][i][j] / sum;
            }

          // adiciona j com maior probabilidade
          // TODO: verificar se não é pra sortear (I create a function for bellow action)
          // int j_maxp = -1;

          // for (int j = 0; j < numUsers; j++)
          //   if (!L[u].exist(j)) {
          //     if (j_maxp == -1 || p[u][i][j] > p[u][i][j_maxp]) {
          //       j_maxp = j;
          //     }
          //   }

          // L[u].add_item_idx(j_maxp);
          // i = j_maxp;

          // Alternative:
          int next_element_idx = get_next_element_by_prob(L, p, u, i);
          L[u].add_item_idx(next_element_idx);
          i = next_element_idx;
        }

        // Substitui melhor solução, caso L[u] seja melhor
        if (best.empty() || L[u].solution.cardinality() > best.solution.cardinality()) {
          best = L[u];
        }
      }

      std::vector<std::vector<float>> delta(numUsers, std::vector<float>(numUsers, 0));
      std::vector<std::vector<int>> Q(numUsers, std::vector<int>(numUsers, 0));

      for (int u = 0; u < numUsers; u++) {
        int Lu_card = L[u].solution.cardinality();

        for (int i : L[u].solution_ids) {
          for (int j : L[u].solution_ids)
            if (i != j) {
              delta[i][j] = delta[i][j] + Lu_card;
              Q[i][j]++;
            }
        }
      }

      int best_card = best.solution.cardinality();

      for (int i = 0; i < numUsers; i++) {
        for (int j = 0; j < numUsers; j++)
          if (i != j && Q[i][j] > 0) {
            delta[i][j] = delta[i][j] / Q[i][j];
            delta[i][j] = delta[i][j] / best_card;
          }
      }

      for (int i = 0; i < numUsers; i++) {
        for (int j = 0; j < numUsers; j++)
          if (i != j) {
            pheromone_matrix_[i][j] = (1 - rho_) * pheromone_matrix_[i][j] + delta[i][j];
          }
      }

      auto end_time = get_current_time();
      int elapsed_time = TIME_DIFF(start_time, end_time);

      reports.push_back(ReportExecData(best.solution_ids, elapsed_time));

      iter++;
    }

    std::cout << "[success]: ACOKMIS success runned!" << std::endl;

    return reports;
  }
};
