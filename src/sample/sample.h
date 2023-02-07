#pragma once

#include <string>

/// This function wraps one of the C++ samples from OpenVINO project repository.
/// The sample is titled "Hello Classification" and can be found in:
/// https://github.com/openvinotoolkit/openvino/tree/releases/2022/3/samples/cpp/hello_classification
int sample(
	const std::string model_path,
	const std::string image_path,
	const std::string device_name,
	const std::string cl_cache_path
);