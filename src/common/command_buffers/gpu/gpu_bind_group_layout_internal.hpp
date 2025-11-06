#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout_base.hpp>
#include <common/command_buffers/gpu/gpu_binding_info.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;

  // In the BindGroupLayout, entries are sorted by type for more efficient lookup and iteration.
  // This enum is the order that's used and can also be used to index various ranges of entries.
  // The enum is public so that helper function can use it during creation of the BindGroupLayout,
  // but the order is not meant to be used anywhere else. Use the accessors on the BindGroupLayout for
  // logic that relies on the packing or the order.
  enum GPUBindingTypeOrder : uint32_t
  {
    // Buffers
    GPUBindingTypeOrder_DynamicBuffer,
    GPUBindingTypeOrder_RegularBuffer,
    // Textures
    GPUBindingTypeOrder_SampledTexture,
    GPUBindingTypeOrder_StorageTexture,
    GPUBindingTypeOrder_InputAttachment,
    // Samplers
    GPUBindingTypeOrder_StaticSampler,
    GPUBindingTypeOrder_RegularSampler,
    // Texel Buffers
    GPUBindingTypeOrder_TexelBuffer,
    // Start of entries that are expanded in the frontend and aren't actually stored in the bind
    // groups.
    GPUBindingTypeOrder_ExternalTexture,
    GPUBindingTypeOrder_Count,
  };

  class GPUBindGroupLayoutInternalBase : public GPUHandle
  {
    using BindingMap = std::map<GPUBindingNumber, GPUClientBindingIndex>;

  public:
    GPUBindGroupLayoutInternalBase(Ref<GPUDeviceBase> device,
                                   const GPUBindGroupLayoutDescriptor &descriptor);
    virtual ~GPUBindGroupLayoutInternalBase();

    GPUHandleType type() const override
    {
      return GPUHandleType::kBindGroupLayoutInternal;
    }

    // Getters for static bindings
    const GPUBindingInfo &getBindingInfo(GPUBindingIndex bindingIndex) const;
    const GPUBindingInfo &getClientBindingInfo(GPUClientBindingIndex bindingIndex) const;
    const BindingMap &getBindingMap() const;
    GPUBindingIndex asBindingIndex(GPUClientBindingIndex bindingIndex) const;
    GPUClientBindingIndex getClientBindingIndex(GPUBindingNumber bindingNumber) const;
    GPUBindingIndex getBindingCount() const;

  public:
    bool isEmpty() const;

    std::string entriesToString() const;

  private:
    std::unordered_map<GPUBindingIndex, GPUBindingInfo> binding_info_;
    GPUBindingCounts validation_binding_counts_ = {};

    // Keep a list of the start indices for each kind of binding. Then (exclusive) end of a range
    // of bindings is the start of the next range. (that's why we use count + 1 entry, to have the
    // "end" of the last binding type)
    GPUBindingIndex getBindingTypeStart(GPUBindingTypeOrder type) const;
    GPUBindingIndex getBindingTypeEnd(GPUBindingTypeOrder type) const;
    std::array<GPUBindingIndex, GPUBindingTypeOrder_Count + 1> binding_type_start_;

    BindingMap binding_map_;

    // Information about the dynamic binding array part of the BGL.
    bool has_dynamic_array_ = false;
    GPUBindingNumber client_dynamic_array_start{0};
    GPUBindingIndex dynamic_array_start{0};
    GPUBindingIndex dynamic_array_metadata_binding{0};
    GPUDynamicBindingKind dynamic_array_kind = GPUDynamicBindingKind::kUndefined;
  };
}
