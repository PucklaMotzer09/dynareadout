#include <binout.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace dro {

template <typename T>
inline std::vector<T> array_vector_wrapper(Array<T> &&arr) noexcept {
  std::vector<T> vec;
  vec.resize(arr.size());
  memcpy(vec.data(), arr.data(), arr.size() * sizeof(T));
  return vec;
}

template <typename T>
std::vector<T> binout_read_type_wrapper(Binout &bin_file,
                                        const std::string &path_to_variable) {
  return array_vector_wrapper(bin_file.read<T>(path_to_variable));
}

template <typename T> py::list array_to_python_list(Array<T> &&arr) {
  py::list l;

  for (const auto &v : arr) {
    l.append(v);
  }

  return l;
}

template <typename T> py::str array_to_python_string(Array<T> &&arr) {
  return py::str(arr.str());
}

py::object binout_read_wrapper(dro::Binout &bin_file,
                               const std::string &path_to_variable) {
  switch (bin_file.get_type_id(path_to_variable)) {
  case Int8:
    return array_to_python_string(bin_file.read<int8_t>(path_to_variable));
  case Int16:
    return array_to_python_list(bin_file.read<int16_t>(path_to_variable));
  case Int32:
    return array_to_python_list(bin_file.read<int32_t>(path_to_variable));
  case Int64:
    return array_to_python_list(bin_file.read<int64_t>(path_to_variable));
  case Uint8:
    return array_to_python_string(bin_file.read<uint8_t>(path_to_variable));
  case Uint16:
    return array_to_python_list(bin_file.read<uint16_t>(path_to_variable));
  case Uint32:
    return array_to_python_list(bin_file.read<uint32_t>(path_to_variable));
  case Uint64:
    return array_to_python_list(bin_file.read<uint64_t>(path_to_variable));
  case Float32:
    return array_to_python_list(bin_file.read<float>(path_to_variable));
  case Float64:
    return array_to_python_list(bin_file.read<double>(path_to_variable));
  default:
    const char *msg = "Variable not found";
    char *_msg = (char *)malloc(19);
    memcpy(_msg, msg, 19);
    throw Binout::Exception(String(_msg));
  }
}

std::vector<std::string>
binout_get_children_wrapper(const dro::Binout &bin_file,
                            const std::string &path) noexcept {
  const auto children(bin_file.get_children(path));
  std::vector<std::string> vec;
  vec.resize(children.size());
  for (size_t i = 0; i < children.size(); i++) {
    vec[i] = children[i].str();
  }
  return vec;
}

} // namespace dro

PYBIND11_MODULE(libbinout, m) {
  m.doc() = "Read in LS-Dyna Binout files";

  py::enum_<dro::BinoutType>(m, "BinoutType")
      .value("Int8", dro::BinoutType::Int8)
      .value("Int16", dro::BinoutType::Int16)
      .value("Int32", dro::BinoutType::Int32)
      .value("Int64", dro::BinoutType::Int64)
      .value("Uint8", dro::BinoutType::Uint8)
      .value("Uint16", dro::BinoutType::Uint16)
      .value("Uint32", dro::BinoutType::Uint32)
      .value("Uint64", dro::BinoutType::Uint64)
      .value("Float32", dro::BinoutType::Float32)
      .value("Float64", dro::BinoutType::Float64)
      .value("Invalid", dro::BinoutType::Invalid)

      ;

  py::class_<dro::Binout>(m, "Binout")
      .def(py::init<const std::string &>())
      .def("read", &dro::binout_read_wrapper)
      .def("read_int8", &dro::binout_read_type_wrapper<int8_t>)
      .def("read_uint8", &dro::binout_read_type_wrapper<uint8_t>)
      .def("read_int16", &dro::binout_read_type_wrapper<int16_t>)
      .def("read_uint16", &dro::binout_read_type_wrapper<uint16_t>)
      .def("read_int32", &dro::binout_read_type_wrapper<int32_t>)
      .def("read_uint32", &dro::binout_read_type_wrapper<uint32_t>)
      .def("read_int64", &dro::binout_read_type_wrapper<int64_t>)
      .def("read_uint64", &dro::binout_read_type_wrapper<uint64_t>)
      .def("read_float", &dro::binout_read_type_wrapper<float>)
      .def("read_double", &dro::binout_read_type_wrapper<double>)
      .def("get_type_id", &dro::Binout::get_type_id)
      .def("variable_exists", &dro::Binout::variable_exists)
      .def("get_children", &dro::binout_get_children_wrapper)

      ;
}