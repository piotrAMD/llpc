/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
 ***********************************************************************************************************************
 * @file  PatchEntryPointMutate.h
 * @brief LLPC header file: contains declaration of class lgc::PatchEntryPointMutate.
 ***********************************************************************************************************************
 */
#pragma once

#include "lgc/patch/Patch.h"
#include "lgc/state/PipelineState.h"
#include "llvm/IR/InstVisitor.h"

namespace lgc {

class PipelineShaders;

// =====================================================================================================================
// Represents the pass of LLVM patching opertions for entry-point mutation.
class PatchEntryPointMutate : public Patch, public llvm::InstVisitor<PatchEntryPointMutate> {
public:
  PatchEntryPointMutate();

  void getAnalysisUsage(llvm::AnalysisUsage &analysisUsage) const override {
    analysisUsage.addRequired<PipelineStateWrapper>();
    analysisUsage.addRequired<PipelineShaders>();
    // Does not preserve PipelineShaders because it replaces the entrypoints.
  }

  virtual bool runOnModule(llvm::Module &module) override;

  // -----------------------------------------------------------------------------------------------------------------

  static char ID; // ID of this pass

private:
  PatchEntryPointMutate(const PatchEntryPointMutate &) = delete;
  PatchEntryPointMutate &operator=(const PatchEntryPointMutate &) = delete;

  void processShader();

  llvm::FunctionType *generateEntryPointType(uint64_t *inRegMask) const;

  bool isResourceNodeActive(const ResourceNode *node, bool isRootNode) const;

  // -----------------------------------------------------------------------------------------------------------------

  // Reserved argument count for single DWORD descriptor table pointer
  static const unsigned TablePtrReservedArgCount = 2;

  bool m_hasTs; // Whether the pipeline has tessllation shader
  bool m_hasGs; // Whether the pipeline has geometry shader
  PipelineState *m_pipelineState = nullptr;
  // Pipeline state from PipelineStateWrapper pass
};

} // namespace lgc
