#include <pybind11/pybind11.h>
#include "logsink.h"
namespace py = pybind11;

PYBIND11_MODULE(log_module, m) {
  py::class_<LogSink>(m, "LogSink")
      .def(py::init<std::string, int, std::string>())
      .def("start", &LogSink::start)
      .def("stop", &LogSink::stop);
}
