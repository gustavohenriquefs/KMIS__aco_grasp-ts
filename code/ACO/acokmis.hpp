#pragma once
#include <set>
#include <vector>

#include "../bibliotecas/roaring.hh"

class ACO {
 protected:
  std::vector<roaring::Roaring> connections;
  double alpha_;
  double beta_;
  double tau_0_;
  double rho_;
  int iter_max_;
  int numUsers;

  std::vector<std::vector<double>>
      pheromone_matrix_;

 public:
  ACO(std::vector<roaring::Roaring> connections,
      int numUsers,
      int numIterations,
      double alpha = 0.5,
      double beta = 2.0,
      double tau_0 = 1.0,
      double rho = 0.7,
      int iter_max = 50)
      : connections(connections),
        numUsers(numUsers),
        alpha_(alpha),
        beta_(beta),
        tau_0_(tau_0),
        rho_(rho),
        iter_max_(iter_max) {}

  virtual ~ACO() = default;

  virtual std::vector<ReportExecData> solve_kMIS(int k) = 0;
};
