#include <sstream>
#include <common/command_buffers/gpu/gpu_info.hpp>

using namespace std;

namespace commandbuffers::gpu_info
{
  enum class Architecture
  {
    Unknown,
    AMD_GCN1,
    AMD_GCN2,
    AMD_GCN3,
    AMD_GCN4,
    AMD_GCN5,
    AMD_RDNA1,
    AMD_RDNA2,
    AMD_RDNA3,
    AMD_RDNA4,
    AMD_CDNA1,
    ARM_Midgard,
    ARM_Bifrost,
    ARM_Valhall,
    ARM_Gen5,
    Broadcom_VideoCore,
    Google_Swiftshader,
    ImgTec_Rogue,
    ImgTec_Furian,
    ImgTec_Albiorix,
    Intel_Gen7,
    Intel_Gen8,
    Intel_Gen9,
    Intel_Gen11,
    Intel_Gen12LP,
    Intel_Gen12HP,
    Intel_XeLPG,
    Intel_Xe2LPG,
    Intel_Xe2HPG,
    Intel_Xe3LPG,
    Mesa_Software,
    Microsoft_WARP,
    Nvidia_Fermi,
    Nvidia_Kepler,
    Nvidia_Maxwell,
    Nvidia_Pascal,
    Nvidia_Turing,
    Nvidia_Ampere,
    Nvidia_Lovelace,
    Nvidia_Blackwell,
    Nvidia_Volta,
    QualcommPCI_Adreno4xx,
    QualcommPCI_Adreno5xx,
    QualcommPCI_Adreno6xx,
    QualcommPCI_Adreno7xx,
    QualcommPCI_Adreno8xx,
    QualcommACPI_Adreno6xx,
    QualcommACPI_Adreno7xx,
    Samsung_RDNA2,
    Samsung_RDNA3,
  };

  Architecture GetArchitecture(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    switch (vendorId)
    {
    case kVendorID_AMD:
    {
      switch (deviceId & 0xFFF0)
      {
      case 0x1300:
      case 0x1310:
      case 0x6600:
      case 0x6610:
      case 0x6660:
      case 0x6790:
      case 0x6800:
      case 0x6810:
      case 0x6820:
      case 0x6830:
        return Architecture::AMD_GCN1;
      case 0x6640:
      case 0x6650:
      case 0x67A0:
      case 0x67B0:
      case 0x9830:
      case 0x9850:
        return Architecture::AMD_GCN2;
      case 0x6900:
      case 0x6920:
      case 0x6930:
      case 0x7300:
      case 0x9870:
      case 0x98E0:
        return Architecture::AMD_GCN3;
      case 0x67C0:
      case 0x67D0:
      case 0x67E0:
      case 0x67F0:
      case 0x6980:
      case 0x6990:
      case 0x6FD0:
      case 0x9920:
        return Architecture::AMD_GCN4;
      case 0x66A0:
      case 0x6860:
      case 0x6870:
      case 0x6940:
      case 0x69A0:
      case 0x15D0:
      case 0x1630:
        return Architecture::AMD_GCN5;
      case 0x7310:
      case 0x7340:
      case 0x7360:
        return Architecture::AMD_RDNA1;
      case 0x73A0:
      case 0x73B0:
      case 0x73D0:
      case 0x73E0:
      case 0x73F0:
      case 0x7400:
      case 0x7420:
      case 0x7430:
      case 0x1430:
      case 0x1500:
      case 0x15E0:
      case 0x1640:
      case 0x1680:
      case 0x13c0:
      case 0x13f0:
        return Architecture::AMD_RDNA2;
      case 0x7440:
      case 0x7470:
      case 0x7480:
      case 0x15B0:
      case 0x7450:
      case 0x1900:
      case 0x1580:
      case 0x1110:
        return Architecture::AMD_RDNA3;
      case 0x7550:
      case 0x7590:
        return Architecture::AMD_RDNA4;
      case 0x7380:
        return Architecture::AMD_CDNA1;
      }
    }
    break;
    case kVendorID_ARM:
    {
      switch (deviceId & 0xF0000000)
      {
      case 0x00000000:
        return Architecture::ARM_Midgard;
      case 0x60000000:
      case 0x70000000:
        return Architecture::ARM_Bifrost;
      case 0x90000000:
      case 0xA0000000:
      case 0xB0000000:
        return Architecture::ARM_Valhall;
      case 0xC0000000:
      case 0xD0000000:
        return Architecture::ARM_Gen5;
      }
    }
    break;
    case kVendorID_Broadcom:
    {
      switch (deviceId & 0x00000000)
      {
      case 0x00000000:
        return Architecture::Broadcom_VideoCore;
      }
    }
    break;
    case kVendorID_Google:
    {
      switch (deviceId)
      {
      case 0xC0DE:
        return Architecture::Google_Swiftshader;
      }
    }
    break;
    case kVendorID_ImgTec:
    {
      switch (deviceId & 0xFF000000)
      {
      case 0x00000000:
      case 0x22000000:
      case 0x24000000:
        return Architecture::ImgTec_Rogue;
      case 0x1b000000:
        return Architecture::ImgTec_Furian;
      case 0x35000000:
      case 0x36000000:
        return Architecture::ImgTec_Albiorix;
      }
    }
    break;
    case kVendorID_Intel:
    {
      switch (deviceId & 0xFF00)
      {
      case 0x0100:
      case 0x0400:
      case 0x0A00:
      case 0x0D00:
      case 0x0F00:
        return Architecture::Intel_Gen7;
      case 0x1600:
      case 0x2200:
        return Architecture::Intel_Gen8;
      case 0x1900:
      case 0x3100:
      case 0x3E00:
      case 0x5A00:
      case 0x5900:
      case 0x8700:
      case 0x9B00:
        return Architecture::Intel_Gen9;
      case 0x8A00:
      case 0x4E00:
      case 0x9800:
        return Architecture::Intel_Gen11;
      case 0x4600:
      case 0x4C00:
      case 0x4900:
      case 0x9A00:
      case 0xA700:
        return Architecture::Intel_Gen12LP;
      case 0x4F00:
      case 0x5600:
        return Architecture::Intel_Gen12HP;
      case 0x7D00:
      case 0xB600:
        return Architecture::Intel_XeLPG;
      case 0x6400:
        return Architecture::Intel_Xe2LPG;
      case 0xE200:
        return Architecture::Intel_Xe2HPG;
      case 0xB000:
        return Architecture::Intel_Xe3LPG;
      }
    }
    break;
    case kVendorID_Mesa:
    {
      switch (deviceId)
      {
      case 0x0000:
        return Architecture::Mesa_Software;
      }
    }
    break;
    case kVendorID_Microsoft:
    {
      switch (deviceId)
      {
      case 0x8c:
        return Architecture::Microsoft_WARP;
      }
    }
    break;
    case kVendorID_Nvidia:
    {
      switch (deviceId & 0xFFFFFF00)
      {
      case 0x0D00:
        return Architecture::Nvidia_Fermi;
      case 0x0F00:
      case 0x1000:
      case 0x1100:
      case 0x1200:
        return Architecture::Nvidia_Kepler;
      case 0x1300:
      case 0x1400:
      case 0x1600:
      case 0x1700:
        return Architecture::Nvidia_Maxwell;
      case 0x1500:
      case 0x1B00:
      case 0x1C00:
      case 0x1D00:
        return Architecture::Nvidia_Pascal;
      case 0x1E00:
      case 0x1F00:
      case 0x2100:
        return Architecture::Nvidia_Turing;
      case 0x2200:
      case 0x2400:
      case 0x2500:
      case 0x2000:
        return Architecture::Nvidia_Ampere;
      case 0x2600:
      case 0x2700:
      case 0x2800:
        return Architecture::Nvidia_Lovelace;
      case 0x2b00:
      case 0x2c00:
      case 0x2d00:
      case 0x2f00:
        return Architecture::Nvidia_Blackwell;
      }
      switch (deviceId & 0xFF000000)
      {
      case 0x1e000000:
        return Architecture::Nvidia_Kepler;
      case 0x92000000:
        return Architecture::Nvidia_Maxwell;
      case 0x93000000:
        return Architecture::Nvidia_Pascal;
      case 0x97000000:
        return Architecture::Nvidia_Ampere;
      case 0xa5000000:
        return Architecture::Nvidia_Volta;
      }
    }
    break;
    case kVendorID_QualcommPCI:
    {
      switch (deviceId & 0xFF000000)
      {
      case 0x04000000:
        return Architecture::QualcommPCI_Adreno4xx;
      case 0x05000000:
        return Architecture::QualcommPCI_Adreno5xx;
      case 0x06000000:
        return Architecture::QualcommPCI_Adreno6xx;
      case 0x07000000:
      case 0x43000000:
      case 0x36000000:
      case 0x37000000:
        return Architecture::QualcommPCI_Adreno7xx;
      case 0x44000000:
        return Architecture::QualcommPCI_Adreno8xx;
      }
    }
    break;
    case kVendorID_QualcommACPI:
    {
      switch (deviceId & 0xFFFFFF00)
      {
      case 0x41333800:
      case 0x36334100:
      case 0x41333400:
      case 0x36333600:
        return Architecture::QualcommACPI_Adreno6xx;
      case 0x37314400:
      case 0x36334300:
        return Architecture::QualcommACPI_Adreno7xx;
      }
    }
    break;
    case kVendorID_Samsung:
    {
      switch (deviceId & 0xFFFFFFFF)
      {
      case 0x000073A0:
      case 0x01300100:
        return Architecture::Samsung_RDNA2;
      case 0x02600200:
        return Architecture::Samsung_RDNA3;
      }
    }
    break;
    case kVendorID_Huawei:
    {
      switch (deviceId & 0xFFFFFFFF)
      {
      }
    }
    break;
    }

    return Architecture::Unknown;
  }

  bool IsAMD(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_AMD;
  }
  bool IsApple(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Apple;
  }
  bool IsARM(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_ARM;
  }
  bool IsBroadcom(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Broadcom;
  }
  bool IsGoogle(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Google;
  }
  bool IsImgTec(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_ImgTec;
  }
  bool IsIntel(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Intel;
  }
  bool IsMesa(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Mesa;
  }
  bool IsMicrosoft(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Microsoft;
  }
  bool IsNvidia(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Nvidia;
  }
  bool IsQualcommPCI(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_QualcommPCI;
  }
  bool IsQualcommACPI(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_QualcommACPI;
  }
  bool IsSamsung(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Samsung;
  }
  bool IsHuawei(PCIVendorID vendorId)
  {
    return vendorId == kVendorID_Huawei;
  }

  // Architecture checks

  // AMD architectures
  bool IsAMDGCN1(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_GCN1;
  }
  bool IsAMDGCN2(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_GCN2;
  }
  bool IsAMDGCN3(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_GCN3;
  }
  bool IsAMDGCN4(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_GCN4;
  }
  bool IsAMDGCN5(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_GCN5;
  }
  bool IsAMDRDNA1(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_RDNA1;
  }
  bool IsAMDRDNA2(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_RDNA2;
  }
  bool IsAMDRDNA3(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_RDNA3;
  }
  bool IsAMDRDNA4(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_RDNA4;
  }
  bool IsAMDCDNA1(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::AMD_CDNA1;
  }
  // ARM architectures
  bool IsARMMidgard(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::ARM_Midgard;
  }
  bool IsARMBifrost(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::ARM_Bifrost;
  }
  bool IsARMValhall(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::ARM_Valhall;
  }
  bool IsARMGen5(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::ARM_Gen5;
  }
  // Broadcom architectures
  bool IsBroadcomVideoCore(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Broadcom_VideoCore;
  }
  // Google architectures
  bool IsGoogleSwiftshader(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Google_Swiftshader;
  }
  // Img Tec architectures
  bool IsImgTecRogue(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::ImgTec_Rogue;
  }
  bool IsImgTecFurian(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::ImgTec_Furian;
  }
  bool IsImgTecAlbiorix(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::ImgTec_Albiorix;
  }
  // Intel architectures
  bool IsIntelGen7(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Gen7;
  }
  bool IsIntelGen8(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Gen8;
  }
  bool IsIntelGen9(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Gen9;
  }
  bool IsIntelGen11(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Gen11;
  }
  bool IsIntelGen12LP(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Gen12LP;
  }
  bool IsIntelGen12HP(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Gen12HP;
  }
  bool IsIntelXeLPG(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_XeLPG;
  }
  bool IsIntelXe2LPG(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Xe2LPG;
  }
  bool IsIntelXe2HPG(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Xe2HPG;
  }
  bool IsIntelXe3LPG(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Intel_Xe3LPG;
  }
  // Mesa architectures
  bool IsMesaSoftware(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Mesa_Software;
  }
  // Microsoft architectures
  bool IsMicrosoftWARP(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Microsoft_WARP;
  }
  // Nvidia architectures
  bool IsNvidiaFermi(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Fermi;
  }
  bool IsNvidiaKepler(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Kepler;
  }
  bool IsNvidiaMaxwell(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Maxwell;
  }
  bool IsNvidiaPascal(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Pascal;
  }
  bool IsNvidiaTuring(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Turing;
  }
  bool IsNvidiaAmpere(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Ampere;
  }
  bool IsNvidiaLovelace(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Lovelace;
  }
  bool IsNvidiaBlackwell(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Blackwell;
  }
  bool IsNvidiaVolta(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Nvidia_Volta;
  }
  // QualcommPCI architectures
  bool IsQualcommPCIAdreno4xx(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::QualcommPCI_Adreno4xx;
  }
  bool IsQualcommPCIAdreno5xx(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::QualcommPCI_Adreno5xx;
  }
  bool IsQualcommPCIAdreno6xx(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::QualcommPCI_Adreno6xx;
  }
  bool IsQualcommPCIAdreno7xx(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::QualcommPCI_Adreno7xx;
  }
  bool IsQualcommPCIAdreno8xx(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::QualcommPCI_Adreno8xx;
  }
  // QualcommACPI architectures
  bool IsQualcommACPIAdreno6xx(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::QualcommACPI_Adreno6xx;
  }
  bool IsQualcommACPIAdreno7xx(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::QualcommACPI_Adreno7xx;
  }
  // Samsung architectures
  bool IsSamsungRDNA2(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Samsung_RDNA2;
  }
  bool IsSamsungRDNA3(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    return GetArchitecture(vendorId, deviceId) == Architecture::Samsung_RDNA3;
  }

  // GPUAdapterInfo fields
  string GetVendorName(PCIVendorID vendorId)
  {
    switch (vendorId)
    {
    case kVendorID_AMD:
      return "amd";
    case kVendorID_Apple:
      return "apple";
    case kVendorID_ARM:
      return "arm";
    case kVendorID_Broadcom:
      return "broadcom";
    case kVendorID_Google:
      return "google";
    case kVendorID_ImgTec:
      return "img-tec";
    case kVendorID_Intel:
      return "intel";
    case kVendorID_Mesa:
      return "mesa";
    case kVendorID_Microsoft:
      return "microsoft";
    case kVendorID_Nvidia:
      return "nvidia";
    case kVendorID_QualcommPCI:
      return "qualcomm";
    case kVendorID_QualcommACPI:
      return "qualcomm";
    case kVendorID_Samsung:
      return "samsung";
    case kVendorID_Huawei:
      return "huawei";
    }

    return "";
  }

  string GetArchitectureName(PCIVendorID vendorId, PCIDeviceID deviceId)
  {
    Architecture arch = GetArchitecture(vendorId, deviceId);
    switch (arch)
    {
    case Architecture::Unknown:
      return "";
    case Architecture::AMD_GCN1:
      return "gcn-1";
    case Architecture::AMD_GCN2:
      return "gcn-2";
    case Architecture::AMD_GCN3:
      return "gcn-3";
    case Architecture::AMD_GCN4:
      return "gcn-4";
    case Architecture::AMD_GCN5:
      return "gcn-5";
    case Architecture::AMD_RDNA1:
      return "rdna-1";
    case Architecture::AMD_RDNA2:
      return "rdna-2";
    case Architecture::AMD_RDNA3:
      return "rdna-3";
    case Architecture::AMD_RDNA4:
      return "rdna-4";
    case Architecture::AMD_CDNA1:
      return "cdna-1";
    case Architecture::ARM_Midgard:
      return "midgard";
    case Architecture::ARM_Bifrost:
      return "bifrost";
    case Architecture::ARM_Valhall:
      return "valhall";
    case Architecture::ARM_Gen5:
      return "gen-5";
    case Architecture::Broadcom_VideoCore:
      return "videocore";
    case Architecture::Google_Swiftshader:
      return "swiftshader";
    case Architecture::ImgTec_Rogue:
      return "rogue";
    case Architecture::ImgTec_Furian:
      return "furian";
    case Architecture::ImgTec_Albiorix:
      return "albiorix";
    case Architecture::Intel_Gen7:
      return "gen-7";
    case Architecture::Intel_Gen8:
      return "gen-8";
    case Architecture::Intel_Gen9:
      return "gen-9";
    case Architecture::Intel_Gen11:
      return "gen-11";
    case Architecture::Intel_Gen12LP:
      return "gen-12lp";
    case Architecture::Intel_Gen12HP:
      return "gen-12hp";
    case Architecture::Intel_XeLPG:
      return "xe-lpg";
    case Architecture::Intel_Xe2LPG:
      return "xe-2lpg";
    case Architecture::Intel_Xe2HPG:
      return "xe-2hpg";
    case Architecture::Intel_Xe3LPG:
      return "xe-3lpg";
    case Architecture::Mesa_Software:
      return "software";
    case Architecture::Microsoft_WARP:
      return "warp";
    case Architecture::Nvidia_Fermi:
      return "fermi";
    case Architecture::Nvidia_Kepler:
      return "kepler";
    case Architecture::Nvidia_Maxwell:
      return "maxwell";
    case Architecture::Nvidia_Pascal:
      return "pascal";
    case Architecture::Nvidia_Turing:
      return "turing";
    case Architecture::Nvidia_Ampere:
      return "ampere";
    case Architecture::Nvidia_Lovelace:
      return "lovelace";
    case Architecture::Nvidia_Blackwell:
      return "blackwell";
    case Architecture::Nvidia_Volta:
      return "volta";
    case Architecture::QualcommPCI_Adreno4xx:
      return "adreno-4xx";
    case Architecture::QualcommPCI_Adreno5xx:
      return "adreno-5xx";
    case Architecture::QualcommPCI_Adreno6xx:
      return "adreno-6xx";
    case Architecture::QualcommPCI_Adreno7xx:
      return "adreno-7xx";
    case Architecture::QualcommPCI_Adreno8xx:
      return "adreno-8xx";
    case Architecture::QualcommACPI_Adreno6xx:
      return "adreno-6xx";
    case Architecture::QualcommACPI_Adreno7xx:
      return "adreno-7xx";
    case Architecture::Samsung_RDNA2:
      return "rdna-2";
    case Architecture::Samsung_RDNA3:
      return "rdna-3";
    }

    return "";
  }

  DriverVersion::DriverVersion() = default;

  DriverVersion::DriverVersion(const std::initializer_list<uint16_t> &version)
  {
    assert(version.size() <= kMaxVersionFields);
    driver_version_.assign(version.begin(), version.end());
  }

  uint16_t &DriverVersion::operator[](size_t i)
  {
    return driver_version_.operator[](i);
  }

  const uint16_t &DriverVersion::operator[](size_t i) const
  {
    return driver_version_.operator[](i);
  }

  uint32_t DriverVersion::size() const
  {
    return driver_version_.size();
  }

  std::string DriverVersion::toString() const
  {
    std::ostringstream oss;
    if (!driver_version_.empty())
    {
      // Convert all but the last element to avoid a trailing "."
      std::copy(driver_version_.begin(), driver_version_.end() - 1, std::ostream_iterator<uint16_t>(oss, "."));
      // Add the last element
      oss << driver_version_.back();
    }
    return oss.str();
  }


  IntelGen GetIntelGen(PCIVendorID venderId, PCIDeviceID deviceId)
  {
    assert(gpu_info::IsIntel(venderId));

    if (gpu_info::IsIntelGen7(venderId, deviceId))
    {
      return IntelGen::Gen7;
    }
    else if (gpu_info::IsIntelGen8(venderId, deviceId))
    {
      return IntelGen::Gen8;
    }
    else if (gpu_info::IsIntelGen9(venderId, deviceId))
    {
      return IntelGen::Gen9;
    }
    else if (gpu_info::IsIntelGen11(venderId, deviceId))
    {
      return IntelGen::Gen11;
    }
    else if (gpu_info::IsIntelGen12LP(venderId, deviceId))
    {
      return IntelGen::Xe;
    }
    else if (gpu_info::IsIntelGen12HP(venderId, deviceId))
    {
      return IntelGen::Xe;
    }
    else if (gpu_info::IsIntelXeLPG(venderId, deviceId))
    {
      return IntelGen::Xe;
    }
    else if (gpu_info::IsIntelXe2LPG(venderId, deviceId))
    {
      return IntelGen::Xe2;
    }
    else if (gpu_info::IsIntelXe2HPG(venderId, deviceId))
    {
      return IntelGen::Xe2;
    }
    else if (gpu_info::IsIntelXe3LPG(venderId, deviceId))
    {
      return IntelGen::Xe3;
    }
    else
    {
      return IntelGen::Unknown;
    }
  }

  QualcommACPIGen GetQualcommACPIGen(PCIVendorID venderId, PCIDeviceID deviceId)
  {
    assert(gpu_info::IsQualcommACPI(venderId));

    if (gpu_info::IsQualcommACPIAdreno6xx(venderId, deviceId))
    {
      return QualcommACPIGen::Adreno6xx;
    }
    else if (gpu_info::IsQualcommACPIAdreno7xx(venderId, deviceId))
    {
      return QualcommACPIGen::Adreno7xx;
    }
    else
    {
      return QualcommACPIGen::Unknown;
    }
  }
}
