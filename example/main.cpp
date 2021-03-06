// File: main.cpp
// Project: example
// Created Date: 17/05/2021
// Author: Shun Suzuki
// -----
// Last Modified: 07/07/2021
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2021 Hapis Lab. All rights reserved.
//

#include <autd3.hpp>
#include <autd3/gain/holo.hpp>
#include <autd3/link/soem.hpp>
#include <autd3/modulation/primitive.hpp>
#include <iostream>

#include "blas_backend.hpp"

using autd::NUM_TRANS_X, autd::NUM_TRANS_Y, autd::TRANS_SPACING_MM;
using autd::gain::holo::BLASBackend;
using namespace std;

string GetAdapterName() {
  auto adapters = autd::link::SOEM::enumerate_adapters();
  for (size_t i = 0; i < adapters.size(); i++) {
    auto& [fst, snd] = adapters[i];
    cout << "[" << i << "]: " << fst << ", " << snd << endl;
  }

  int index;
  cout << "Choose number: ";
  cin >> index;
  cin.ignore();

  return adapters[index].name;
}

inline autd::GainPtr SelectOpt(std::vector<autd::Vector3>& foci, std::vector<double>& amps) {
  std::cout << "Select Optimization Method" << std::endl;
  const std::vector<std::string> opts = {"SDP", "EVD", "GS", "GS-PAT", "NAIVE", "LM"};
  for (size_t i = 0; i < opts.size(); i++) std::cout << "[" << i << "]: " << opts[i] << std::endl;
  std::cout << "[others]: Finish" << std::endl;

  std::string in;
  size_t idx = 0;
  getline(std::cin, in);
  std::stringstream s(in);
  if (const auto empty = in == "\n"; !(s >> idx) || idx >= opts.size() || empty) return nullptr;

  const auto backend = BLASBackend::create();
  switch (idx) {
    case 0:
      return autd::gain::holo::SDP::create(backend, foci, amps);
    case 1:
      return autd::gain::holo::EVD::create(backend, foci, amps);
    case 2:
      return autd::gain::holo::GS::create(backend, foci, amps);
    case 3:
      return autd::gain::holo::GSPAT::create(backend, foci, amps);
    case 4:
      return autd::gain::holo::Naive::create(backend, foci, amps);
    case 5:
      return autd::gain::holo::LM::create(backend, foci, amps);
    default:
      return autd::gain::holo::SDP::create(backend, foci, amps);
  }
}

int main() {
  try {
    auto autd = autd::Controller::create();
    autd->geometry()->add_device(autd::Vector3(0, 0, 0), autd::Vector3(0, 0, 0));
    const auto ifname = GetAdapterName();
    autd->open(autd::link::SOEM::create(ifname, autd->geometry()->num_devices()));

    autd->geometry()->wavelength() = 8.5;

    autd->clear();

    autd->silent_mode() = true;

    const auto m = autd::modulation::Sine::create(150);  // 150Hz AM

    const auto center = autd::Vector3(TRANS_SPACING_MM * ((NUM_TRANS_X - 1) / 2.0), TRANS_SPACING_MM * ((NUM_TRANS_Y - 1) / 2.0), 150.0);

    while (true) {
      std::vector<autd::Vector3> foci = {center - autd::Vector3::UnitX() * 30.0, center + autd::Vector3::UnitX() * 30.0};
      std::vector<double> amps = {1, 1};

      const auto g = SelectOpt(foci, amps);
      if (g == nullptr) break;

      autd->send(g, m);

      cout << "press any key to finish..." << endl;
      cin.ignore();

      cout << "finish." << endl;
      autd->stop();
    }

    autd->clear();
    autd->close();

  } catch (exception& e) {
    std::cerr << e.what() << std::endl;
    return ENXIO;
  }
  return 0;
}
