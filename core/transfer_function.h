// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "common.h"

namespace oidn {

  // Color transfer function
  class TransferFunction
  {
  public:
    virtual ~TransferFunction() = default;

    virtual float forward(float x) const = 0;
    virtual float inverse(float x) const = 0;
  };

  class LinearTransferFunction : public TransferFunction
  {
  public:
    __forceinline float forward(float x) const override { return x; }
    __forceinline float inverse(float x) const override { return x; }
  };

  // sRGB transfer function
  class SRGBTransferFunction : public TransferFunction
  {
  public:
    __forceinline float forward(float x) const override
    {
      return pow(x, 1.f/2.2f);
    }

    __forceinline float inverse(float x) const override
    {
      return pow(x, 2.2f);
    }
  };

  // HDR transfer function: log + sRGB curve
  // Compresses [0..65536] to [0..1]
  class HDRTransferFunction : public TransferFunction
  {
  private:
    float exposure;
    float rcpExposure;

  public:
    HDRTransferFunction(float exposure = 1.f)
    {
      setExposure(exposure);
    }

    void setExposure(float exposure)
    {
      this->exposure = exposure;
      this->rcpExposure = 1.f / exposure;
    }

    __forceinline float forward(float x) const override
    {
      x *= exposure;
      return pow(log2(x+1.f) * (1.f/16.f), 1.f/2.2f);
    }

    __forceinline float inverse(float x) const override
    {
      const float y = pow(x, 2.2f);
      return (exp2(y * 16.f) - 1.f) * rcpExposure;
    }
  };

} // namespace oidn
