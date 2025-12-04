#include "./acokmis.hpp"

#include <math.h>

#include <cassert>
#include <iostream>
#include <set>
#include <vector>

typedef roaring::Roaring Subset;

class ACOKMIS : public ACO {
 private:
  std::vector<Subset> neighbors_R_;

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
    neighbors_R_.resize(numUsers);
  }

  // Implementações
  void init_pheromone_matrix() {
    pheromone_matrix_.assign(numUsers, std::vector<double>(numUsers, tau_0_));

// Verificação apenas em modo debug
#ifndef NDEBUG
    assert(!pheromone_matrix_.empty() && "Pheromone matrix should not be empty");
    assert(pheromone_matrix_[0][0] == tau_0_ && "Pheromone matrix not initialized correctly");
#endif
  }
  // Initilização do ACO:
  // -- Init pheromone matrix
  void init() override {
    init_pheromone_matrix();
  }

  int tamanho_intersec(std::set<int> s) {
    Subset intersec = connections[*(s.begin())];  // primeiro elem
    for (int i : s) {
      intersec = intersec & this->connections[i];
    }
    return intersec.cardinality();
  }

  std::set<int> solve_kMIS(int k) override {
    init_pheromone_matrix();
    
    std::set<int> best;

    int iter = 0;
    
    std::vector<std::set<int>> L(numUsers, std::set<int>());  // soluções de cada formiga

    while (iter < iter_max_) {  // limite por tempo
      // inicia todas soluções u
      for (int u = 0; u < numUsers; u++) {
        L[u].insert(u);
      }

      std::vector<std::vector<std::vector<float>>> p(numUsers, std::vector<std::vector<float>>(numUsers, std::vector<float>(numUsers, 0)));  // probabilidade de escolher cada nó

      for (int u = 0; u < numUsers; u++) {
        // Construir cada formiga u
        int i = u;

        while (L[u].size() < k) {
          Subset intersec = connections[u];

          for (int i : L[u])
            if (i != u) {
              intersec = intersec & this->connections[i];
            }

          // calcula pontuação gulosa
          std::vector<float> mu(numUsers);

          for (int j = 0; j < numUsers; j++)
            if (L[u].count(j) == 0) {
              int newAnsCard = connections[j].and_cardinality(intersec);

              mu[j] = (float)newAnsCard / intersec.cardinality();
            }

          // calcula probabilidade
          float sum = 0;
          for (int j = 0; j < numUsers; j++)
            if (L[u].count(j) == 0) {
              p[u][i][j] = pow(pheromone_matrix_[i][j], this->alpha_) * pow(mu[j], this->beta_);
              sum += p[u][i][j];
            }
          for (int j = 0; j < numUsers; j++)
            if (L[u].count(j) == 0) {
              p[u][i][j] = p[u][i][j] / sum;
            }

          // adiciona j com maior probabilidade
          // TODO: verificar se não é pra sortear
          int j_maxp = -1;
          
          for (int j = 0; j < numUsers; j++)
            if (L[u].count(j) == 0) {
              if (j_maxp == -1 || p[u][i][j] > p[u][i][j_maxp]) {
                j_maxp = j;
              }
            }

          L[u].insert(j_maxp);
          i = j_maxp;
        }

        // Substitui melhor solução, caso L[u] seja melhor
        if (best.empty() || tamanho_intersec(L[u]) > tamanho_intersec(best)) {
          best = L[u];
        }
      }

      std::vector<std::vector<float>> delta(numUsers, std::vector<float>(numUsers, 0));
      std::vector<std::vector<int>> Q(numUsers, std::vector<int>(numUsers, 0));

      for (int u = 0; u < numUsers; u++) {
        int Lu_card = tamanho_intersec(L[u]);
        for (int i : L[u]) {
          for (int j : L[u])
            if (i != j) {
              delta[i][j] = delta[i][j] + Lu_card;
              Q[i][j]++;
            }
        }
      }

      int best_card = tamanho_intersec(best);
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

      iter++;
    }

    return best;
  }

 private:
  int intersectionSize(const roaring::Roaring& A,
                       const roaring::Roaring& B) const {
    return (A & B).cardinality();
  }
};
