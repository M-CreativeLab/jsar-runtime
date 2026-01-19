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

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <new>
#include <utility>

#include <common/command_buffers/gpu/command_allocator.hpp>

using namespace std;

namespace commandbuffers::gpu
{
  // TODO(cwallez@chromium.org): figure out a way to have more type safety for the iterator

  CommandIterator::CommandIterator()
  {
    reset();
  }

  CommandIterator::~CommandIterator()
  {
    assert(isEmpty());
  }

  CommandIterator::CommandIterator(CommandIterator &&other)
  {
    if (!other.isEmpty())
    {
      blocks_ = std::move(other.blocks_);
      other.reset();
    }
    reset();
  }

  CommandIterator &CommandIterator::operator=(CommandIterator &&other)
  {
    assert(isEmpty());
    if (!other.isEmpty())
    {
      blocks_ = std::move(other.blocks_);
      other.reset();
    }
    reset();
    return *this;
  }

  CommandIterator::CommandIterator(CommandAllocator allocator)
      : blocks_(allocator.acquireBlocks())
  {
    reset();
  }

  void CommandIterator::acquireCommandBlocks(std::vector<CommandAllocator> allocators)
  {
    assert(isEmpty());
    blocks_.clear();

    size_t totalBlocksCount = 0;
    for (CommandAllocator &allocator : allocators)
    {
      totalBlocksCount += allocator.getCommandBlocksCount();
    }

    blocks_.reserve(totalBlocksCount);
    for (CommandAllocator &allocator : allocators)
    {
      CommandBlocks blocks = allocator.acquireBlocks();
      if (!blocks.empty())
      {
        for (BlockDef &block : blocks)
        {
          blocks_.push_back(std::move(block));
        }
      }
    }
    reset();
  }

  bool CommandIterator::nextCommandIdInNewBlock(uint32_t *commandId)
  {
    current_block_++;
    if (current_block_ >= blocks_.size())
    {
      reset();
      *commandId = detail::kEndOfBlock;
      return false;
    }
    current_ptr_ = transmute::common::AlignPtr(blocks_[current_block_].block.get(), alignof(uint32_t));
    return nextCommandId(commandId);
  }

  void CommandIterator::reset()
  {
    current_block_ = 0;

    if (blocks_.empty())
    {
      // This will case the first NextCommandId call to try to move to the next block and stop
      // the iteration immediately, without special casing the initialization.
      current_ptr_ = reinterpret_cast<char *>(&end_of_block_);
    }
    else
    {
      current_ptr_ = transmute::common::AlignPtr(blocks_[0].block.get(), alignof(uint32_t));
    }
  }

  void CommandIterator::makeEmptyAsDataWasDestroyed()
  {
    if (isEmpty())
    {
      return;
    }

    current_ptr_ = reinterpret_cast<char *>(&end_of_block_);
    blocks_.clear();
    reset();
    assert(isEmpty());
  }

  bool CommandIterator::isEmpty() const
  {
    return blocks_.empty();
  }

  // Potential TODO(crbug.com/dawn/835):
  //  - Host the size and pointer to next block in the block itself to avoid having an allocation
  //    in the vector
  //  - Assume T's alignof is, say 64bits, static assert it, and make commandAlignment a constant
  //    in Allocate
  //  - Be able to optimize allocation to one block, for command buffers expected to live long to
  //    avoid cache misses
  //  - Better block allocation, maybe have Dawn API to say command buffer is going to have size
  //    close to another

  CommandAllocator::CommandAllocator()
  {
    resetPointers();
  }

  CommandAllocator::~CommandAllocator()
  {
    reset();
  }

  CommandAllocator::CommandAllocator(CommandAllocator &&other)
      : blocks_(std::move(other.blocks_))
      , last_allocation_size_(other.last_allocation_size_)
  {
    other.blocks_.clear();
    if (!other.isEmpty())
    {
      current_ptr_ = other.current_ptr_;
      end_ptr_ = other.end_ptr_;
    }
    else
    {
      resetPointers();
    }
    other.reset();
  }

  CommandAllocator &CommandAllocator::operator=(CommandAllocator &&other)
  {
    reset();
    if (!other.isEmpty())
    {
      std::swap(blocks_, other.blocks_);
      last_allocation_size_ = other.last_allocation_size_;
      current_ptr_ = other.current_ptr_;
      end_ptr_ = other.end_ptr_;
    }
    other.reset();
    return *this;
  }

  void CommandAllocator::reset()
  {
    resetPointers();
    blocks_.clear();
    last_allocation_size_ = kDefaultBaseAllocationSize;
  }

  bool CommandAllocator::isEmpty() const
  {
    return current_ptr_ == reinterpret_cast<const char *>(&placeholder_space_[0]);
  }

  size_t CommandAllocator::getCommandBlocksCount() const
  {
    return blocks_.size();
  }

  CommandBlocks &&CommandAllocator::acquireBlocks()
  {
    assert(current_ptr_ != nullptr && end_ptr_ != nullptr);
    assert(transmute::common::IsPtrAligned(current_ptr_, alignof(uint32_t)));
    assert(current_ptr_ + sizeof(uint32_t) <= end_ptr_);
    *reinterpret_cast<uint32_t *>(current_ptr_) = detail::kEndOfBlock;

    current_ptr_ = nullptr;
    end_ptr_ = nullptr;
    return std::move(blocks_);
  }

  char *CommandAllocator::allocateInNewBlock(uint32_t commandId,
                                             size_t commandSize,
                                             size_t commandAlignment)
  {
    // When there is not enough space, we signal the kEndOfBlock, so that the iterator knows
    // to move to the next one. kEndOfBlock on the last block means the end of the commands.
    uint32_t *idAlloc = reinterpret_cast<uint32_t *>(current_ptr_);
    *idAlloc = detail::kEndOfBlock;

    // We'll request a block that can contain at least the command ID, the command and an
    // additional ID to contain the kEndOfBlock tag.
    size_t requestedBlockSize = commandSize + kWorstCaseAdditionalSize;

    // The computation of the request could overflow.
    if (requestedBlockSize <= commandSize) [[unlikely]]
    {
      return nullptr;
    }

    if (!getNewBlock(requestedBlockSize)) [[unlikely]]
    {
      return nullptr;
    }
    return allocate(commandId, commandSize, commandAlignment);
  }

  bool CommandAllocator::getNewBlock(size_t minimumSize)
  {
    // Allocate blocks doubling sizes each time, to a maximum of 16k (or at least minimumSize).
    last_allocation_size_ = std::max(minimumSize, std::min(last_allocation_size_ * 2, size_t(16384)));

    auto block = std::unique_ptr<char[]>(new (std::nothrow) char[last_allocation_size_]);
    if (block == nullptr) [[unlikely]]
    {
      return false;
    }

    current_ptr_ = transmute::common::AlignPtr(block.get(), alignof(uint32_t));
    end_ptr_ = block.get() + last_allocation_size_;
    blocks_.push_back({last_allocation_size_, std::move(block)});
    return true;
  }

  void CommandAllocator::resetPointers()
  {
    current_ptr_ = reinterpret_cast<char *>(&placeholder_space_[0]);
    end_ptr_ = reinterpret_cast<char *>(&placeholder_space_[1]);
  }
}
