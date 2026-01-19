// Copyright 2019 The Dawn & Tint Authors
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

#include <cassert>
#include <cstdint>
#include <string>

namespace commandbuffers::gpu_info
{
  using PCIVendorID = uint32_t;
  using PCIDeviceID = uint32_t;

  // Vendor IDs
  static constexpr PCIVendorID kVendorID_AMD = 0x1002;
  static constexpr PCIVendorID kVendorID_Apple = 0x106b;
  static constexpr PCIVendorID kVendorID_ARM = 0x13B5;
  static constexpr PCIVendorID kVendorID_Broadcom = 0x14e4;
  static constexpr PCIVendorID kVendorID_Google = 0x1AE0;
  static constexpr PCIVendorID kVendorID_ImgTec = 0x1010;
  static constexpr PCIVendorID kVendorID_Intel = 0x8086;
  static constexpr PCIVendorID kVendorID_Mesa = 0x10005;
  static constexpr PCIVendorID kVendorID_Microsoft = 0x1414;
  static constexpr PCIVendorID kVendorID_Nvidia = 0x10DE;
  static constexpr PCIVendorID kVendorID_QualcommPCI = 0x5143;
  static constexpr PCIVendorID kVendorID_QualcommACPI = 0x4D4F4351;
  static constexpr PCIVendorID kVendorID_Samsung = 0x144d;
  static constexpr PCIVendorID kVendorID_Huawei = 0x19e5;
  static constexpr uint32_t kMaxVersionFields = 4;

  // Vendor checks
  bool IsAMD(PCIVendorID vendorId);
  bool IsApple(PCIVendorID vendorId);
  bool IsARM(PCIVendorID vendorId);
  bool IsBroadcom(PCIVendorID vendorId);
  bool IsGoogle(PCIVendorID vendorId);
  bool IsImgTec(PCIVendorID vendorId);
  bool IsIntel(PCIVendorID vendorId);
  bool IsMesa(PCIVendorID vendorId);
  bool IsMicrosoft(PCIVendorID vendorId);
  bool IsNvidia(PCIVendorID vendorId);
  bool IsQualcommPCI(PCIVendorID vendorId);
  bool IsQualcommACPI(PCIVendorID vendorId);
  bool IsSamsung(PCIVendorID vendorId);
  bool IsHuawei(PCIVendorID vendorId);

  // Architecture checks

  // AMD architectures
  bool IsAMDGCN1(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDGCN2(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDGCN3(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDGCN4(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDGCN5(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDRDNA1(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDRDNA2(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDRDNA3(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDRDNA4(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsAMDCDNA1(PCIVendorID vendorId, PCIDeviceID deviceId);

  // ARM architectures
  bool IsARMMidgard(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsARMBifrost(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsARMValhall(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsARMGen5(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Broadcom architectures
  bool IsBroadcomVideoCore(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Google architectures
  bool IsGoogleSwiftshader(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Img Tec architectures
  bool IsImgTecRogue(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsImgTecFurian(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsImgTecAlbiorix(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Intel architectures
  bool IsIntelGen7(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelGen8(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelGen9(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelGen11(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelGen12LP(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelGen12HP(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelXeLPG(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelXe2LPG(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelXe2HPG(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsIntelXe3LPG(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Mesa architectures
  bool IsMesaSoftware(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Microsoft architectures
  bool IsMicrosoftWARP(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Nvidia architectures
  bool IsNvidiaFermi(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaKepler(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaMaxwell(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaPascal(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaTuring(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaAmpere(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaLovelace(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaBlackwell(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsNvidiaVolta(PCIVendorID vendorId, PCIDeviceID deviceId);

  // QualcommPCI architectures
  bool IsQualcommPCIAdreno4xx(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsQualcommPCIAdreno5xx(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsQualcommPCIAdreno6xx(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsQualcommPCIAdreno7xx(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsQualcommPCIAdreno8xx(PCIVendorID vendorId, PCIDeviceID deviceId);

  // QualcommACPI architectures
  bool IsQualcommACPIAdreno6xx(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsQualcommACPIAdreno7xx(PCIVendorID vendorId, PCIDeviceID deviceId);

  // Samsung architectures
  bool IsSamsungRDNA2(PCIVendorID vendorId, PCIDeviceID deviceId);
  bool IsSamsungRDNA3(PCIVendorID vendorId, PCIDeviceID deviceId);

  // GPUAdapterInfo fields
  std::string GetVendorName(PCIVendorID vendorId);
  std::string GetArchitectureName(PCIVendorID vendorId, PCIDeviceID deviceId);

  class DriverVersion
  {
  public:
    DriverVersion();
    DriverVersion(const std::initializer_list<uint16_t> &version);

    uint16_t &operator[](size_t i);
    const uint16_t &operator[](size_t i) const;

    uint32_t size() const;
    std::string toString() const;

  private:
    std::vector<uint16_t> driver_version_;
  };

  // GPU generation is an internal concept, rather than a value defined by vendors
  // Intel generations
  enum class IntelGen
  {
    Unknown = 0,
    Gen7 = 7,
    Gen8 = 8,
    Gen9 = 9,
    Gen10 = 10,
    Gen11 = 11,
    Xe = 12,
    Xe2 = 13,
    Xe3 = 14,
  };

  // Qualcomm ACPI generations
  enum class QualcommACPIGen
  {
    Unknown = 0,
    Adreno6xx = 6,
    Adreno7xx = 7,
  };

  IntelGen GetIntelGen(PCIVendorID venderId, PCIDeviceID deviceId);
  QualcommACPIGen GetQualcommACPIGen(PCIVendorID venderId, PCIDeviceID deviceId);
}
