// File: main.cpp
// Project: example
// Created Date: 17/05/2021
// Author: Shun Suzuki
// -----
// Last Modified: 17/05/2021
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2021 Hapis Lab. All rights reserved.
//

#include <iostream>

#include "autd3.hpp"
#include "blas_backend.hpp"
#include "holo_gain.hpp"
#include "primitive_modulation.hpp"
#include "soem_link.hpp"

using autd::NUM_TRANS_X, autd::NUM_TRANS_Y, autd::TRANS_SPACING_MM;
using autd::gain::holo::BLASBackend;
using namespace std;

string GetAdapterName() {
  size_t size;
  auto adapters = autd::link::SOEMLink::EnumerateAdapters(&size);
  for (size_t i = 0; i < size; i++) {
    auto& [fst, snd] = adapters[i];
    cout << "[" << i << "]: " << fst << ", " << snd << endl;
  }

  int index;
  cout << "Choose number: ";
  cin >> index;
  cin.ignore();

  return adapters[index].second;
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

  const auto backend = BLASBackend::Create();
  switch (idx) {
    case 0:
      return autd::gain::holo::HoloGainSDP::Create(backend, foci, amps);
    case 1:
      return autd::gain::holo::HoloGainEVD::Create(backend, foci, amps);
    case 2:
      return autd::gain::holo::HoloGainGS::Create(backend, foci, amps);
    case 3:
      return autd::gain::holo::HoloGainGSPAT::Create(backend, foci, amps);
    case 4:
      return autd::gain::holo::HoloGainNaive::Create(backend, foci, amps);
    case 5:
      return autd::gain::holo::HoloGainLM::Create(backend, foci, amps);
    default:
      return autd::gain::holo::HoloGainSDP::Create(backend, foci, amps);
  }
}

int main() {
  try {
    autd::Controller autd;
    autd.geometry()->AddDevice(autd::Vector3(0, 0, 0), autd::Vector3(0, 0, 0));
    const auto ifname = GetAdapterName();
    if (auto res = autd.OpenWith(autd::link::SOEMLink::Create(ifname, autd.geometry()->num_devices())); res.is_err()) {
      std::cerr << res.unwrap_err() << std::endl;
      return ENXIO;
    }

    autd.geometry()->wavelength() = 8.5;

    autd.Clear().unwrap();
    autd.Synchronize().unwrap();

    autd.silent_mode() = true;

    const auto m = autd::modulation::Sine::Create(150);  // 150Hz AM

    const auto center = autd::Vector3(TRANS_SPACING_MM * ((NUM_TRANS_X - 1) / 2.0), TRANS_SPACING_MM * ((NUM_TRANS_Y - 1) / 2.0), 150.0);

    while (true) {
      std::vector<autd::Vector3> foci = {center - autd::Vector3::UnitX() * 30.0, center + autd::Vector3::UnitX() * 30.0};
      std::vector<double> amps = {1, 1};

      const auto g = SelectOpt(foci, amps);
      if (g == nullptr) break;

      autd.Send(g, m).unwrap();

      cout << "press any key to finish..." << endl;
      cin.ignore();

      cout << "finish." << endl;
      autd.Stop().unwrap();
    }

    autd.Clear().unwrap();
    autd.Close().unwrap();

  } catch (exception& e) {
    std::cerr << e.what() << std::endl;
    return ENXIO;
  }
  return 0;
}
