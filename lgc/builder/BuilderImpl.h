/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2019-2020 Advanced Micro Devices, Inc. All Rights Reserved.
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
 * @file  BuilderImpl.h
 * @brief LLPC header file: declaration of lgc::Builder implementation classes
 ***********************************************************************************************************************
 */
#pragma once

#include "PipelineState.h"
#include "lgc/Builder.h"

namespace lgc {

// =====================================================================================================================
// Builder implementation base class
class BuilderImplBase : public Builder {
public:
  BuilderImplBase(BuilderContext *builderContext) : Builder(builderContext) {}

  // Create scalar from dot product of vector
  llvm::Value *CreateDotProduct(llvm::Value *const vector1, llvm::Value *const vector2,
                                const llvm::Twine &instName = "") override final;

protected:
  // Get the ShaderModes object.
  ShaderModes *getShaderModes() override final;

  // Get the PipelineState object.
  PipelineState *getPipelineState() const { return m_pipelineState; }

  // Get whether the context we are building in supports DPP operations.
  bool supportDpp() const;

  // Get whether the context we are building in support the bpermute operation.
  bool supportBPermute() const;

  // Get whether the context we are building in supports permute lane DPP operations.
  bool supportPermLaneDpp() const;

  // Create an "if..endif" or "if..else..endif" structure.
  llvm::BranchInst *createIf(llvm::Value *condition, bool wantElse, const llvm::Twine &instName);

  // Create a waterfall loop containing the specified instruction.
  llvm::Instruction *createWaterfallLoop(llvm::Instruction *nonUniformInst, llvm::ArrayRef<unsigned> operandIdxs,
                                         const llvm::Twine &instName = "");

  // Helper method to scalarize a possibly vector unary operation
  llvm::Value *scalarize(llvm::Value *value, std::function<llvm::Value *(llvm::Value *)> callback);

  // Helper method to scalarize in pairs a possibly vector unary operation.
  llvm::Value *scalarizeInPairs(llvm::Value *value, std::function<llvm::Value *(llvm::Value *)> callback);

  // Helper method to scalarize a possibly vector binary operation
  llvm::Value *scalarize(llvm::Value *value0, llvm::Value *value1,
                         std::function<llvm::Value *(llvm::Value *, llvm::Value *)> callback);

  // Helper method to scalarize a possibly vector trinary operation
  llvm::Value *scalarize(llvm::Value *value0, llvm::Value *value1, llvm::Value *value2,
                         std::function<llvm::Value *(llvm::Value *, llvm::Value *, llvm::Value *)> callback);

  PipelineState *m_pipelineState = nullptr; // Pipeline state

private:
  BuilderImplBase() = delete;
  BuilderImplBase(const BuilderImplBase &) = delete;
  BuilderImplBase &operator=(const BuilderImplBase &) = delete;
};

// =====================================================================================================================
// Builder implementation subclass for arithmetic operations
class BuilderImplArith : virtual public BuilderImplBase {
public:
  BuilderImplArith(BuilderContext *builderContext) : BuilderImplBase(builderContext) {}

  // Create calculation of 2D texture coordinates that would be used for accessing the selected cube map face for
  // the given cube map texture coordinates.
  llvm::Value *CreateCubeFaceCoord(llvm::Value *coord, const llvm::Twine &instName = "") override final;

  // Create calculation of the index of the cube map face that would be accessed by a texture lookup function for
  // the given cube map texture coordinates.
  llvm::Value *CreateCubeFaceIndex(llvm::Value *coord, const llvm::Twine &instName = "") override final;

  // Create scalar or vector FP truncate operation with rounding mode.
  llvm::Value *CreateFpTruncWithRounding(llvm::Value *value, llvm::Type *destTy, unsigned roundingMode,
                                         const llvm::Twine &instName = "") override final;

  // Create quantize operation.
  llvm::Value *CreateQuantizeToFp16(llvm::Value *value, const llvm::Twine &instName = "") override final;

  // Create signed integer or FP modulo operation.
  llvm::Value *CreateSMod(llvm::Value *dividend, llvm::Value *divisor, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateFMod(llvm::Value *dividend, llvm::Value *divisor, const llvm::Twine &instName = "") override final;

  // Create scalar/vector float/half fused multiply-and-add, to compute a * b + c
  llvm::Value *CreateFma(llvm::Value *a, llvm::Value *b, llvm::Value *c,
                         const llvm::Twine &instName = "") override final;

  // Methods to create trig and exponential operations.
  llvm::Value *CreateTan(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateASin(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateACos(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateATan(llvm::Value *yOverX, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateATan2(llvm::Value *y, llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateSinh(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateCosh(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateTanh(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateASinh(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateACosh(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateATanh(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreatePower(llvm::Value *x, llvm::Value *y, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateExp(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateLog(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateInverseSqrt(llvm::Value *x, const llvm::Twine &instName = "") override final;

  // General arithmetic operations.
  llvm::Value *CreateSAbs(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateFSign(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateSSign(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateFract(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateSmoothStep(llvm::Value *edge0, llvm::Value *edge1, llvm::Value *x,
                                const llvm::Twine &instName = "") override final;
  llvm::Value *CreateLdexp(llvm::Value *x, llvm::Value *exp, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateExtractSignificand(llvm::Value *value, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateExtractExponent(llvm::Value *value, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateCrossProduct(llvm::Value *x, llvm::Value *y, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateNormalizeVector(llvm::Value *x, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateFaceForward(llvm::Value *n, llvm::Value *i, llvm::Value *nref,
                                 const llvm::Twine &instName = "") override final;
  llvm::Value *CreateReflect(llvm::Value *i, llvm::Value *n, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateRefract(llvm::Value *i, llvm::Value *n, llvm::Value *eta,
                             const llvm::Twine &instName = "") override final;

  // Create "fclamp" operation.
  llvm::Value *CreateFClamp(llvm::Value *x, llvm::Value *minVal, llvm::Value *maxVal,
                            const llvm::Twine &instName = "") override final;

  // FP min/max
  llvm::Value *CreateFMin(llvm::Value *value1, llvm::Value *value2, const llvm::Twine &instName = "") override final;
  llvm::Value *CreateFMax(llvm::Value *value1, llvm::Value *value2, const llvm::Twine &instName = "") override final;

  // Methods for trinary min/max/mid.
  llvm::Value *CreateFMin3(llvm::Value *value1, llvm::Value *value2, llvm::Value *value3,
                           const llvm::Twine &instName = "") override final;
  llvm::Value *CreateFMax3(llvm::Value *value1, llvm::Value *value2, llvm::Value *value3,
                           const llvm::Twine &instName = "") override final;
  llvm::Value *CreateFMid3(llvm::Value *value1, llvm::Value *value2, llvm::Value *value3,
                           const llvm::Twine &instName = "") override final;

  // Create "isInf" operation: return true if the supplied FP (or vector) value is infinity
  llvm::Value *CreateIsInf(llvm::Value *x, const llvm::Twine &instName = "") override final;

  // Create "isNaN" operation: return true if the supplied FP (or vector) value is NaN
  llvm::Value *CreateIsNaN(llvm::Value *x, const llvm::Twine &instName = "") override final;

  // Create an "insert bitfield" operation for a (vector of) integer type.
  llvm::Value *CreateInsertBitField(llvm::Value *base, llvm::Value *insert, llvm::Value *offset, llvm::Value *count,
                                    const llvm::Twine &instName = "") override final;

  // Create an "extract bitfield " operation for a (vector of) i32.
  llvm::Value *CreateExtractBitField(llvm::Value *base, llvm::Value *offset, llvm::Value *count, bool isSigned,
                                     const llvm::Twine &instName = "") override final;

  // Create "find MSB" operation for a (vector of) signed int.
  llvm::Value *CreateFindSMsb(llvm::Value *value, const llvm::Twine &instName = "") override final;

  // Create "fmix" operation.
  llvm::Value *createFMix(llvm::Value *x, llvm::Value *y, llvm::Value *a,
                          const llvm::Twine &instName = "") override final;

private:
  BuilderImplArith() = delete;
  BuilderImplArith(const BuilderImplArith &) = delete;
  BuilderImplArith &operator=(const BuilderImplArith &) = delete;

  // Common code for asin and acos
  llvm::Value *aSinACosCommon(llvm::Value *x, llvm::Constant *coefP0, llvm::Constant *coefP1);

  // Generate FP division, using fast fdiv for float to bypass optimization.
  llvm::Value *fDivFast(llvm::Value *numerator, llvm::Value *denominator);

  // Helper method to create call to llvm.amdgcn.class, scalarizing if necessary. This is not exposed outside of
  // BuilderImplArith.
  llvm::Value *createCallAmdgcnClass(llvm::Value *value, unsigned flags, const llvm::Twine &instName = "");

  // Methods to get various FP constants as scalar or vector. Any needed directly by a client should be moved
  // to Builder.h. Using these (rather than just using for example
  // ConstantFP::get(.., M_PI)) ensures that we always get the same value, independent of the
  // host platform and its compiler.
  // TODO: Use values that are suitable for doubles.

  // Get PI = 3.14159274 scalar or vector
  llvm::Constant *getPi(llvm::Type *ty) {
    return getFpConstant(ty, llvm::APFloat(llvm::APFloat::IEEEdouble(), llvm::APInt(64, 0x400921FB60000000)));
  }

  // Get PI/2 = 1.57079637 scalar or vector
  llvm::Constant *getPiByTwo(llvm::Type *ty) {
    return getFpConstant(ty, llvm::APFloat(llvm::APFloat::IEEEdouble(), llvm::APInt(64, 0x3FF921FB60000000)));
  }

  // Get PI/4 - 1 = -0.21460181 scalar or vector
  llvm::Constant *getPiByFourMinusOne(llvm::Type *ty) {
    return getFpConstant(ty, llvm::APFloat(llvm::APFloat::IEEEdouble(), llvm::APInt(64, 0xBFCB781280000000)));
  }

  // Get 1/log(2) = 1.442695 scalar or vector
  llvm::Constant *getRecipLog2(llvm::Type *ty) {
    return getFpConstant(ty, llvm::APFloat(llvm::APFloat::IEEEdouble(), llvm::APInt(64, 0x3FF7154760000000)));
  }

  // Get 0.5 * log(2) = 0.34657359 scalar or vector
  llvm::Constant *getHalfLog2(llvm::Type *ty) {
    return getFpConstant(ty, llvm::APFloat(llvm::APFloat::IEEEdouble(), llvm::APInt(64, 0x3FD62E4300000000)));
  }

  // Get log(2) = 0.6931471824646 scalar or vector
  llvm::Constant *getLog2(llvm::Type *ty) {
    return getFpConstant(ty, llvm::APFloat(llvm::APFloat::IEEEdouble(), llvm::APInt(64, 0x3FE62E4300000000)));
  }

  // Get 2^-15 (normalized float16 minimum) scalar or vector
  llvm::Constant *getMinNormalizedF16(llvm::Type *ty) { return llvm::ConstantFP::get(ty, 0.000030517578125); }

  // Ensure result is canonicalized if the shader's FP mode is flush denorms.
  llvm::Value *canonicalize(llvm::Value *value);
};

// =====================================================================================================================
// Builder implementation subclass for descriptors
class BuilderImplDesc : virtual public BuilderImplBase {
public:
  BuilderImplDesc(BuilderContext *builderContext) : BuilderImplBase(builderContext) {}

  // Create a load of a buffer descriptor.
  llvm::Value *CreateLoadBufferDesc(unsigned descSet, unsigned binding, llvm::Value *descIndex, bool isNonUniform,
                                    bool isWritten, llvm::Type *pointeeTy, const llvm::Twine &instName) override final;

  // Add index onto pointer to image/sampler/texelbuffer/F-mask array of descriptors.
  llvm::Value *CreateIndexDescPtr(llvm::Value *descPtr, llvm::Value *index, bool isNonUniform,
                                  const llvm::Twine &instName) override final;

  // Load image/sampler/texelbuffer/F-mask descriptor from pointer.
  llvm::Value *CreateLoadDescFromPtr(llvm::Value *descPtr, const llvm::Twine &instName) override final;

  // Create a pointer to sampler descriptor. Returns a value of the type returned by GetSamplerDescPtrTy.
  llvm::Value *CreateGetSamplerDescPtr(unsigned descSet, unsigned binding, const llvm::Twine &instName) override final;

  // Create a pointer to image descriptor. Returns a value of the type returned by GetImageDescPtrTy.
  llvm::Value *CreateGetImageDescPtr(unsigned descSet, unsigned binding, const llvm::Twine &instName) override final;

  // Create a pointer to texel buffer descriptor. Returns a value of the type returned by GetTexelBufferDescPtrTy.
  llvm::Value *CreateGetTexelBufferDescPtr(unsigned descSet, unsigned binding,
                                           const llvm::Twine &instName) override final;

  // Create a pointer to F-mask descriptor. Returns a value of the type returned by GetFmaskDescPtrTy.
  llvm::Value *CreateGetFmaskDescPtr(unsigned descSet, unsigned binding, const llvm::Twine &instName) override final;

  // Create a load of the push constants pointer.
  llvm::Value *CreateLoadPushConstantsPtr(llvm::Type *pushConstantsTy, const llvm::Twine &instName) override final;

  // Create a buffer length query based on the specified descriptor.
  llvm::Value *CreateGetBufferDescLength(llvm::Value *const bufferDesc,
                                         const llvm::Twine &instName = "") override final;

private:
  BuilderImplDesc() = delete;
  BuilderImplDesc(const BuilderImplDesc &) = delete;
  BuilderImplDesc &operator=(const BuilderImplDesc &) = delete;

  llvm::Value *scalarizeIfUniform(llvm::Value *value, bool isNonUniform);
};

// =====================================================================================================================
// Builder implementation subclass for image operations
class BuilderImplImage : virtual public BuilderImplBase {
public:
  BuilderImplImage(BuilderContext *builderContext) : BuilderImplBase(builderContext) {}

  // Create an image load.
  llvm::Value *CreateImageLoad(llvm::Type *resultTy, unsigned dim, unsigned flags, llvm::Value *imageDesc,
                               llvm::Value *coord, llvm::Value *mipLevel,
                               const llvm::Twine &instName = "") override final;

  // Create an image load with F-mask.
  llvm::Value *CreateImageLoadWithFmask(llvm::Type *resultTy, unsigned dim, unsigned flags, llvm::Value *imageDesc,
                                        llvm::Value *fmaskDesc, llvm::Value *coord, llvm::Value *sampleNum,
                                        const llvm::Twine &instName = "") override final;

  // Create an image store.
  llvm::Value *CreateImageStore(llvm::Value *texel, unsigned dim, unsigned flags, llvm::Value *imageDesc,
                                llvm::Value *coord, llvm::Value *mipLevel,
                                const llvm::Twine &instName = "") override final;

  // Create an image sample.
  llvm::Value *CreateImageSample(llvm::Type *resultTy, unsigned dim, unsigned flags, llvm::Value *imageDesc,
                                 llvm::Value *samplerDesc, llvm::ArrayRef<llvm::Value *> address,
                                 const llvm::Twine &instName = "") override final;

  // Create an image sample with conversion.
  // This is not yet a Builder API, but it could become one if there was to be a new SPIR-V YCbCr
  // converting sampler spec that allows the SPIR-V reader to tell that it has a converting sampler.
  llvm::Value *CreateImageSampleConvert(llvm::Type *resultTy, unsigned dim, unsigned flags, llvm::Value *imageDesc,
                                        llvm::Value *convertingSamplerDesc, llvm::ArrayRef<llvm::Value *> address,
                                        const llvm::Twine &instName = "");

  // Create an image gather
  llvm::Value *CreateImageGather(llvm::Type *resultTy, unsigned dim, unsigned flags, llvm::Value *imageDesc,
                                 llvm::Value *samplerDesc, llvm::ArrayRef<llvm::Value *> address,
                                 const llvm::Twine &instName = "") override final;

  // Create an image atomic operation other than compare-and-swap.
  llvm::Value *CreateImageAtomic(unsigned atomicOp, unsigned dim, unsigned flags, llvm::AtomicOrdering ordering,
                                 llvm::Value *imageDesc, llvm::Value *coord, llvm::Value *inputValue,
                                 const llvm::Twine &instName = "") override final;

  // Create an image atomic compare-and-swap.
  llvm::Value *CreateImageAtomicCompareSwap(unsigned dim, unsigned flags, llvm::AtomicOrdering ordering,
                                            llvm::Value *imageDesc, llvm::Value *coord, llvm::Value *inputValue,
                                            llvm::Value *comparatorValue,
                                            const llvm::Twine &instName = "") override final;

  // Create a query of the number of mipmap levels in an image. Returns an i32 value.
  llvm::Value *CreateImageQueryLevels(unsigned dim, unsigned flags, llvm::Value *imageDesc,
                                      const llvm::Twine &instName = "") override final;

  // Create a query of the number of samples in an image. Returns an i32 value.
  llvm::Value *CreateImageQuerySamples(unsigned dim, unsigned flags, llvm::Value *imageDesc,
                                       const llvm::Twine &instName = "") override final;

  // Create a query of size of an image at the specified LOD
  llvm::Value *CreateImageQuerySize(unsigned dim, unsigned flags, llvm::Value *imageDesc, llvm::Value *lod,
                                    const llvm::Twine &instName = "") override final;

  // Create a get of the LOD that would be used for an image sample with the given coordinates
  // and implicit LOD.
  llvm::Value *CreateImageGetLod(unsigned dim, unsigned flags, llvm::Value *imageDesc, llvm::Value *samplerDesc,
                                 llvm::Value *coord, const llvm::Twine &instName = "") override final;

private:
  BuilderImplImage() = delete;
  BuilderImplImage(const BuilderImplImage &) = delete;
  BuilderImplImage &operator=(const BuilderImplImage &) = delete;

  // Implement pre-GFX9 integer gather workaround to patch descriptor or coordinate before the gather
  llvm::Value *preprocessIntegerImageGather(unsigned dim, llvm::Value *&imageDesc, llvm::Value *&coord);

  // Implement pre-GFX9 integer gather workaround to modify result.
  llvm::Value *postprocessIntegerImageGather(llvm::Value *needDescPatch, unsigned flags, llvm::Value *imageDesc,
                                             llvm::Type *texelTy, llvm::Value *result);

  // Common code to create an image sample or gather.
  llvm::Value *CreateImageSampleGather(llvm::Type *resultTy, unsigned dim, unsigned flags, llvm::Value *coord,
                                       llvm::Value *imageDesc, llvm::Value *samplerDesc,
                                       llvm::ArrayRef<llvm::Value *> address, const llvm::Twine &instName,
                                       bool isSample);

  // Common code for CreateImageAtomic and CreateImageAtomicCompareSwap
  llvm::Value *CreateImageAtomicCommon(unsigned atomicOp, unsigned dim, unsigned flags, llvm::AtomicOrdering ordering,
                                       llvm::Value *imageDesc, llvm::Value *coord, llvm::Value *inputValue,
                                       llvm::Value *comparatorValue, const llvm::Twine &instName);

  // Change 1D or 1DArray dimension to 2D or 2DArray if needed as a workaround on GFX9+
  unsigned change1DTo2DIfNeeded(unsigned dim);

  // Prepare coordinate and explicit derivatives, pushing the separate components into the supplied vectors, and
  // modifying if necessary.
  // Returns possibly modified image dimension.
  unsigned prepareCoordinate(unsigned dim, llvm::Value *coord, llvm::Value *projective, llvm::Value *derivativeX,
                             llvm::Value *derivativeY, llvm::SmallVectorImpl<llvm::Value *> &outCoords,
                             llvm::SmallVectorImpl<llvm::Value *> &outDerivatives);

  // For a cubearray with integer coordinates, combine the face and slice into a single component.
  void combineCubeArrayFaceAndSlice(llvm::Value *coord, llvm::SmallVectorImpl<llvm::Value *> &coords);

  // Patch descriptor with cube dimension for image call
  llvm::Value *patchCubeDescriptor(llvm::Value *desc, unsigned dim);

  // Handle cases where we need to add the FragCoord x,y to the coordinate, and use ViewIndex as the z coordinate.
  llvm::Value *handleFragCoordViewIndex(llvm::Value *coord, unsigned flags, unsigned &dim);

  // -----------------------------------------------------------------------------------------------------------------

  enum ImgDataFormat {
    IMG_DATA_FORMAT_32 = 4,
    IMG_DATA_FORMAT_32_32 = 11,
    IMG_DATA_FORMAT_32_32_32_32 = 14,
  };

  static const unsigned AtomicOpCompareSwap = 1;
};

// =====================================================================================================================
// Builder implementation subclass for input/output operations
class BuilderImplInOut : virtual public BuilderImplBase {
public:
  BuilderImplInOut(BuilderContext *builderContext) : BuilderImplBase(builderContext) {}

  // Create a read of (part of) a user input value.
  llvm::Value *CreateReadGenericInput(llvm::Type *resultTy, unsigned location, llvm::Value *locationOffset,
                                      llvm::Value *elemIdx, unsigned locationCount, InOutInfo inputInfo,
                                      llvm::Value *vertexIndex, const llvm::Twine &instName = "") override final;

  // Create a read of (part of) a user output value.
  llvm::Value *CreateReadGenericOutput(llvm::Type *resultTy, unsigned location, llvm::Value *locationOffset,
                                       llvm::Value *elemIdx, unsigned locationCount, InOutInfo outputInfo,
                                       llvm::Value *vertexIndex, const llvm::Twine &instName = "") override final;

  // Create a write of (part of) a user output value.
  llvm::Instruction *CreateWriteGenericOutput(llvm::Value *valueToWrite, unsigned location, llvm::Value *locationOffset,
                                              llvm::Value *elemIdx, unsigned locationCount, InOutInfo outputInfo,
                                              llvm::Value *vertexIndex) override final;

  // Create a write to an XFB (transform feedback / streamout) buffer.
  llvm::Instruction *CreateWriteXfbOutput(llvm::Value *valueToWrite, bool isBuiltIn, unsigned location,
                                          unsigned xfbBuffer, unsigned xfbStride, llvm::Value *xfbOffset,
                                          InOutInfo outputInfo) override final;

  // Create a read of (part of) a built-in input value.
  llvm::Value *CreateReadBuiltInInput(BuiltInKind builtIn, InOutInfo inputInfo, llvm::Value *vertexIndex,
                                      llvm::Value *index, const llvm::Twine &instName = "") override final;

  // Create a read of (part of) an output built-in value.
  llvm::Value *CreateReadBuiltInOutput(BuiltInKind builtIn, InOutInfo outputInfo, llvm::Value *vertexIndex,
                                       llvm::Value *index, const llvm::Twine &instName = "") override final;

  // Create a write of (part of) a built-in output value.
  llvm::Instruction *CreateWriteBuiltInOutput(llvm::Value *valueToWrite, BuiltInKind builtIn, InOutInfo outputInfo,
                                              llvm::Value *vertexIndex, llvm::Value *index) override final;

  // Get name of built-in
  static llvm::StringRef getBuiltInName(BuiltInKind builtIn);

private:
  BuilderImplInOut() = delete;
  BuilderImplInOut(const BuilderImplInOut &) = delete;
  BuilderImplInOut &operator=(const BuilderImplInOut &) = delete;

  // Read (a part of) a generic (user) input/output value.
  llvm::Value *readGenericInputOutput(bool isOutput, llvm::Type *resultTy, unsigned location,
                                      llvm::Value *locationOffset, llvm::Value *elemIdx, unsigned locationCount,
                                      InOutInfo inOutInfo, llvm::Value *vertexIndex, const llvm::Twine &instName);

  // Mark usage for a generic (user) input or output
  void markGenericInputOutputUsage(bool isOutput, unsigned location, unsigned locationCount, InOutInfo inOutInfo,
                                   llvm::Value *vertexIndex);

  // Mark interpolation info for FS input.
  void markInterpolationInfo(InOutInfo interpInfo);

  // Mark fragment output type
  void markFsOutputType(llvm::Type *outputTy, unsigned location, InOutInfo outputInfo);

  // Modify aux interp value according to custom interp mode, and its helper functions.
  llvm::Value *modifyAuxInterpValue(llvm::Value *auxInterpValue, InOutInfo inputInfo);
  llvm::Value *evalIjOffsetNoPersp(llvm::Value *offset);
  llvm::Value *evalIjOffsetSmooth(llvm::Value *offset);
  llvm::Value *adjustIj(llvm::Value *value, llvm::Value *offset);

  // Read (part of) a built-in value
  llvm::Value *readBuiltIn(bool isOutput, BuiltInKind builtIn, InOutInfo inOutInfo, llvm::Value *vertexIndex,
                           llvm::Value *index, const llvm::Twine &instName);

  // Get the type of a built-in. This overrides the one in Builder to additionally recognize the internal built-ins.
  llvm::Type *getBuiltInTy(BuiltInKind builtIn, InOutInfo inOutInfo);

  // Mark usage of a built-in input
  void markBuiltInInputUsage(BuiltInKind builtIn, unsigned arraySize);

  // Mark usage of a built-in output
  void markBuiltInOutputUsage(BuiltInKind builtIn, unsigned arraySize, unsigned streamId);

#ifndef NDEBUG
  // Get a bitmask of which shader stages are valid for a built-in to be an input or output of
  unsigned getBuiltInValidMask(BuiltInKind builtIn, bool isOutput);

  // Determine whether a built-in is an input for a particular shader stage.
  bool isBuiltInInput(BuiltInKind builtIn);

  // Determine whether a built-in is an output for a particular shader stage.
  bool isBuiltInOutput(BuiltInKind builtIn);
#endif // NDEBUG
};

// =====================================================================================================================
// Builder implementation subclass for matrix operations
class BuilderImplMatrix : virtual public BuilderImplBase {
public:
  BuilderImplMatrix(BuilderContext *builderContext) : BuilderImplBase(builderContext) {}

  // Create a matrix transpose.
  llvm::Value *CreateTransposeMatrix(llvm::Value *const matrix, const llvm::Twine &instName = "") override final;

  // Create matrix multiplication: matrix times scalar, resulting in matrix
  llvm::Value *CreateMatrixTimesScalar(llvm::Value *const matrix, llvm::Value *const scalar,
                                       const llvm::Twine &instName = "") override final;

  // Create matrix multiplication: vector times matrix, resulting in vector
  llvm::Value *CreateVectorTimesMatrix(llvm::Value *const vector, llvm::Value *const matrix,
                                       const llvm::Twine &instName = "") override final;

  // Create matrix multiplication: matrix times vector, resulting in vector
  llvm::Value *CreateMatrixTimesVector(llvm::Value *const matrix, llvm::Value *const vector,
                                       const llvm::Twine &instName = "") override final;

  // Create matrix multiplication:  matrix times matrix, resulting in matrix
  llvm::Value *CreateMatrixTimesMatrix(llvm::Value *const matrix1, llvm::Value *const matrix2,
                                       const llvm::Twine &instName = "") override final;

  // Create vector outer product operation, resulting in matrix
  llvm::Value *CreateOuterProduct(llvm::Value *const vector1, llvm::Value *const vector2,
                                  const llvm::Twine &instName = "") override final;

  // Create matrix determinant operation.
  llvm::Value *CreateDeterminant(llvm::Value *const matrix, const llvm::Twine &instName = "") override final;

  // Create matrix inverse operation.
  llvm::Value *CreateMatrixInverse(llvm::Value *const matrix, const llvm::Twine &instName = "") override final;

private:
  BuilderImplMatrix() = delete;
  BuilderImplMatrix(const BuilderImplMatrix &) = delete;
  BuilderImplMatrix &operator=(const BuilderImplMatrix &) = delete;

  // Helper function for determinant calculation
  llvm::Value *determinant(llvm::ArrayRef<llvm::Value *> elements, unsigned order);

  // Get submatrix by deleting specified row and column
  void getSubmatrix(llvm::ArrayRef<llvm::Value *> matrix, llvm::MutableArrayRef<llvm::Value *> submatrix,
                    unsigned order, unsigned rowToDelete, unsigned columnToDelete);
};

// =====================================================================================================================
// Builder implementation subclass for misc. operations
class BuilderImplMisc : virtual public BuilderImplBase {
public:
  BuilderImplMisc(BuilderContext *builderContext) : BuilderImplBase(builderContext) {}

  // In the GS, emit the current values of outputs (as written by CreateWriteBuiltIn and CreateWriteOutput) to
  // the current output primitive in the specified output-primitive stream.
  llvm::Instruction *CreateEmitVertex(unsigned streamId) override final;

  // In the GS, finish the current primitive and start a new one in the specified output-primitive stream.
  llvm::Instruction *CreateEndPrimitive(unsigned streamId) override final;

  // Create a workgroup control barrier.
  llvm::Instruction *CreateBarrier() override final;

  // Create a "kill". Only allowed in a fragment shader.
  llvm::Instruction *CreateKill(const llvm::Twine &instName) override final;

  // Create a "readclock".
  llvm::Instruction *CreateReadClock(bool realtime, const llvm::Twine &instName) override final;

  // Create derivative calculation on float or vector of float or half
  llvm::Value *CreateDerivative(llvm::Value *value, bool isDirectionY, bool isFine,
                                const llvm::Twine &instName = "") override final;

  // Create a demote to helper invocation operation. Only allowed in a fragment shader.
  llvm::Instruction *CreateDemoteToHelperInvocation(const llvm::Twine &instName) override final;

  // Create a helper invocation query. Only allowed in a fragment shader.
  llvm::Value *CreateIsHelperInvocation(const llvm::Twine &instName) override final;

private:
  BuilderImplMisc() = delete;
  BuilderImplMisc(const BuilderImplMisc &) = delete;
  BuilderImplMisc &operator=(const BuilderImplMisc &) = delete;
};

// =====================================================================================================================
// Builder implementation subclass for subgroup operations
class BuilderImplSubgroup : virtual public BuilderImplBase {
public:
  BuilderImplSubgroup(BuilderContext *builderContext) : BuilderImplBase(builderContext) {}

  // Create a get subgroup size query.
  llvm::Value *CreateGetSubgroupSize(const llvm::Twine &instName) override final;

  // Create a subgroup elect.
  llvm::Value *CreateSubgroupElect(const llvm::Twine &instName) override final;

  // Create a subgroup all.
  llvm::Value *CreateSubgroupAll(llvm::Value *const value, bool wqm, const llvm::Twine &instName) override final;

  // Create a subgroup any
  llvm::Value *CreateSubgroupAny(llvm::Value *const value, bool wqm, const llvm::Twine &instName) override final;

  // Create a subgroup all equal.
  llvm::Value *CreateSubgroupAllEqual(llvm::Value *const value, bool wqm, const llvm::Twine &instName) override final;

  // Create a subgroup broadcast.
  llvm::Value *CreateSubgroupBroadcast(llvm::Value *const value, llvm::Value *const index,
                                       const llvm::Twine &instName) override final;

  // Create a subgroup broadcast first.
  llvm::Value *CreateSubgroupBroadcastFirst(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup ballot.
  llvm::Value *CreateSubgroupBallot(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup inverse ballot.
  llvm::Value *CreateSubgroupInverseBallot(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup ballot bit extract.
  llvm::Value *CreateSubgroupBallotBitExtract(llvm::Value *const value, llvm::Value *const index,
                                              const llvm::Twine &instName) override final;

  // Create a subgroup ballot bit count.
  llvm::Value *CreateSubgroupBallotBitCount(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup ballot inclusive bit count.
  llvm::Value *CreateSubgroupBallotInclusiveBitCount(llvm::Value *const value,
                                                     const llvm::Twine &instName) override final;

  // Create a subgroup ballot exclusive bit count.
  llvm::Value *CreateSubgroupBallotExclusiveBitCount(llvm::Value *const value,
                                                     const llvm::Twine &instName) override final;

  // Create a subgroup ballot find least significant bit.
  llvm::Value *CreateSubgroupBallotFindLsb(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup ballot find most significant bit.
  llvm::Value *CreateSubgroupBallotFindMsb(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup shuffle.
  llvm::Value *CreateSubgroupShuffle(llvm::Value *const value, llvm::Value *const index,
                                     const llvm::Twine &instName) override final;

  // Create a subgroup shuffle xor.
  llvm::Value *CreateSubgroupShuffleXor(llvm::Value *const value, llvm::Value *const mask,
                                        const llvm::Twine &instName) override final;

  // Create a subgroup shuffle up.
  llvm::Value *CreateSubgroupShuffleUp(llvm::Value *const value, llvm::Value *const delta,
                                       const llvm::Twine &instName) override final;

  // Create a subgroup shuffle down.
  llvm::Value *CreateSubgroupShuffleDown(llvm::Value *const value, llvm::Value *const delta,
                                         const llvm::Twine &instName) override final;

  // Create a subgroup clustered reduction.
  llvm::Value *CreateSubgroupClusteredReduction(GroupArithOp groupArithOp, llvm::Value *const value,
                                                llvm::Value *const clusterSize,
                                                const llvm::Twine &instName) override final;

  // Create a subgroup clustered inclusive scan.
  llvm::Value *CreateSubgroupClusteredInclusive(GroupArithOp groupArithOp, llvm::Value *const value,
                                                llvm::Value *const clusterSize,
                                                const llvm::Twine &instName) override final;

  // Create a subgroup clustered exclusive scan.
  llvm::Value *CreateSubgroupClusteredExclusive(GroupArithOp groupArithOp, llvm::Value *const value,
                                                llvm::Value *const clusterSize,
                                                const llvm::Twine &instName) override final;

  // Create a subgroup quad broadcast.
  llvm::Value *CreateSubgroupQuadBroadcast(llvm::Value *const value, llvm::Value *const index,
                                           const llvm::Twine &instName) override final;

  // Create a subgroup quad swap horizontal.
  llvm::Value *CreateSubgroupQuadSwapHorizontal(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup quad swap vertical.
  llvm::Value *CreateSubgroupQuadSwapVertical(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup quad swap diagonal.
  llvm::Value *CreateSubgroupQuadSwapDiagonal(llvm::Value *const value, const llvm::Twine &instName) override final;

  // Create a subgroup swizzle quad.
  llvm::Value *CreateSubgroupSwizzleQuad(llvm::Value *const value, llvm::Value *const offset,
                                         const llvm::Twine &instName) override final;

  // Create a subgroup swizzle masked.
  llvm::Value *CreateSubgroupSwizzleMask(llvm::Value *const value, llvm::Value *const mask,
                                         const llvm::Twine &instName) override final;

  // Create a subgroup write invocation.
  llvm::Value *CreateSubgroupWriteInvocation(llvm::Value *const inputValue, llvm::Value *const writeValue,
                                             llvm::Value *const index, const llvm::Twine &instName) override final;

  // Create a subgroup mbcnt.
  llvm::Value *CreateSubgroupMbcnt(llvm::Value *const mask, const llvm::Twine &instName) override final;

private:
  BuilderImplSubgroup() = delete;
  BuilderImplSubgroup(const BuilderImplSubgroup &) = delete;
  BuilderImplSubgroup &operator=(const BuilderImplSubgroup &) = delete;

  enum class DppCtrl : unsigned {
    DppQuadPerm0000 = 0x000,
    DppQuadPerm1111 = 0x055,
    DppQuadPerm2222 = 0x0AA,
    DppQuadPerm3333 = 0x0FF,
    DppQuadPerm1032 = 0x0B1,
    DppQuadPerm2301 = 0x04E,
    DppQuadPerm0123 = 0x01B,
    DppRowSr1 = 0x111,
    DppRowSr2 = 0x112,
    DppRowSr3 = 0x113,
    DppRowSr4 = 0x114,
    DppRowSr8 = 0x118,
    DppWfSl1 = 0x130,
    DppWfSr1 = 0x138,
    DppRowMirror = 0x140,
    DppRowHalfMirror = 0x141,
    DppRowBcast15 = 0x142,
    DppRowBcast31 = 0x143,
  };

  unsigned getShaderSubgroupSize();
  llvm::Value *createGroupArithmeticIdentity(GroupArithOp groupArithOp, llvm::Type *const type);
  llvm::Value *createGroupArithmeticOperation(GroupArithOp groupArithOp, llvm::Value *const x, llvm::Value *const y);
  llvm::Value *createInlineAsmSideEffect(llvm::Value *const value);
  llvm::Value *createDppMov(llvm::Value *const value, DppCtrl dppCtrl, unsigned rowMask, unsigned bankMask,
                            bool boundCtrl);
  llvm::Value *createDppUpdate(llvm::Value *const origValue, llvm::Value *const updateValue, DppCtrl dppCtrl,
                               unsigned rowMask, unsigned bankMask, bool boundCtrl);

  llvm::Value *createPermLane16(llvm::Value *const origValue, llvm::Value *const updateValue, unsigned selectBitsLow,
                                unsigned selectBitsHigh, bool fetchInactive, bool boundCtrl);
  llvm::Value *createPermLaneX16(llvm::Value *const origValue, llvm::Value *const updateValue, unsigned selectBitsLow,
                                 unsigned selectBitsHigh, bool fetchInactive, bool boundCtrl);

  llvm::Value *createDsSwizzle(llvm::Value *const value, uint16_t dsPattern);
  llvm::Value *createWwm(llvm::Value *const value);
  llvm::Value *createSetInactive(llvm::Value *const active, llvm::Value *const inactive);
  llvm::Value *createThreadMask();
  llvm::Value *createThreadMaskedSelect(llvm::Value *const threadMask, uint64_t andMask, llvm::Value *const value1,
                                        llvm::Value *const value2);
  uint16_t getDsSwizzleBitMode(uint8_t xorMask, uint8_t orMask, uint8_t andMask);
  uint16_t getDsSwizzleQuadMode(uint8_t lane0, uint8_t lane1, uint8_t lane2, uint8_t lane3);
  llvm::Value *createGroupBallot(llvm::Value *const value);
};

// =====================================================================================================================
// The Builder implementation, encompassing all the individual builder implementation subclasses
class BuilderImpl final : public BuilderImplArith,
                          BuilderImplDesc,
                          BuilderImplImage,
                          BuilderImplInOut,
                          BuilderImplMatrix,
                          BuilderImplMisc,
                          BuilderImplSubgroup {
  friend BuilderContext;

public:
  ~BuilderImpl() {}

private:
  BuilderImpl() = delete;
  BuilderImpl(const BuilderImpl &) = delete;
  BuilderImpl &operator=(const BuilderImpl &) = delete;

  BuilderImpl(BuilderContext *builderContext, Pipeline *pipeline);
};

// Built-ins for fragment input interpolation (I/J)
static const BuiltInKind BuiltInInterpPerspSample = static_cast<BuiltInKind>(0x10000000);
static const BuiltInKind BuiltInInterpPerspCenter = static_cast<BuiltInKind>(0x10000001);
static const BuiltInKind BuiltInInterpPerspCentroid = static_cast<BuiltInKind>(0x10000002);
static const BuiltInKind BuiltInInterpPullMode = static_cast<BuiltInKind>(0x10000003);
static const BuiltInKind BuiltInInterpLinearSample = static_cast<BuiltInKind>(0x10000004);
static const BuiltInKind BuiltInInterpLinearCenter = static_cast<BuiltInKind>(0x10000005);
static const BuiltInKind BuiltInInterpLinearCentroid = static_cast<BuiltInKind>(0x10000006);

// Built-ins for sample position emulation
static const BuiltInKind BuiltInSamplePosOffset = static_cast<BuiltInKind>(0x10000007);
static const BuiltInKind BuiltInNumSamples = static_cast<BuiltInKind>(0x10000008);
static const BuiltInKind BuiltInSamplePatternIdx = static_cast<BuiltInKind>(0x10000009);
static const BuiltInKind BuiltInWaveId = static_cast<BuiltInKind>(0x1000000A);

} // namespace lgc