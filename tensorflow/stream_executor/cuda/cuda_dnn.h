/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// The CUDA-specific DNN library support, implementing the general DnnSupport
// interface.

#ifndef TENSORFLOW_STREAM_EXECUTOR_CUDA_CUDA_DNN_H_
#define TENSORFLOW_STREAM_EXECUTOR_CUDA_CUDA_DNN_H_

#include "tensorflow/core/platform/thread_annotations.h"
#include "tensorflow/stream_executor/cuda/cuda_activation.h"
#include "tensorflow/stream_executor/dnn.h"
#include "tensorflow/stream_executor/lib/status.h"
#include "tensorflow/stream_executor/plugin_registry.h"
#include "tensorflow/stream_executor/temporary_device_memory.h"

namespace stream_executor {
namespace gpu {

class GpuExecutor;
class CudnnRnnDescriptor;
class CudnnRnnSequenceTensorDescriptor;
class CudnnRnnStateTensorDescriptor;
class CudnnCtcLossDescriptor;

// Opaque and unique identifier for the cuDNN plugin.
extern const PluginId kCuDnnPlugin;

// cudnn-library based DNN support. For details on overridden interface
// functions, see dnn.h.
class CudnnSupport : public dnn::DnnSupport {
 public:
  explicit CudnnSupport(GpuExecutor* parent);

  port::Status Init() override;
  port::StatusOr<perftools::gputools::dnn::VersionInfo> GetVersion() override;

  port::StatusOr<std::unique_ptr<dnn::RnnDescriptor>> createRnnDescriptor(
      int num_layers, int hidden_size, int input_size, int cell_size,
      int batch_size, dnn::RnnInputMode input_mode,
      dnn::RnnDirectionMode direction_mode, dnn::RnnMode rnn_mode,
      dnn::DataType data_type, const dnn::AlgorithmConfig& algorithm_config,
      float dropout, uint64 seed, ScratchAllocator* state_allocator,
      bool use_padded_io) override;

  port::StatusOr<std::unique_ptr<dnn::RnnSequenceTensorDescriptor>>
  createRnnSequenceTensorDescriptor(int max_seq_length, int batch_size,
                                    int data_size,
                                    dnn::DataType data_type) override;

  port::StatusOr<std::unique_ptr<dnn::RnnSequenceTensorDescriptor>>
  createRnnSequenceTensorDescriptor(int max_seq_length, int batch_size,
                                    int data_size,
                                    const absl::Span<const int>& seq_lengths,
                                    bool time_major,
                                    dnn::DataType data_type) override;

  port::StatusOr<std::unique_ptr<dnn::RnnStateTensorDescriptor>>
  createRnnStateTensorDescriptor(int num_layer, int batch_size, int data_size,
                                 dnn::DataType data_type) override;

  bool DoRnnForward(Stream* stream, const dnn::RnnDescriptor& rnn_desc,
                    const dnn::RnnSequenceTensorDescriptor& input_desc,
                    const DeviceMemory<Eigen::half>& input_data,
                    const DeviceMemory<int>& seq_lengths_data,
                    const dnn::RnnStateTensorDescriptor& input_h_desc,
                    const DeviceMemory<Eigen::half>& input_h_data,
                    const dnn::RnnStateTensorDescriptor& input_c_desc,
                    const DeviceMemory<Eigen::half>& input_c_data,
                    const DeviceMemory<Eigen::half>& params,
                    const dnn::RnnSequenceTensorDescriptor& output_desc,
                    DeviceMemory<Eigen::half>* output_data,
                    const dnn::RnnStateTensorDescriptor& output_h_desc,
                    DeviceMemory<Eigen::half>* output_h_data,
                    const dnn::RnnStateTensorDescriptor& output_c_desc,
                    DeviceMemory<Eigen::half>* output_c_data, bool is_training,
                    ScratchAllocator* reserve_space_allocator,
                    ScratchAllocator* workspace_allocator,
                    dnn::ProfileResult* output_profile_result) override;

  bool DoRnnForward(Stream* stream, const dnn::RnnDescriptor& rnn_desc,
                    const dnn::RnnSequenceTensorDescriptor& input_desc,
                    const DeviceMemory<float>& input_data,
                    const DeviceMemory<int>& seq_lengths_data,
                    const dnn::RnnStateTensorDescriptor& input_h_desc,
                    const DeviceMemory<float>& input_h_data,
                    const dnn::RnnStateTensorDescriptor& input_c_desc,
                    const DeviceMemory<float>& input_c_data,
                    const DeviceMemory<float>& params,
                    const dnn::RnnSequenceTensorDescriptor& output_desc,
                    DeviceMemory<float>* output_data,
                    const dnn::RnnStateTensorDescriptor& output_h_desc,
                    DeviceMemory<float>* output_h_data,
                    const dnn::RnnStateTensorDescriptor& output_c_desc,
                    DeviceMemory<float>* output_c_data, bool is_training,
                    ScratchAllocator* reserve_space_allocator,
                    ScratchAllocator* workspace_allocator,
                    dnn::ProfileResult* output_profile_result) override;

  bool DoRnnForward(Stream* stream, const dnn::RnnDescriptor& rnn_desc,
                    const dnn::RnnSequenceTensorDescriptor& input_desc,
                    const DeviceMemory<double>& input_data,
                    const DeviceMemory<int>& seq_lengths_data,
                    const dnn::RnnStateTensorDescriptor& input_h_desc,
                    const DeviceMemory<double>& input_h_data,
                    const dnn::RnnStateTensorDescriptor& input_c_desc,
                    const DeviceMemory<double>& input_c_data,
                    const DeviceMemory<double>& params,
                    const dnn::RnnSequenceTensorDescriptor& output_desc,
                    DeviceMemory<double>* output_data,
                    const dnn::RnnStateTensorDescriptor& output_h_desc,
                    DeviceMemory<double>* output_h_data,
                    const dnn::RnnStateTensorDescriptor& output_c_desc,
                    DeviceMemory<double>* output_c_data, bool is_training,
                    ScratchAllocator* reserve_space_allocator,
                    ScratchAllocator* workspace_allocator,
                    dnn::ProfileResult* output_profile_result) override;

  bool DoRnnBackward(Stream* stream, const dnn::RnnDescriptor& rnn_desc,
                     const dnn::RnnSequenceTensorDescriptor& input_desc,
                     const DeviceMemory<Eigen::half>& input_data,
                     const DeviceMemory<int>& seq_lengths_data,
                     const dnn::RnnStateTensorDescriptor& input_h_desc,
                     const DeviceMemory<Eigen::half>& input_h_data,
                     const dnn::RnnStateTensorDescriptor& input_c_desc,
                     const DeviceMemory<Eigen::half>& input_c_data,
                     const DeviceMemory<Eigen::half>& params,
                     const dnn::RnnSequenceTensorDescriptor& output_desc,
                     const DeviceMemory<Eigen::half>& output_data,
                     const dnn::RnnStateTensorDescriptor& output_h_desc,
                     const DeviceMemory<Eigen::half>& output_h_data,
                     const dnn::RnnStateTensorDescriptor& output_c_desc,
                     const DeviceMemory<Eigen::half>& output_c_data,
                     const DeviceMemory<Eigen::half>& output_backprop_data,
                     const DeviceMemory<Eigen::half>& output_h_backprop_data,
                     const DeviceMemory<Eigen::half>& output_c_backprop_data,
                     DeviceMemory<Eigen::half>* input_backprop_data,
                     DeviceMemory<Eigen::half>* input_h_backprop_data,
                     DeviceMemory<Eigen::half>* input_c_backprop_data,
                     DeviceMemory<Eigen::half>* params_backprop_data,
                     DeviceMemory<uint8>* reserve_space_data,
                     ScratchAllocator* workspace_allocator,
                     dnn::ProfileResult* output_profile_result) override;

  bool DoRnnBackward(Stream* stream, const dnn::RnnDescriptor& rnn_desc,
                     const dnn::RnnSequenceTensorDescriptor& input_desc,
                     const DeviceMemory<float>& input_data,
                     const DeviceMemory<int>& seq_lengths_data,
                     const dnn::RnnStateTensorDescriptor& input_h_desc,
                     const DeviceMemory<float>& input_h_data,
                     const dnn::RnnStateTensorDescriptor& input_c_desc,
                     const DeviceMemory<float>& input_c_data,
                     const DeviceMemory<float>& params,
                     const dnn::RnnSequenceTensorDescriptor& output_desc,
                     const DeviceMemory<float>& output_data,
                     const dnn::RnnStateTensorDescriptor& output_h_desc,
                     const DeviceMemory<float>& output_h_data,
                     const dnn::RnnStateTensorDescriptor& output_c_desc,
                     const DeviceMemory<float>& output_c_data,
                     const DeviceMemory<float>& output_backprop_data,
                     const DeviceMemory<float>& output_h_backprop_data,
                     const DeviceMemory<float>& output_c_backprop_data,
                     DeviceMemory<float>* input_backprop_data,
                     DeviceMemory<float>* input_h_backprop_data,
                     DeviceMemory<float>* input_c_backprop_data,
                     DeviceMemory<float>* params_backprop_data,
                     DeviceMemory<uint8>* reserve_space_data,
                     ScratchAllocator* workspace_allocator,
                     dnn::ProfileResult* output_profile_result) override;

  bool DoRnnBackward(Stream* stream, const dnn::RnnDescriptor& rnn_desc,
                     const dnn::RnnSequenceTensorDescriptor& input_desc,
                     const DeviceMemory<double>& input_data,
                     const DeviceMemory<int>& seq_lengths_data,
                     const dnn::RnnStateTensorDescriptor& input_h_desc,
                     const DeviceMemory<double>& input_h_data,
                     const dnn::RnnStateTensorDescriptor& input_c_desc,
                     const DeviceMemory<double>& input_c_data,
                     const DeviceMemory<double>& params,
                     const dnn::RnnSequenceTensorDescriptor& output_desc,
                     const DeviceMemory<double>& output_data,
                     const dnn::RnnStateTensorDescriptor& output_h_desc,
                     const DeviceMemory<double>& output_h_data,
                     const dnn::RnnStateTensorDescriptor& output_c_desc,
                     const DeviceMemory<double>& output_c_data,
                     const DeviceMemory<double>& output_backprop_data,
                     const DeviceMemory<double>& output_h_backprop_data,
                     const DeviceMemory<double>& output_c_backprop_data,
                     DeviceMemory<double>* input_backprop_data,
                     DeviceMemory<double>* input_h_backprop_data,
                     DeviceMemory<double>* input_c_backprop_data,
                     DeviceMemory<double>* params_backprop_data,
                     DeviceMemory<uint8>* reserve_space_data,
                     ScratchAllocator* workspace_allocator,
                     dnn::ProfileResult* output_profile_result) override;

  bool GetConvolveAlgorithms(
      bool with_winograd_nonfused, int cc_major, int cc_minor,
      std::vector<dnn::AlgorithmDesc>* out_algorithms) override;

  bool GetConvolveExecutionPlans(
      dnn::ConvolutionKind kind, dnn::DataType element_type, Stream* stream,
      const dnn::BatchDescriptor& input_descriptor,
      const dnn::FilterDescriptor& filter_descriptor,
      const dnn::BatchDescriptor& output_descriptor,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      std::vector<std::unique_ptr<dnn::ConvolveExecutionPlan>>* out_exec_plans)
      override;

  port::Status GetFusedConvolveExecutionPlans(
      dnn::ConvolutionKind kind, dnn::DataType element_type,
      double conv_input_scale, double side_input_scale, Stream* stream,
      const dnn::BatchDescriptor& input_descriptor,
      const dnn::FilterDescriptor& filter_descriptor,
      const dnn::BatchDescriptor& bias_descriptor,
      const dnn::BatchDescriptor& output_descriptor,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      std::vector<std::unique_ptr<dnn::ConvolveExecutionPlan>>* out_exec_plans);

  bool GetRnnAlgorithms(
      std::vector<dnn::AlgorithmDesc>* out_algorithms) override;

  bool GetConvolveBackwardDataAlgorithms(
      bool with_winograd_nonfused, int cc_major, int cc_minor,
      std::vector<dnn::AlgorithmDesc>* out_algorithms) override;

  bool GetConvolveBackwardFilterAlgorithms(
      bool with_winograd_nonfused, int cc_major, int cc_minor,
      std::vector<dnn::AlgorithmDesc>* out_algorithms) override;

  bool DoBatchNormalizationForward(
      Stream* stream, const DeviceMemory<float>& x,
      const DeviceMemory<float>& scale, const DeviceMemory<float>& offset,
      const DeviceMemory<float>& estimated_mean,
      const DeviceMemory<float>& estimated_var_iance,
      const DeviceMemory<float>& side_input, const dnn::BatchDescriptor& x_desc,
      const dnn::BatchDescriptor& scale_offset_desc, const double epsilon,
      const double exponential_average_factor,
      dnn::ActivationMode activation_mode, DeviceMemory<float>* y,
      DeviceMemory<float>* batch_mean, DeviceMemory<float>* batch_var,
      DeviceMemory<float>* saved_mean, DeviceMemory<float>* saved_inv_var,
      bool is_training, ScratchAllocator* reserve_space_allocator,
      ScratchAllocator* workspace_allocator) override;

  bool DoBatchNormalizationForward(
      Stream* stream, const DeviceMemory<Eigen::half>& x,
      const DeviceMemory<float>& scale, const DeviceMemory<float>& offset,
      const DeviceMemory<float>& estimated_mean,
      const DeviceMemory<float>& estimated_variance,
      const DeviceMemory<Eigen::half>& side_input,
      const dnn::BatchDescriptor& x_desc,
      const dnn::BatchDescriptor& scale_offset_desc, const double epsilon,
      const double exponential_average_factor,
      dnn::ActivationMode activation_mode, DeviceMemory<Eigen::half>* y,
      DeviceMemory<float>* batch_mean, DeviceMemory<float>* batch_var,
      DeviceMemory<float>* saved_mean, DeviceMemory<float>* saved_inv_var,
      bool is_training, ScratchAllocator* reserve_space_allocator,
      ScratchAllocator* workspace_allocator) override;

  bool DoBatchNormalizationBackward(
      Stream* stream, const DeviceMemory<float>& y_backprop,
      const DeviceMemory<float>& x, const DeviceMemory<float>& scale,
      const DeviceMemory<float>& mean, const DeviceMemory<float>& inv_var,
      const dnn::BatchDescriptor& x_desc,
      const dnn::BatchDescriptor& scale_offset_desc, const double epsilon,
      DeviceMemory<float>* x_backprop, DeviceMemory<float>* scale_backprop,
      DeviceMemory<float>* offset_backprop,
      DeviceMemory<uint8>* reserve_space_data,
      ScratchAllocator* workspace_allocator) override;

  bool DoBatchNormalizationBackward(
      Stream* stream, const DeviceMemory<Eigen::half>& y_backprop,
      const DeviceMemory<Eigen::half>& x, const DeviceMemory<float>& scale,
      const DeviceMemory<float>& mean, const DeviceMemory<float>& inv_var,
      const dnn::BatchDescriptor& x_desc,
      const dnn::BatchDescriptor& scale_offset_desc, const double epsilon,
      DeviceMemory<Eigen::half>* x_backprop,
      DeviceMemory<float>* scale_backprop, DeviceMemory<float>* offset_backprop,
      DeviceMemory<uint8>* reserve_space_data,
      ScratchAllocator* workspace_allocator) override;

  port::Status DoConvolve(
      dnn::ConvolutionKind kind, dnn::DataType element_type,
      dnn::DataType output_type, Stream* stream,
      const dnn::BatchDescriptor& input_descriptor, DeviceMemoryBase input_data,
      const dnn::FilterDescriptor& filter_descriptor,
      DeviceMemoryBase filter_data,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemoryBase output_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      dnn::AlgorithmDesc algorithm_desc, DeviceMemory<uint8> scratch_memory,
      dnn::ProfileResult* output_profile_result) override;

  port::Status DoConvolveWithExecutionPlan(
      dnn::ConvolutionKind kind, dnn::DataType element_type,
      dnn::DataType output_type, Stream* stream,
      const dnn::BatchDescriptor& input_descriptor, DeviceMemoryBase input_data,
      const dnn::FilterDescriptor& filter_descriptor,
      DeviceMemoryBase filter_data,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemoryBase output_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      const dnn::AlgorithmConfig& plan_config,
      ScratchAllocator* scratch_allocator,
      dnn::ProfileResult* output_profile_result);

  port::Status DoFusedConvolve(
      Stream* stream, dnn::DataType input_type, dnn::DataType side_input_type,
      dnn::DataType bias_type, dnn::DataType output_type,
      const dnn::BatchDescriptor& conv_input_descriptor,
      DeviceMemoryBase conv_input_data, double conv_input_scale,
      const dnn::FilterDescriptor& filter_descriptor,
      DeviceMemoryBase filter_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      DeviceMemoryBase side_input_data, double side_input_scale,
      const dnn::BatchDescriptor& bias_descriptor, DeviceMemoryBase biases,
      dnn::ActivationMode activation_mode,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemoryBase output_data, ScratchAllocator* scratch_allocator,
      const dnn::AlgorithmConfig& algorithm_config,
      dnn::ProfileResult* output_profile_result) override;

  port::Status DoFusedConvolveWithExecutionPlan(
      Stream* stream, dnn::DataType element_type,
      const dnn::BatchDescriptor& conv_input_descriptor,
      DeviceMemoryBase conv_input_data, double conv_input_scale,
      const dnn::FilterDescriptor& filter_descriptor,
      DeviceMemoryBase filter_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      DeviceMemoryBase side_input_data, double side_input_scale,
      const dnn::BatchDescriptor& bias_descriptor, DeviceMemoryBase biases,
      dnn::ActivationMode activation_mode,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemoryBase output_data, ScratchAllocator* scratch_allocator,
      const dnn::AlgorithmConfig& algorithm_config,
      dnn::ProfileResult* output_profile_result);

  bool DoConvolveQuantized(
      Stream* stream, const dnn::BatchDescriptor& input_descriptor,
      const DeviceMemory<float>& input_data,
      const dnn::FilterDescriptor& filter_descriptor,
      const DeviceMemory<int8>& filter_coefficients,
      const DeviceMemory<float>& coefficient_scales,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemory<float>* output_data) override {
    LOG(ERROR) << "DoConvolveQuantized not supported by cuDNN";
    return false;
  }

  bool DoConvolveQuantized(
      Stream* stream, const dnn::BatchDescriptor& input_descriptor,
      const DeviceMemory<float>& input_data,
      const dnn::FilterDescriptor& filter_descriptor,
      const DeviceMemory<int16>& filter_coefficients,
      const DeviceMemory<float>& coefficient_scales,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemory<float>* output_data) override {
    LOG(ERROR) << "DoConvolveQuantized not supported by cuDNN";
    return false;
  }

  bool DoSeparableConvolve(
      Stream* stream, const dnn::BatchDescriptor& batch_descriptor,
      const DeviceMemory<float>& input_data,
      const dnn::FilterDescriptor& filter_descriptor, int depth_multiplier,
      const DeviceMemory<float>& first_weights,
      const DeviceMemory<float>& second_weights,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemory<float>* output_data) override {
    LOG(ERROR) << "separable convolution not supported by CUDNN";
    return false;
  }

  bool DoConvolveBackwardBias(
      Stream* stream, const dnn::BatchDescriptor& input_descriptor,
      const DeviceMemory<double>& input_data,
      const dnn::BatchDescriptor& bias_descriptor,
      DeviceMemory<double>* backward_bias_data) override;

  bool DoConvolveBackwardBias(Stream* stream,
                              const dnn::BatchDescriptor& input_descriptor,
                              const DeviceMemory<float>& input_data,
                              const dnn::BatchDescriptor& bias_descriptor,
                              DeviceMemory<float>* backward_bias_data) override;

  bool DoConvolveBackwardBias(
      Stream* stream, const dnn::BatchDescriptor& input_descriptor,
      const DeviceMemory<Eigen::half>& input_data,
      const dnn::BatchDescriptor& bias_descriptor,
      DeviceMemory<Eigen::half>* backward_bias_data) override;

  bool DoMatMul(Stream* stream, const DeviceMemory<float>& input_data,
                const DeviceMemory<float>& weights,
                const dnn::BatchDescriptor& input_dimensions,
                const dnn::BatchDescriptor& output_dimensions,
                DeviceMemory<float>* output_data) override;

  bool DoMatMulQuantized(Stream* stream, const DeviceMemory<float>& input_data,
                         const DeviceMemory<int8>& quantized_weights,
                         const DeviceMemory<float>& weight_scales,
                         const dnn::BatchDescriptor& input_dimensions,
                         const dnn::BatchDescriptor& output_dimensions,
                         DeviceMemory<float>* output_data) override {
    LOG(ERROR) << "DNN MatMulQuantized not supported by CUDNN";
    return false;
  }

  bool DoMatMulQuantized(Stream* stream, const DeviceMemory<float>& input_data,
                         const DeviceMemory<int16>& quantized_weights,
                         const DeviceMemory<float>& weight_scales,
                         const dnn::BatchDescriptor& input_dimensions,
                         const dnn::BatchDescriptor& output_dimensions,
                         DeviceMemory<float>* output_data) override {
    LOG(ERROR) << "DNN MatMulQuantized not supported by CUDNN";
    return false;
  }

  bool DoBiasAdd(Stream* stream, const DeviceMemory<float>& input_data,
                 const DeviceMemory<float>& biases,
                 const dnn::BatchDescriptor& dimensions,
                 DeviceMemory<float>* output_data) override;

  bool DoActivate(Stream* stream, dnn::ActivationMode activation_mode,
                  const dnn::BatchDescriptor& dimensions,
                  const DeviceMemory<float>& input_data,
                  DeviceMemory<float>* output_data, uint64 options) override;

  bool DoPoolForward(Stream* stream,
                     const dnn::PoolingDescriptor& pooling_dimensions,
                     const dnn::BatchDescriptor& input_dimensions,
                     const DeviceMemory<double>& input_data,
                     const dnn::BatchDescriptor& output_dimensions,
                     DeviceMemory<double>* output_data,
                     ScratchAllocator* workspace_allocator) override;

  bool DoPoolForward(Stream* stream,
                     const dnn::PoolingDescriptor& pooling_dimensions,
                     const dnn::BatchDescriptor& input_dimensions,
                     const DeviceMemory<float>& input_data,
                     const dnn::BatchDescriptor& output_dimensions,
                     DeviceMemory<float>* output_data,
                     ScratchAllocator* workspace_allocator) override;

  bool DoPoolForward(Stream* stream,
                     const dnn::PoolingDescriptor& pooling_dimensions,
                     const dnn::BatchDescriptor& input_dimensions,
                     const DeviceMemory<Eigen::half>& input_data,
                     const dnn::BatchDescriptor& output_dimensions,
                     DeviceMemory<Eigen::half>* output_data,
                     ScratchAllocator* workspace_allocator) override;

  bool DoPoolForward(Stream* stream,
                     const dnn::PoolingDescriptor& pooling_dimensions,
                     const dnn::BatchDescriptor& input_dimensions,
                     const DeviceMemory<int8>& input_data,
                     const dnn::BatchDescriptor& output_dimensions,
                     DeviceMemory<int8>* output_data,
                     ScratchAllocator* workspace_allocator) override;

  bool DoPoolBackward(Stream* stream,
                      const dnn::PoolingDescriptor& pooling_dimensions,
                      const dnn::BatchDescriptor& input_dimensions,
                      const DeviceMemory<double>& input_data,
                      const dnn::BatchDescriptor& output_dimensions,
                      const DeviceMemory<double>& output_data,
                      const DeviceMemory<double>& input_diff_data,
                      DeviceMemory<double>* output_diff_data,
                      ScratchAllocator* workspace_allocator) override;

  bool DoPoolBackward(Stream* stream,
                      const dnn::PoolingDescriptor& pooling_dimensions,
                      const dnn::BatchDescriptor& input_dimensions,
                      const DeviceMemory<float>& input_data,
                      const dnn::BatchDescriptor& output_dimensions,
                      const DeviceMemory<float>& output_data,
                      const DeviceMemory<float>& input_diff_data,
                      DeviceMemory<float>* output_diff_data,
                      ScratchAllocator* workspace_allocator) override;

  bool DoPoolBackward(Stream* stream,
                      const dnn::PoolingDescriptor& pooling_dimensions,
                      const dnn::BatchDescriptor& input_dimensions,
                      const DeviceMemory<Eigen::half>& input_data,
                      const dnn::BatchDescriptor& output_dimensions,
                      const DeviceMemory<Eigen::half>& output_data,
                      const DeviceMemory<Eigen::half>& input_diff_data,
                      DeviceMemory<Eigen::half>* output_diff_data,
                      ScratchAllocator* workspace_allocator) override;

  bool DoNormalizeWithDimensions(
      Stream* stream, const dnn::NormalizeDescriptor& normalize_descriptor,
      const dnn::BatchDescriptor& dimensions,
      const DeviceMemory<float>& input_data,
      DeviceMemory<float>* output_data) override;

  bool DoNormalizeBackwardWithDimensions(
      Stream* stream, const dnn::NormalizeDescriptor& normalize_descriptor,
      const dnn::BatchDescriptor& dimensions,
      const DeviceMemory<float>& raw_data,
      const DeviceMemory<float>& normalized_data,
      const DeviceMemory<float>& normalized_variable_gradient,
      DeviceMemory<float>* raw_variable_gradient,
      ScratchAllocator* workspace_allocator) override;

  bool DoDepthConcatenate(
      Stream* stream, port::ArraySlice<dnn::BatchDescriptor> input_dimensions,
      port::ArraySlice<const DeviceMemory<float>*> input_data,
      DeviceMemory<float>* output_data) override;

  bool DoElementwiseOperate(
      Stream* stream, dnn::ElementwiseOperation operation,
      port::ArraySlice<dnn::BatchDescriptor> input_dimensions,
      port::ArraySlice<const DeviceMemory<float>*> input_data,
      const dnn::BatchDescriptor& output_dimensions,
      DeviceMemory<float>* output_data) override;

  bool DoXYPad(Stream* stream, const dnn::BatchDescriptor& dimensions,
               const DeviceMemory<float>& input_data, int64 left_pad,
               int64 right_pad, int64 top_pad, int64 bottom_pad,
               DeviceMemory<float>* output_data) override;

  bool DoXYSlice(Stream* stream, const dnn::BatchDescriptor& dimensions,
                 const DeviceMemory<float>& input_data, int64 left_trim,
                 int64 right_trim, int64 top_trim, int64 bottom_trim,
                 DeviceMemory<float>* output_data) override;

  bool DoMemcpyD2HQuantized(Stream* stream,
                            const DeviceMemory<float>& device_unquantized_src,
                            dnn::QuantizedActivationMode mode, void* host_dst,
                            int64 size) override;

  bool DoMemcpyH2DQuantized(
      Stream* stream, const void* host_src, int64 size,
      dnn::QuantizedActivationMode mode,
      DeviceMemory<float>* device_unquantized_dst) override;

  // Derives an output batch descriptor from an input batch and convolution
  // descriptors.
  bool DeriveOutputBatchDescriptor(
      const dnn::BatchDescriptor& batch_descriptor,
      const dnn::FilterDescriptor& filter_descriptor,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      dnn::BatchDescriptor* output_batch_descriptor);

  port::Status DoCtcLoss(Stream* stream, dnn::DataType element_type,
                         const dnn::RnnStateTensorDescriptor& probs_desc,
                         const DeviceMemoryBase probs_data,
                         absl::Span<const int> labels_data,
                         absl::Span<const int> labels_lengths_data,
                         absl::Span<const int> input_lengths_data,
                         DeviceMemoryBase costs_data,
                         const dnn::RnnStateTensorDescriptor& grads_desc,
                         DeviceMemoryBase grads_data,
                         DeviceMemory<uint8> scratch_memory,
                         int ctc_loss_algo_id) override;

  bool DoTransformTensor(Stream* stream, const dnn::BatchDescriptor& input_desc,
                         dnn::DataType input_type,
                         const DeviceMemoryBase& input_data,
                         const dnn::BatchDescriptor& output_desc,
                         dnn::DataType output_type, float scale,
                         DeviceMemoryBase* output_data) override;

 private:
  GpuExecutor* parent_;  // Parent executor object. Not owned.

  // Provides access to the cuDNN handle.
  std::unique_ptr<class CudnnAccess> cudnn_;

  template <class T, class U>
  port::Status DoBatchNormalizationForwardImpl(
      Stream* stream, dnn::DataType input_data_type,
      dnn::DataType scale_data_type, const DeviceMemory<T>& x,
      const DeviceMemory<U>& scale, const DeviceMemory<U>& offset,
      const DeviceMemory<U>& estimated_mean,
      const DeviceMemory<U>& estimated_variance,
      const DeviceMemory<T>& side_input, const dnn::BatchDescriptor& x_desc,
      const dnn::BatchDescriptor& scale_offset_desc, const double epsilon,
      const double exponential_average_factor,
      dnn::ActivationMode activation_mode, DeviceMemory<T>* y,
      DeviceMemory<U>* batch_mean, DeviceMemory<U>* batch_var,
      DeviceMemory<U>* saved_mean, DeviceMemory<U>* saved_inv_var,
      bool is_training, ScratchAllocator* reserve_space_allocator,
      ScratchAllocator* workspace_allocator);

  template <class T, class U>
  port::Status DoBatchNormalizationBackwardImpl(
      Stream* stream, int cudnn_input_type, int cudnn_scale_type,
      const DeviceMemory<T>& y_backprop, const DeviceMemory<T>& x,
      const DeviceMemory<U>& scale, const DeviceMemory<U>& mean,
      const DeviceMemory<U>& inv_var, const dnn::BatchDescriptor& x_desc,
      const dnn::BatchDescriptor& scale_offset_desc, const double epsilon,
      DeviceMemory<T>* x_backprop, DeviceMemory<U>* scale_backprop,
      DeviceMemory<U>* offset_backprop, DeviceMemory<uint8>* reserve_space_data,
      ScratchAllocator* workspace_allocator);

  template <typename ScaleType>
  port::Status DoFusedConvolveImpl(
      Stream* stream, dnn::DataType input_type, dnn::DataType side_input_type,
      dnn::DataType bias_type, dnn::DataType output_type,
      const dnn::BatchDescriptor& conv_input_descriptor,
      DeviceMemoryBase conv_input_data, ScaleType conv_input_scale,
      const dnn::FilterDescriptor& filter_descriptor,
      DeviceMemoryBase filter_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      DeviceMemoryBase side_input_data, ScaleType side_input_scale,
      const dnn::BatchDescriptor& bias_descriptor, DeviceMemoryBase biases,
      dnn::ActivationMode activation_mode,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemoryBase output_data, dnn::DataType accumulator_type,
      ScratchAllocator* scratch_allocator,
      const dnn::AlgorithmConfig& algorithm_config,
      dnn::ProfileResult* output_profile_result);

  port::Status DoFusedConvolveWithExecutionPlanImpl(
      Stream* stream, const dnn::BatchDescriptor& conv_input_descriptor,
      DeviceMemoryBase conv_input_data, double conv_input_scale,
      const dnn::FilterDescriptor& filter_descriptor,
      DeviceMemoryBase filter_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      DeviceMemoryBase side_input_data, double side_input_scale,
      const dnn::BatchDescriptor& bias_descriptor, DeviceMemoryBase biases,
      dnn::ActivationMode activation_mode,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemoryBase output_data, dnn::DataType accumulator_type,
      ScratchAllocator* scratch_allocator,
      const dnn::AlgorithmConfig& plan_config,
      dnn::ProfileResult* output_profile_result);

  template <class T>
  port::Status DoConvolveBackwardBiasImpl(
      Stream* stream, const dnn::BatchDescriptor& input_descriptor,
      const DeviceMemory<T>& input_data,
      const dnn::BatchDescriptor& bias_descriptor,
      DeviceMemory<T>* backward_bias_data);

  template <class T>
  port::Status DoRnnForwardImpl(
      Stream* stream, const CudnnRnnDescriptor& rnn_desc,
      const CudnnRnnSequenceTensorDescriptor& input_desc,
      const DeviceMemory<T>& input_data,
      const DeviceMemory<int>& seq_lengths_data,
      const CudnnRnnStateTensorDescriptor& input_h_desc,
      const DeviceMemory<T>& input_h_data,
      const CudnnRnnStateTensorDescriptor& input_c_desc,
      const DeviceMemory<T>& input_c_data, const DeviceMemory<T>& params,
      const CudnnRnnSequenceTensorDescriptor& output_desc,
      DeviceMemory<T>* output_data,
      const CudnnRnnStateTensorDescriptor& output_h_desc,
      DeviceMemory<T>* output_h_data,
      const CudnnRnnStateTensorDescriptor& output_c_desc,
      DeviceMemory<T>* output_c_data, bool is_training,
      ScratchAllocator* reserve_space_allocator,
      ScratchAllocator* workspace_allocator,
      dnn::ProfileResult* output_profile_result);

  template <class T>
  port::Status DoRnnBackwardImpl(
      Stream* stream, const CudnnRnnDescriptor& rnn_desc,
      const CudnnRnnSequenceTensorDescriptor& input_desc,
      const DeviceMemory<T>& input_data,
      const DeviceMemory<int>& seq_lengths_data,
      const CudnnRnnStateTensorDescriptor& input_h_desc,
      const DeviceMemory<T>& input_h_data,
      const CudnnRnnStateTensorDescriptor& input_c_desc,
      const DeviceMemory<T>& input_c_data, const DeviceMemory<T>& params,
      const CudnnRnnSequenceTensorDescriptor& output_desc,
      const DeviceMemory<T>& output_data,
      const CudnnRnnStateTensorDescriptor& output_h_desc,
      const DeviceMemory<T>& output_h_data,
      const CudnnRnnStateTensorDescriptor& output_c_desc,
      const DeviceMemory<T>& output_c_data,
      const DeviceMemory<T>& output_backprop_data,
      const DeviceMemory<T>& output_h_backprop_data,
      const DeviceMemory<T>& output_c_backprop_data,
      DeviceMemory<T>* input_backprop_data,
      DeviceMemory<T>* input_h_backprop_data,
      DeviceMemory<T>* input_c_backprop_data,
      DeviceMemory<T>* params_backprop_data,
      DeviceMemory<uint8>* reserve_space_data,
      ScratchAllocator* workspace_allocator,
      dnn::ProfileResult* output_profile_result);

  port::Status DoCtcLossImpl(
      Stream* stream, const CudnnRnnStateTensorDescriptor& probs_desc,
      const DeviceMemoryBase probs_data, absl::Span<const int> labels_data,
      absl::Span<const int> labels_lengths_data,
      absl::Span<const int> input_lengths_data, DeviceMemoryBase costs_data,
      const CudnnRnnStateTensorDescriptor& grads_desc,
      DeviceMemoryBase grads_data, const CudnnCtcLossDescriptor& ctc_loss_desc,
      DeviceMemory<uint8> scratch_memory, int ctc_loss_algo_id);

 private:
  port::Status DoPrepareForConvolution(
      dnn::ConvolutionKind kind, dnn::DataType element_type, Stream* stream,
      const dnn::BatchDescriptor& input_descriptor, DeviceMemoryBase input_data,
      const dnn::FilterDescriptor& filter_descriptor,
      DeviceMemoryBase filter_data,
      const dnn::BatchDescriptor& output_descriptor,
      DeviceMemoryBase output_data,
      const dnn::ConvolutionDescriptor& convolution_descriptor,
      const dnn::AlgorithmConfig& algorithm_config,
      ScratchAllocator* scratch_allocator, dnn::AlgorithmDesc* algorithm_desc,
      DeviceMemory<uint8>* scratch_memory) override;

  port::Status DoPrepareForCtcLoss(
      Stream* stream, dnn::DataType element_type,
      const dnn::RnnStateTensorDescriptor& probs_desc,
      const dnn::RnnStateTensorDescriptor& grads_desc,
      absl::Span<const int> labels_data,
      absl::Span<const int> labels_lengths_data,
      absl::Span<const int> input_lengths_data,
      ScratchAllocator* scratch_allocator, DeviceMemory<uint8>* scratch_memory,
      int* ctc_loss_algo_id) override;

  SE_DISALLOW_COPY_AND_ASSIGN(CudnnSupport);
};

}  // namespace gpu
}  // namespace stream_executor

#endif  // TENSORFLOW_STREAM_EXECUTOR_CUDA_CUDA_DNN_H_
