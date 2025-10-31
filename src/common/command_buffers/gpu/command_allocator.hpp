// Copyright 2017 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <string_view>
#include <vector>

#include <common/non_copyable.hpp>
#include <common/math_utils.hpp>

namespace commandbuffers::gpu
{
  // These are the lists of blocks, should not be used directly, only through CommandAllocator
  // and CommandIterator
  struct BlockDef
  {
    size_t size;
    std::unique_ptr<char[]> block;
  };
  using CommandBlocks = std::vector<BlockDef>;

  namespace detail
  {
    constexpr uint32_t kEndOfBlock = std::numeric_limits<uint32_t>::max();
    constexpr uint32_t kAdditionalData = std::numeric_limits<uint32_t>::max() - 1;
  }

  class CommandAllocator;

  class CommandIterator : public NonCopyable
  {
  public:
    CommandIterator();
    ~CommandIterator();

    CommandIterator(CommandIterator &&other);
    CommandIterator &operator=(CommandIterator &&other);

    // Shorthand constructor for acquiring CommandBlocks from a single CommandAllocator.
    explicit CommandIterator(CommandAllocator allocator);

    void acquireCommandBlocks(std::vector<CommandAllocator> allocators);

    template <typename E>
    bool nextCommandId(E *commandId)
    {
      return nextCommandId(reinterpret_cast<uint32_t *>(commandId));
    }
    template <typename T>
    T *nextCommand()
    {
      return static_cast<T *>(nextCommand(sizeof(T), alignof(T)));
    }
    template <typename T>
    T *nextData(size_t count)
    {
      return static_cast<T *>(nextData(sizeof(T) * count, alignof(T)));
    }

    // Sets iterator to the beginning of the commands without emptying the list. This method can
    // be used if iteration was stopped early and the iterator needs to be restarted.
    void reset();

    // This method must to be called after commands have been deleted. This indicates that the
    // commands have been submitted and they are no longer valid.
    void makeEmptyAsDataWasDestroyed();

  private:
    bool isEmpty() const;

    inline bool nextCommandId(uint32_t *commandId)
    {
      char *idPtr = transmute::common::AlignPtr(current_ptr_, alignof(uint32_t));
      assert(idPtr == reinterpret_cast<char *>(&end_of_block_) ||
             idPtr + sizeof(uint32_t) <= blocks_[current_block_].block.get() + blocks_[current_block_].size);

      uint32_t id = *reinterpret_cast<uint32_t *>(idPtr);
      if (id != detail::kEndOfBlock)
      {
        current_ptr_ = idPtr + sizeof(uint32_t);
        *commandId = id;
        return true;
      }
      return nextCommandIdInNewBlock(commandId);
    }

    bool nextCommandIdInNewBlock(uint32_t *commandId);

    inline void *nextCommand(size_t commandSize, size_t commandAlignment)
    {
      char *commandPtr = transmute::common::AlignPtr(current_ptr_, commandAlignment);
      assert(commandPtr + sizeof(commandSize) <=
             blocks_[current_block_].block.get() + blocks_[current_block_].size);

      current_ptr_ = commandPtr + commandSize;
      return commandPtr;
    }

    inline void *nextData(size_t dataSize, size_t dataAlignment)
    {
      uint32_t id;
      bool hasId = nextCommandId(&id);
      assert(hasId);
      assert(id == detail::kAdditionalData);

      return nextCommand(dataSize, dataAlignment);
    }

    CommandBlocks blocks_;
    char *current_ptr_ = nullptr;
    size_t current_block_ = 0;
    // Used to avoid a special case for empty iterators.
    uint32_t end_of_block_ = detail::kEndOfBlock;
  };

  class CommandAllocator : public NonCopyable
  {
  public:
    CommandAllocator();
    ~CommandAllocator();

    // NOTE: A moved-from CommandAllocator is reset to its initial empty state.
    CommandAllocator(CommandAllocator &&);
    CommandAllocator &operator=(CommandAllocator &&);

    // Frees all blocks held by the allocator and restores it to its initial empty state.
    void reset();

    bool isEmpty() const;

    template <typename T, typename E>
    T *allocate(E commandId)
    {
      static_assert(sizeof(E) == sizeof(uint32_t));
      static_assert(alignof(E) == alignof(uint32_t));
      static_assert(alignof(T) <= kMaxSupportedAlignment);
      T *result = reinterpret_cast<T *>(allocate(static_cast<uint32_t>(commandId), sizeof(T), alignof(T)));
      if (!result)
      {
        return nullptr;
      }
      new (result) T;
      return result;
    }

    template <typename T>
    T *allocateData(size_t count)
    {
      static_assert(alignof(T) <= kMaxSupportedAlignment);
      T *result = reinterpret_cast<T *>(allocateData(sizeof(T) * count, alignof(T)));
      if (!result)
      {
        return nullptr;
      }
      for (size_t i = 0; i < count; i++)
      {
        new (result + i) T;
      }
      return result;
    }

    size_t getCommandBlocksCount() const;

  private:
    // This is used for some internal computations and can be any power of two as long as code
    // using the CommandAllocator passes the static_asserts.
    static constexpr size_t kMaxSupportedAlignment = 8;

    // To avoid checking for overflows at every step of the computations we compute an upper
    // bound of the space that will be needed in addition to the command data.
    static constexpr size_t kWorstCaseAdditionalSize =
      sizeof(uint32_t) + kMaxSupportedAlignment + alignof(uint32_t) + sizeof(uint32_t);

    // The default value of mLastAllocationSize.
    static constexpr size_t kDefaultBaseAllocationSize = 2048;

    friend CommandIterator;
    CommandBlocks &&acquireBlocks();

    inline char *allocate(uint32_t commandId,
                          size_t commandSize,
                          size_t commandAlignment)
    {
      assert(current_ptr_ != nullptr);
      assert(end_ptr_ != nullptr);
      assert(commandId != detail::kEndOfBlock);

      // It should always be possible to allocate one id, for kEndOfBlock tagging,
      assert(transmute::common::IsPtrAligned(current_ptr_, alignof(uint32_t)));
      assert(end_ptr_ >= current_ptr_);
      assert(static_cast<size_t>(end_ptr_ - current_ptr_) >= sizeof(uint32_t));

      // The memory after the ID will contain the following:
      //   - the current ID
      //   - padding to align the command, maximum kMaxSupportedAlignment
      //   - the command of size commandSize
      //   - padding to align the next ID, maximum alignof(uint32_t)
      //   - the next ID of size sizeof(uint32_t)

      // This can't overflow because by construction current_ptr_ always has space for the next
      // ID.
      size_t remainingSize = static_cast<size_t>(end_ptr_ - current_ptr_);

      // The good case were we have enough space for the command data and upper bound of the
      // extra required space.
      if ((remainingSize >= kWorstCaseAdditionalSize) &&
          (remainingSize - kWorstCaseAdditionalSize >= commandSize))
      {
        uint32_t *idAlloc = reinterpret_cast<uint32_t *>(current_ptr_);
        *idAlloc = commandId;

        char *commandAlloc = transmute::common::AlignPtr(current_ptr_ + sizeof(uint32_t), commandAlignment);
        current_ptr_ = transmute::common::AlignPtr(commandAlloc + commandSize, alignof(uint32_t));

        return commandAlloc;
      }
      return allocateInNewBlock(commandId, commandSize, commandAlignment);
    }

    char *allocateInNewBlock(uint32_t commandId, size_t commandSize, size_t commandAlignment);

    inline char *allocateData(size_t commandSize, size_t commandAlignment)
    {
      return allocate(detail::kAdditionalData, commandSize, commandAlignment);
    }

    bool getNewBlock(size_t minimumSize);

    void resetPointers();

    CommandBlocks blocks_;
    size_t last_allocation_size_ = kDefaultBaseAllocationSize;

    // Data used for the block range at initialization so that the first call to Allocate sees
    // there is not enough space and calls GetNewBlock. This avoids having to special case the
    // initialization in Allocate.
    uint32_t placeholder_space_[1] = {0};
    char *current_ptr_ = nullptr;
    char *end_ptr_ = nullptr;
  };
}
