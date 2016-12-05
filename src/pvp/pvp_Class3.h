// pvp_Class3.h - generated by pvcam2param.pl. Do not Edit.

  bool availExpTime() throw(pvpException);
  pvpAccess accessExpTime() throw(pvpException);
  uint16_t getExpTime() throw(pvpException);
  uint16_t minExpTime() throw(pvpException);
  uint16_t maxExpTime() throw(pvpException);
  uint16_t defaultExpTime() throw(pvpException);
  uint16_t stepExpTime() throw(pvpException);
  int countExpTime() throw(pvpException);
  void setExpTime(uint16_t) throw(pvpException);
  void reportExpTime() throw(pvpException);

  class ExpRes {
  public:
    enum V {
      Millisec,
      Microsec,
      Sec,
    };
    ExpRes(ExpRes::V x): x(x) {}
    ExpRes(int x0=0) { x=(V)(x0); }
    operator pvpEnumInt() const { return x; }
    char const *decode() const;
  private:
    V x;
  };

  bool availExpRes() throw(pvpException);
  pvpAccess accessExpRes() throw(pvpException);
  pvpCamera::ExpRes getExpRes() throw(pvpException);
  pvpCamera::ExpRes minExpRes() throw(pvpException);
  pvpCamera::ExpRes maxExpRes() throw(pvpException);
  pvpCamera::ExpRes defaultExpRes() throw(pvpException);
  pvpCamera::ExpRes stepExpRes() throw(pvpException);
  int countExpRes() throw(pvpException);
  void setExpRes(pvpCamera::ExpRes) throw(pvpException);
  void reportExpRes() throw(pvpException);

  bool availExpResIndex() throw(pvpException);
  pvpAccess accessExpResIndex() throw(pvpException);
  uint16_t getExpResIndex() throw(pvpException);
  uint16_t minExpResIndex() throw(pvpException);
  uint16_t maxExpResIndex() throw(pvpException);
  uint16_t defaultExpResIndex() throw(pvpException);
  uint16_t stepExpResIndex() throw(pvpException);
  int countExpResIndex() throw(pvpException);
  void setExpResIndex(uint16_t) throw(pvpException);
  void reportExpResIndex() throw(pvpException);

  bool availExposureTime() throw(pvpException);
  pvpAccess accessExposureTime() throw(pvpException);
  uint64_t getExposureTime() throw(pvpException);
  uint64_t minExposureTime() throw(pvpException);
  uint64_t maxExposureTime() throw(pvpException);
  uint64_t defaultExposureTime() throw(pvpException);
  uint64_t stepExposureTime() throw(pvpException);
  int countExposureTime() throw(pvpException);
  void setExposureTime(uint64_t) throw(pvpException);
  void reportExposureTime() throw(pvpException);

  class BofEofEnable {
  public:
    enum V {
      NoFrameIrqs,
      BeginFrameIrqs,
      EndFrameIrqs,
      BeginEndFrameIrqs,
    };
    BofEofEnable(BofEofEnable::V x): x(x) {}
    BofEofEnable(int x0=0) { x=(V)(x0); }
    operator pvpEnumInt() const { return x; }
    char const *decode() const;
  private:
    V x;
  };

  bool availBofEofEnable() throw(pvpException);
  pvpAccess accessBofEofEnable() throw(pvpException);
  pvpCamera::BofEofEnable getBofEofEnable() throw(pvpException);
  pvpCamera::BofEofEnable minBofEofEnable() throw(pvpException);
  pvpCamera::BofEofEnable maxBofEofEnable() throw(pvpException);
  pvpCamera::BofEofEnable defaultBofEofEnable() throw(pvpException);
  pvpCamera::BofEofEnable stepBofEofEnable() throw(pvpException);
  int countBofEofEnable() throw(pvpException);
  void setBofEofEnable(pvpCamera::BofEofEnable) throw(pvpException);
  void reportBofEofEnable() throw(pvpException);

  bool availBofEofCount() throw(pvpException);
  pvpAccess accessBofEofCount() throw(pvpException);
  uint32_t getBofEofCount() throw(pvpException);
  uint32_t minBofEofCount() throw(pvpException);
  uint32_t maxBofEofCount() throw(pvpException);
  uint32_t defaultBofEofCount() throw(pvpException);
  uint32_t stepBofEofCount() throw(pvpException);
  int countBofEofCount() throw(pvpException);
  void setBofEofCount(uint32_t) throw(pvpException);
  void reportBofEofCount() throw(pvpException);

  bool availBofEofClr() throw(pvpException);
  pvpAccess accessBofEofClr() throw(pvpException);
  bool getBofEofClr() throw(pvpException);
  bool minBofEofClr() throw(pvpException);
  bool maxBofEofClr() throw(pvpException);
  bool defaultBofEofClr() throw(pvpException);
  bool stepBofEofClr() throw(pvpException);
  int countBofEofClr() throw(pvpException);
  void setBofEofClr(bool) throw(pvpException);
  void reportBofEofClr() throw(pvpException);

  bool availCircBuffer() throw(pvpException);
  pvpAccess accessCircBuffer() throw(pvpException);
  bool getCircBuffer() throw(pvpException);
  bool minCircBuffer() throw(pvpException);
  bool maxCircBuffer() throw(pvpException);
  bool defaultCircBuffer() throw(pvpException);
  bool stepCircBuffer() throw(pvpException);
  int countCircBuffer() throw(pvpException);
  void setCircBuffer(bool) throw(pvpException);
  void reportCircBuffer() throw(pvpException);

  bool availFrameBufferSize() throw(pvpException);
  pvpAccess accessFrameBufferSize() throw(pvpException);
  uint64_t getFrameBufferSize() throw(pvpException);
  uint64_t minFrameBufferSize() throw(pvpException);
  uint64_t maxFrameBufferSize() throw(pvpException);
  uint64_t defaultFrameBufferSize() throw(pvpException);
  uint64_t stepFrameBufferSize() throw(pvpException);
  int countFrameBufferSize() throw(pvpException);
  void setFrameBufferSize(uint64_t) throw(pvpException);
  void reportFrameBufferSize() throw(pvpException);

  class BinningSer {
  public:
    enum V {
      One = 1,
      Two,
      Three,
      Four,
    };
    BinningSer(BinningSer::V x): x(x) {}
    BinningSer(int x0=0) { x=(V)(x0); }
    operator pvpEnumInt() const { return x; }
    char const *decode() const;
  private:
    V x;
  };

  bool availBinningSer() throw(pvpException);
  pvpAccess accessBinningSer() throw(pvpException);
  pvpCamera::BinningSer getBinningSer() throw(pvpException);
  pvpCamera::BinningSer minBinningSer() throw(pvpException);
  pvpCamera::BinningSer maxBinningSer() throw(pvpException);
  pvpCamera::BinningSer defaultBinningSer() throw(pvpException);
  pvpCamera::BinningSer stepBinningSer() throw(pvpException);
  int countBinningSer() throw(pvpException);
  void setBinningSer(pvpCamera::BinningSer) throw(pvpException);
  void reportBinningSer() throw(pvpException);

  class BinningPar {
  public:
    enum V {
      One = 1,
      Two,
      Three,
      Four,
    };
    BinningPar(BinningPar::V x): x(x) {}
    BinningPar(int x0=0) { x=(V)(x0); }
    operator pvpEnumInt() const { return x; }
    char const *decode() const;
  private:
    V x;
  };

  bool availBinningPar() throw(pvpException);
  pvpAccess accessBinningPar() throw(pvpException);
  pvpCamera::BinningPar getBinningPar() throw(pvpException);
  pvpCamera::BinningPar minBinningPar() throw(pvpException);
  pvpCamera::BinningPar maxBinningPar() throw(pvpException);
  pvpCamera::BinningPar defaultBinningPar() throw(pvpException);
  pvpCamera::BinningPar stepBinningPar() throw(pvpException);
  int countBinningPar() throw(pvpException);
  void setBinningPar(pvpCamera::BinningPar) throw(pvpException);
  void reportBinningPar() throw(pvpException);

  bool availMetadataEnabled() throw(pvpException);
  pvpAccess accessMetadataEnabled() throw(pvpException);
  bool getMetadataEnabled() throw(pvpException);
  bool minMetadataEnabled() throw(pvpException);
  bool maxMetadataEnabled() throw(pvpException);
  bool defaultMetadataEnabled() throw(pvpException);
  bool stepMetadataEnabled() throw(pvpException);
  int countMetadataEnabled() throw(pvpException);
  void setMetadataEnabled(bool) throw(pvpException);
  void reportMetadataEnabled() throw(pvpException);

  bool availRoiCount() throw(pvpException);
  pvpAccess accessRoiCount() throw(pvpException);
  uint16_t getRoiCount() throw(pvpException);
  uint16_t minRoiCount() throw(pvpException);
  uint16_t maxRoiCount() throw(pvpException);
  uint16_t defaultRoiCount() throw(pvpException);
  uint16_t stepRoiCount() throw(pvpException);
  int countRoiCount() throw(pvpException);
  void setRoiCount(uint16_t) throw(pvpException);
  void reportRoiCount() throw(pvpException);

  bool availCentroidsEnabled() throw(pvpException);
  pvpAccess accessCentroidsEnabled() throw(pvpException);
  bool getCentroidsEnabled() throw(pvpException);
  bool minCentroidsEnabled() throw(pvpException);
  bool maxCentroidsEnabled() throw(pvpException);
  bool defaultCentroidsEnabled() throw(pvpException);
  bool stepCentroidsEnabled() throw(pvpException);
  int countCentroidsEnabled() throw(pvpException);
  void setCentroidsEnabled(bool) throw(pvpException);
  void reportCentroidsEnabled() throw(pvpException);

  bool availCentroidsRadius() throw(pvpException);
  pvpAccess accessCentroidsRadius() throw(pvpException);
  uint16_t getCentroidsRadius() throw(pvpException);
  uint16_t minCentroidsRadius() throw(pvpException);
  uint16_t maxCentroidsRadius() throw(pvpException);
  uint16_t defaultCentroidsRadius() throw(pvpException);
  uint16_t stepCentroidsRadius() throw(pvpException);
  int countCentroidsRadius() throw(pvpException);
  void setCentroidsRadius(uint16_t) throw(pvpException);
  void reportCentroidsRadius() throw(pvpException);

  bool availCentroidsCount() throw(pvpException);
  pvpAccess accessCentroidsCount() throw(pvpException);
  uint16_t getCentroidsCount() throw(pvpException);
  uint16_t minCentroidsCount() throw(pvpException);
  uint16_t maxCentroidsCount() throw(pvpException);
  uint16_t defaultCentroidsCount() throw(pvpException);
  uint16_t stepCentroidsCount() throw(pvpException);
  int countCentroidsCount() throw(pvpException);
  void setCentroidsCount(uint16_t) throw(pvpException);
  void reportCentroidsCount() throw(pvpException);

  class TrigtabSignal {
  public:
    enum V {
      Out,
    };
    TrigtabSignal(TrigtabSignal::V x): x(x) {}
    TrigtabSignal(int x0=0) { x=(V)(x0); }
    operator pvpEnumInt() const { return x; }
    char const *decode() const;
  private:
    V x;
  };

  bool availTrigtabSignal() throw(pvpException);
  pvpAccess accessTrigtabSignal() throw(pvpException);
  pvpCamera::TrigtabSignal getTrigtabSignal() throw(pvpException);
  pvpCamera::TrigtabSignal minTrigtabSignal() throw(pvpException);
  pvpCamera::TrigtabSignal maxTrigtabSignal() throw(pvpException);
  pvpCamera::TrigtabSignal defaultTrigtabSignal() throw(pvpException);
  pvpCamera::TrigtabSignal stepTrigtabSignal() throw(pvpException);
  int countTrigtabSignal() throw(pvpException);
  void setTrigtabSignal(pvpCamera::TrigtabSignal) throw(pvpException);
  void reportTrigtabSignal() throw(pvpException);

  bool availLastMuxedSignal() throw(pvpException);
  pvpAccess accessLastMuxedSignal() throw(pvpException);
  unsigned char getLastMuxedSignal() throw(pvpException);
  unsigned char minLastMuxedSignal() throw(pvpException);
  unsigned char maxLastMuxedSignal() throw(pvpException);
  unsigned char defaultLastMuxedSignal() throw(pvpException);
  unsigned char stepLastMuxedSignal() throw(pvpException);
  int countLastMuxedSignal() throw(pvpException);
  void setLastMuxedSignal(unsigned char) throw(pvpException);
  void reportLastMuxedSignal() throw(pvpException);

  void reportClass3() throw(pvpException);