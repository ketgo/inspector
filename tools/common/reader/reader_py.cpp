
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

#include "tools/common/reader/reader.hpp"

namespace py = pybind11;

// TODO: Need an approach to send singnal to reader. If the reader is waiting
// then the python script freezes since the threads dont know that a SIGTERM or
// SIGINT has been received.

PYBIND11_MODULE(INSPECTOR_PYTHON_MODULE, m) {
  m.doc() =
      "Reader module to load trace events published by the inspector lib.";

  py::class_<inspector::Reader>(m, "Reader")
      .def(py::init<const std::chrono::microseconds,
                    const std::chrono::microseconds, const std::size_t,
                    const inspector::duration_t, const inspector::duration_t>(),
           py::arg("timeout_us") = inspector::Reader::defaultTimeout(),
           py::arg("polling_interval_us") =
               inspector::Reader::defaultPollingInterval(),
           py::arg("num_consumers") = inspector::Reader::defaultConsumerCount(),
           py::arg("min_window_size") =
               inspector::Reader::defaultMinWindowSize(),
           py::arg("max_window_size") =
               inspector::Reader::defaultMaxWindowSize())
      .def(
          "__iter__",
          [](const inspector::Reader& reader) {
            return py::make_iterator(reader.begin(), reader.end());
          },
          py::keep_alive<0, 1>());
}