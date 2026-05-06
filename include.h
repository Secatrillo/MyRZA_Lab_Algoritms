#pragma once

#include <string>
#include <iostream>
#include <optional>
#include <vector>
#include <array>
#include <fstream>
#include <zmq.hpp>
#include <thread>
#include <pybind11/embed.h>
#include <ParserComtrade.h>
#include <cstdint>
#include <future>

namespace py = pybind11;

void run_python_script(intptr_t ctx_address,
                       const std::string& script_path,
                       std::promise<void>* telemetry_pull_ready = nullptr);