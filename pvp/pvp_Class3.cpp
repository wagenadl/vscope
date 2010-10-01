// pvp_Class3.cpp - generated by pvcam2param.pl. Do not Edit.

#include <pvp/pvpCamera.h>
#include <QTextStream>
#include <pvp/dwpvcam.h>

bool pvpCamera::availExpTime() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_EXP_TIME,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get EXP_TIME availability");
  return avail;
}

pvpAccess pvpCamera::accessExpTime() throw(pvpException) {
  uns16 access;
  if (!availExpTime())
    throw pvpException("EXP_TIME not available");
  if (!pl_get_param(camh,PARAM_EXP_TIME,ATTR_ACCESS,&access))
    throw pvpException("Cannot get EXP_TIME accessibility");
  return pvpAccess(access);
}

int pvpCamera::countExpTime() throw(pvpException) {
  uns32 count;
  if (!availExpTime())
    throw pvpException("EXP_TIME not available");
  if (!pl_get_param(camh,PARAM_EXP_TIME,ATTR_COUNT,&count))
    throw pvpException("Cannot get EXP_TIME count");
  return count;
}

unsigned short pvpCamera::getExpTime() throw(pvpException) {
  pvpAccess a = accessExpTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_TIME,ATTR_CURRENT,&x))
      throw pvpException("Cannot read EXP_TIME");
    return x;
  } else {
    throw pvpException("EXP_TIME not accessible for reading");
  }
}

unsigned short pvpCamera::defaultExpTime() throw(pvpException) {
  pvpAccess a = accessExpTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_TIME,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of EXP_TIME");
    return x;
  } else {
    throw pvpException("EXP_TIME not accessible for reading");
  }
}

unsigned short pvpCamera::minExpTime() throw(pvpException) {
  pvpAccess a = accessExpTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_TIME,ATTR_MIN,&x))
      throw pvpException("Cannot read min of EXP_TIME");
    return x;
  } else {
    throw pvpException("EXP_TIME not accessible for reading");
  }
}

unsigned short pvpCamera::maxExpTime() throw(pvpException) {
  pvpAccess a = accessExpTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_TIME,ATTR_MAX,&x))
      throw pvpException("Cannot read max of EXP_TIME");
    return x;
  } else {
    throw pvpException("EXP_TIME not accessible for reading");
  }
}

void pvpCamera::setExpTime(unsigned short x) throw(pvpException) {
  pvpAccess a = accessExpTime();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    uns16 y = x;
    if (!pl_set_param(camh,PARAM_EXP_TIME,(void*)(&y)))
      throw pvpException("Cannot set EXP_TIME");
  }
}

void pvpCamera::reportExpTime() throw(pvpException) {
  if (availExpTime()) {
    pvpAccess a = accessExpTime();
    printf("ExpTime: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getExpTime() <<"\n";
        ss << "  min value: " << minExpTime() <<"\n";
        ss << "  max value: " << maxExpTime() <<"\n";
        ss << "  default value: " << defaultExpTime() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countExpTime());
  } else {
    printf("ExpTime is not available.\n");
  }
}

char const *pvpCamera::ExpRes::decode() const {
  switch (x) {
    case Millisec: return "Millisec";
    case Microsec: return "Microsec";
    case Sec: return "Sec";
    default: return "UNKNOWN";
  }
}

bool pvpCamera::availExpRes() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_EXP_RES,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get EXP_RES availability");
  return avail;
}

pvpAccess pvpCamera::accessExpRes() throw(pvpException) {
  uns16 access;
  if (!availExpRes())
    throw pvpException("EXP_RES not available");
  if (!pl_get_param(camh,PARAM_EXP_RES,ATTR_ACCESS,&access))
    throw pvpException("Cannot get EXP_RES accessibility");
  return pvpAccess(access);
}

int pvpCamera::countExpRes() throw(pvpException) {
  uns32 count;
  if (!availExpRes())
    throw pvpException("EXP_RES not available");
  if (!pl_get_param(camh,PARAM_EXP_RES,ATTR_COUNT,&count))
    throw pvpException("Cannot get EXP_RES count");
  return count;
}

pvpCamera::ExpRes pvpCamera::getExpRes() throw(pvpException) {
  pvpAccess a = accessExpRes();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_EXP_RES,ATTR_CURRENT,&x))
      throw pvpException("Cannot read EXP_RES");
    return x;
  } else {
    throw pvpException("EXP_RES not accessible for reading");
  }
}

pvpCamera::ExpRes pvpCamera::defaultExpRes() throw(pvpException) {
  pvpAccess a = accessExpRes();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_EXP_RES,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of EXP_RES");
    return x;
  } else {
    throw pvpException("EXP_RES not accessible for reading");
  }
}

pvpCamera::ExpRes pvpCamera::minExpRes() throw(pvpException) {
  pvpAccess a = accessExpRes();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_EXP_RES,ATTR_MIN,&x))
      throw pvpException("Cannot read min of EXP_RES");
    return x;
  } else {
    throw pvpException("EXP_RES not accessible for reading");
  }
}

pvpCamera::ExpRes pvpCamera::maxExpRes() throw(pvpException) {
  pvpAccess a = accessExpRes();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_EXP_RES,ATTR_MAX,&x))
      throw pvpException("Cannot read max of EXP_RES");
    return x;
  } else {
    throw pvpException("EXP_RES not accessible for reading");
  }
}

void pvpCamera::setExpRes(pvpCamera::ExpRes x) throw(pvpException) {
  pvpAccess a = accessExpRes();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    uns32 y = x;
    if (!pl_set_param(camh,PARAM_EXP_RES,(void*)(&y)))
      throw pvpException("Cannot set EXP_RES");
  }
}

void pvpCamera::reportExpRes() throw(pvpException) {
  if (availExpRes()) {
    pvpAccess a = accessExpRes();
    printf("ExpRes: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getExpRes().decode() <<"\n";
        ss << "  min value: " << minExpRes().decode() <<"\n";
        ss << "  max value: " << maxExpRes().decode() <<"\n";
        ss << "  default value: " << defaultExpRes().decode() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countExpRes());
  } else {
    printf("ExpRes is not available.\n");
  }
}

bool pvpCamera::availExpMinTime() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_EXP_MIN_TIME,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get EXP_MIN_TIME availability");
  return avail;
}

pvpAccess pvpCamera::accessExpMinTime() throw(pvpException) {
  uns16 access;
  if (!availExpMinTime())
    throw pvpException("EXP_MIN_TIME not available");
  if (!pl_get_param(camh,PARAM_EXP_MIN_TIME,ATTR_ACCESS,&access))
    throw pvpException("Cannot get EXP_MIN_TIME accessibility");
  return pvpAccess(access);
}

int pvpCamera::countExpMinTime() throw(pvpException) {
  uns32 count;
  if (!availExpMinTime())
    throw pvpException("EXP_MIN_TIME not available");
  if (!pl_get_param(camh,PARAM_EXP_MIN_TIME,ATTR_COUNT,&count))
    throw pvpException("Cannot get EXP_MIN_TIME count");
  return count;
}

float pvpCamera::getExpMinTime() throw(pvpException) {
  pvpAccess a = accessExpMinTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    flt64 x;
    if (!pl_get_param(camh,PARAM_EXP_MIN_TIME,ATTR_CURRENT,&x))
      throw pvpException("Cannot read EXP_MIN_TIME");
    return x;
  } else {
    throw pvpException("EXP_MIN_TIME not accessible for reading");
  }
}

float pvpCamera::defaultExpMinTime() throw(pvpException) {
  pvpAccess a = accessExpMinTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    flt64 x;
    if (!pl_get_param(camh,PARAM_EXP_MIN_TIME,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of EXP_MIN_TIME");
    return x;
  } else {
    throw pvpException("EXP_MIN_TIME not accessible for reading");
  }
}

float pvpCamera::minExpMinTime() throw(pvpException) {
  pvpAccess a = accessExpMinTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    flt64 x;
    if (!pl_get_param(camh,PARAM_EXP_MIN_TIME,ATTR_MIN,&x))
      throw pvpException("Cannot read min of EXP_MIN_TIME");
    return x;
  } else {
    throw pvpException("EXP_MIN_TIME not accessible for reading");
  }
}

float pvpCamera::maxExpMinTime() throw(pvpException) {
  pvpAccess a = accessExpMinTime();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    flt64 x;
    if (!pl_get_param(camh,PARAM_EXP_MIN_TIME,ATTR_MAX,&x))
      throw pvpException("Cannot read max of EXP_MIN_TIME");
    return x;
  } else {
    throw pvpException("EXP_MIN_TIME not accessible for reading");
  }
}

void pvpCamera::setExpMinTime(float x) throw(pvpException) {
  pvpAccess a = accessExpMinTime();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    flt64 y = x;
    if (!pl_set_param(camh,PARAM_EXP_MIN_TIME,(void*)(&y)))
      throw pvpException("Cannot set EXP_MIN_TIME");
  }
}

void pvpCamera::reportExpMinTime() throw(pvpException) {
  if (availExpMinTime()) {
    pvpAccess a = accessExpMinTime();
    printf("ExpMinTime: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getExpMinTime() <<"\n";
        ss << "  min value: " << minExpMinTime() <<"\n";
        ss << "  max value: " << maxExpMinTime() <<"\n";
        ss << "  default value: " << defaultExpMinTime() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countExpMinTime());
  } else {
    printf("ExpMinTime is not available.\n");
  }
}

bool pvpCamera::availExpResIndex() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_EXP_RES_INDEX,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get EXP_RES_INDEX availability");
  return avail;
}

pvpAccess pvpCamera::accessExpResIndex() throw(pvpException) {
  uns16 access;
  if (!availExpResIndex())
    throw pvpException("EXP_RES_INDEX not available");
  if (!pl_get_param(camh,PARAM_EXP_RES_INDEX,ATTR_ACCESS,&access))
    throw pvpException("Cannot get EXP_RES_INDEX accessibility");
  return pvpAccess(access);
}

int pvpCamera::countExpResIndex() throw(pvpException) {
  uns32 count;
  if (!availExpResIndex())
    throw pvpException("EXP_RES_INDEX not available");
  if (!pl_get_param(camh,PARAM_EXP_RES_INDEX,ATTR_COUNT,&count))
    throw pvpException("Cannot get EXP_RES_INDEX count");
  return count;
}

unsigned short pvpCamera::getExpResIndex() throw(pvpException) {
  pvpAccess a = accessExpResIndex();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_RES_INDEX,ATTR_CURRENT,&x))
      throw pvpException("Cannot read EXP_RES_INDEX");
    return x;
  } else {
    throw pvpException("EXP_RES_INDEX not accessible for reading");
  }
}

unsigned short pvpCamera::defaultExpResIndex() throw(pvpException) {
  pvpAccess a = accessExpResIndex();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_RES_INDEX,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of EXP_RES_INDEX");
    return x;
  } else {
    throw pvpException("EXP_RES_INDEX not accessible for reading");
  }
}

unsigned short pvpCamera::minExpResIndex() throw(pvpException) {
  pvpAccess a = accessExpResIndex();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_RES_INDEX,ATTR_MIN,&x))
      throw pvpException("Cannot read min of EXP_RES_INDEX");
    return x;
  } else {
    throw pvpException("EXP_RES_INDEX not accessible for reading");
  }
}

unsigned short pvpCamera::maxExpResIndex() throw(pvpException) {
  pvpAccess a = accessExpResIndex();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns16 x;
    if (!pl_get_param(camh,PARAM_EXP_RES_INDEX,ATTR_MAX,&x))
      throw pvpException("Cannot read max of EXP_RES_INDEX");
    return x;
  } else {
    throw pvpException("EXP_RES_INDEX not accessible for reading");
  }
}

void pvpCamera::setExpResIndex(unsigned short x) throw(pvpException) {
  pvpAccess a = accessExpResIndex();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    uns16 y = x;
    if (!pl_set_param(camh,PARAM_EXP_RES_INDEX,(void*)(&y)))
      throw pvpException("Cannot set EXP_RES_INDEX");
  }
}

void pvpCamera::reportExpResIndex() throw(pvpException) {
  if (availExpResIndex()) {
    pvpAccess a = accessExpResIndex();
    printf("ExpResIndex: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getExpResIndex() <<"\n";
        ss << "  min value: " << minExpResIndex() <<"\n";
        ss << "  max value: " << maxExpResIndex() <<"\n";
        ss << "  default value: " << defaultExpResIndex() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countExpResIndex());
  } else {
    printf("ExpResIndex is not available.\n");
  }
}

char const *pvpCamera::BofEofEnable::decode() const {
  switch (x) {
    case NoFrameIrqs: return "NoFrameIrqs";
    case BeginFrameIrqs: return "BeginFrameIrqs";
    case EndFrameIrqs: return "EndFrameIrqs";
    case BeginEndFrameIrqs: return "BeginEndFrameIrqs";
    default: return "UNKNOWN";
  }
}

bool pvpCamera::availBofEofEnable() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_BOF_EOF_ENABLE,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get BOF_EOF_ENABLE availability");
  return avail;
}

pvpAccess pvpCamera::accessBofEofEnable() throw(pvpException) {
  uns16 access;
  if (!availBofEofEnable())
    throw pvpException("BOF_EOF_ENABLE not available");
  if (!pl_get_param(camh,PARAM_BOF_EOF_ENABLE,ATTR_ACCESS,&access))
    throw pvpException("Cannot get BOF_EOF_ENABLE accessibility");
  return pvpAccess(access);
}

int pvpCamera::countBofEofEnable() throw(pvpException) {
  uns32 count;
  if (!availBofEofEnable())
    throw pvpException("BOF_EOF_ENABLE not available");
  if (!pl_get_param(camh,PARAM_BOF_EOF_ENABLE,ATTR_COUNT,&count))
    throw pvpException("Cannot get BOF_EOF_ENABLE count");
  return count;
}

pvpCamera::BofEofEnable pvpCamera::getBofEofEnable() throw(pvpException) {
  pvpAccess a = accessBofEofEnable();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_ENABLE,ATTR_CURRENT,&x))
      throw pvpException("Cannot read BOF_EOF_ENABLE");
    return x;
  } else {
    throw pvpException("BOF_EOF_ENABLE not accessible for reading");
  }
}

pvpCamera::BofEofEnable pvpCamera::defaultBofEofEnable() throw(pvpException) {
  pvpAccess a = accessBofEofEnable();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_ENABLE,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of BOF_EOF_ENABLE");
    return x;
  } else {
    throw pvpException("BOF_EOF_ENABLE not accessible for reading");
  }
}

pvpCamera::BofEofEnable pvpCamera::minBofEofEnable() throw(pvpException) {
  pvpAccess a = accessBofEofEnable();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_ENABLE,ATTR_MIN,&x))
      throw pvpException("Cannot read min of BOF_EOF_ENABLE");
    return x;
  } else {
    throw pvpException("BOF_EOF_ENABLE not accessible for reading");
  }
}

pvpCamera::BofEofEnable pvpCamera::maxBofEofEnable() throw(pvpException) {
  pvpAccess a = accessBofEofEnable();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_ENABLE,ATTR_MAX,&x))
      throw pvpException("Cannot read max of BOF_EOF_ENABLE");
    return x;
  } else {
    throw pvpException("BOF_EOF_ENABLE not accessible for reading");
  }
}

void pvpCamera::setBofEofEnable(pvpCamera::BofEofEnable x) throw(pvpException) {
  pvpAccess a = accessBofEofEnable();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    uns32 y = x;
    if (!pl_set_param(camh,PARAM_BOF_EOF_ENABLE,(void*)(&y)))
      throw pvpException("Cannot set BOF_EOF_ENABLE");
  }
}

void pvpCamera::reportBofEofEnable() throw(pvpException) {
  if (availBofEofEnable()) {
    pvpAccess a = accessBofEofEnable();
    printf("BofEofEnable: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getBofEofEnable().decode() <<"\n";
        ss << "  min value: " << minBofEofEnable().decode() <<"\n";
        ss << "  max value: " << maxBofEofEnable().decode() <<"\n";
        ss << "  default value: " << defaultBofEofEnable().decode() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countBofEofEnable());
  } else {
    printf("BofEofEnable is not available.\n");
  }
}

bool pvpCamera::availBofEofCount() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_BOF_EOF_COUNT,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get BOF_EOF_COUNT availability");
  return avail;
}

pvpAccess pvpCamera::accessBofEofCount() throw(pvpException) {
  uns16 access;
  if (!availBofEofCount())
    throw pvpException("BOF_EOF_COUNT not available");
  if (!pl_get_param(camh,PARAM_BOF_EOF_COUNT,ATTR_ACCESS,&access))
    throw pvpException("Cannot get BOF_EOF_COUNT accessibility");
  return pvpAccess(access);
}

int pvpCamera::countBofEofCount() throw(pvpException) {
  uns32 count;
  if (!availBofEofCount())
    throw pvpException("BOF_EOF_COUNT not available");
  if (!pl_get_param(camh,PARAM_BOF_EOF_COUNT,ATTR_COUNT,&count))
    throw pvpException("Cannot get BOF_EOF_COUNT count");
  return count;
}

unsigned long pvpCamera::getBofEofCount() throw(pvpException) {
  pvpAccess a = accessBofEofCount();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_COUNT,ATTR_CURRENT,&x))
      throw pvpException("Cannot read BOF_EOF_COUNT");
    return x;
  } else {
    throw pvpException("BOF_EOF_COUNT not accessible for reading");
  }
}

unsigned long pvpCamera::defaultBofEofCount() throw(pvpException) {
  pvpAccess a = accessBofEofCount();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_COUNT,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of BOF_EOF_COUNT");
    return x;
  } else {
    throw pvpException("BOF_EOF_COUNT not accessible for reading");
  }
}

unsigned long pvpCamera::minBofEofCount() throw(pvpException) {
  pvpAccess a = accessBofEofCount();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_COUNT,ATTR_MIN,&x))
      throw pvpException("Cannot read min of BOF_EOF_COUNT");
    return x;
  } else {
    throw pvpException("BOF_EOF_COUNT not accessible for reading");
  }
}

unsigned long pvpCamera::maxBofEofCount() throw(pvpException) {
  pvpAccess a = accessBofEofCount();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    uns32 x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_COUNT,ATTR_MAX,&x))
      throw pvpException("Cannot read max of BOF_EOF_COUNT");
    return x;
  } else {
    throw pvpException("BOF_EOF_COUNT not accessible for reading");
  }
}

void pvpCamera::setBofEofCount(unsigned long x) throw(pvpException) {
  pvpAccess a = accessBofEofCount();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    uns32 y = x;
    if (!pl_set_param(camh,PARAM_BOF_EOF_COUNT,(void*)(&y)))
      throw pvpException("Cannot set BOF_EOF_COUNT");
  }
}

void pvpCamera::reportBofEofCount() throw(pvpException) {
  if (availBofEofCount()) {
    pvpAccess a = accessBofEofCount();
    printf("BofEofCount: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getBofEofCount() <<"\n";
        ss << "  min value: " << minBofEofCount() <<"\n";
        ss << "  max value: " << maxBofEofCount() <<"\n";
        ss << "  default value: " << defaultBofEofCount() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countBofEofCount());
  } else {
    printf("BofEofCount is not available.\n");
  }
}

bool pvpCamera::availBofEofClr() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_BOF_EOF_CLR,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get BOF_EOF_CLR availability");
  return avail;
}

pvpAccess pvpCamera::accessBofEofClr() throw(pvpException) {
  uns16 access;
  if (!availBofEofClr())
    throw pvpException("BOF_EOF_CLR not available");
  if (!pl_get_param(camh,PARAM_BOF_EOF_CLR,ATTR_ACCESS,&access))
    throw pvpException("Cannot get BOF_EOF_CLR accessibility");
  return pvpAccess(access);
}

int pvpCamera::countBofEofClr() throw(pvpException) {
  uns32 count;
  if (!availBofEofClr())
    throw pvpException("BOF_EOF_CLR not available");
  if (!pl_get_param(camh,PARAM_BOF_EOF_CLR,ATTR_COUNT,&count))
    throw pvpException("Cannot get BOF_EOF_CLR count");
  return count;
}

bool pvpCamera::getBofEofClr() throw(pvpException) {
  pvpAccess a = accessBofEofClr();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_CLR,ATTR_CURRENT,&x))
      throw pvpException("Cannot read BOF_EOF_CLR");
    return x;
  } else {
    throw pvpException("BOF_EOF_CLR not accessible for reading");
  }
}

bool pvpCamera::defaultBofEofClr() throw(pvpException) {
  pvpAccess a = accessBofEofClr();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_CLR,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of BOF_EOF_CLR");
    return x;
  } else {
    throw pvpException("BOF_EOF_CLR not accessible for reading");
  }
}

bool pvpCamera::minBofEofClr() throw(pvpException) {
  pvpAccess a = accessBofEofClr();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_CLR,ATTR_MIN,&x))
      throw pvpException("Cannot read min of BOF_EOF_CLR");
    return x;
  } else {
    throw pvpException("BOF_EOF_CLR not accessible for reading");
  }
}

bool pvpCamera::maxBofEofClr() throw(pvpException) {
  pvpAccess a = accessBofEofClr();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_BOF_EOF_CLR,ATTR_MAX,&x))
      throw pvpException("Cannot read max of BOF_EOF_CLR");
    return x;
  } else {
    throw pvpException("BOF_EOF_CLR not accessible for reading");
  }
}

void pvpCamera::setBofEofClr(bool x) throw(pvpException) {
  pvpAccess a = accessBofEofClr();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    boolean y = x;
    if (!pl_set_param(camh,PARAM_BOF_EOF_CLR,(void*)(&y)))
      throw pvpException("Cannot set BOF_EOF_CLR");
  }
}

void pvpCamera::reportBofEofClr() throw(pvpException) {
  if (availBofEofClr()) {
    pvpAccess a = accessBofEofClr();
    printf("BofEofClr: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getBofEofClr() <<"\n";
        ss << "  min value: " << minBofEofClr() <<"\n";
        ss << "  max value: " << maxBofEofClr() <<"\n";
        ss << "  default value: " << defaultBofEofClr() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countBofEofClr());
  } else {
    printf("BofEofClr is not available.\n");
  }
}

bool pvpCamera::availCircBuffer() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_CIRC_BUFFER,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get CIRC_BUFFER availability");
  return avail;
}

pvpAccess pvpCamera::accessCircBuffer() throw(pvpException) {
  uns16 access;
  if (!availCircBuffer())
    throw pvpException("CIRC_BUFFER not available");
  if (!pl_get_param(camh,PARAM_CIRC_BUFFER,ATTR_ACCESS,&access))
    throw pvpException("Cannot get CIRC_BUFFER accessibility");
  return pvpAccess(access);
}

int pvpCamera::countCircBuffer() throw(pvpException) {
  uns32 count;
  if (!availCircBuffer())
    throw pvpException("CIRC_BUFFER not available");
  if (!pl_get_param(camh,PARAM_CIRC_BUFFER,ATTR_COUNT,&count))
    throw pvpException("Cannot get CIRC_BUFFER count");
  return count;
}

bool pvpCamera::getCircBuffer() throw(pvpException) {
  pvpAccess a = accessCircBuffer();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_CIRC_BUFFER,ATTR_CURRENT,&x))
      throw pvpException("Cannot read CIRC_BUFFER");
    return x;
  } else {
    throw pvpException("CIRC_BUFFER not accessible for reading");
  }
}

bool pvpCamera::defaultCircBuffer() throw(pvpException) {
  pvpAccess a = accessCircBuffer();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_CIRC_BUFFER,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of CIRC_BUFFER");
    return x;
  } else {
    throw pvpException("CIRC_BUFFER not accessible for reading");
  }
}

bool pvpCamera::minCircBuffer() throw(pvpException) {
  pvpAccess a = accessCircBuffer();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_CIRC_BUFFER,ATTR_MIN,&x))
      throw pvpException("Cannot read min of CIRC_BUFFER");
    return x;
  } else {
    throw pvpException("CIRC_BUFFER not accessible for reading");
  }
}

bool pvpCamera::maxCircBuffer() throw(pvpException) {
  pvpAccess a = accessCircBuffer();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    boolean x;
    if (!pl_get_param(camh,PARAM_CIRC_BUFFER,ATTR_MAX,&x))
      throw pvpException("Cannot read max of CIRC_BUFFER");
    return x;
  } else {
    throw pvpException("CIRC_BUFFER not accessible for reading");
  }
}

void pvpCamera::setCircBuffer(bool x) throw(pvpException) {
  pvpAccess a = accessCircBuffer();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    boolean y = x;
    if (!pl_set_param(camh,PARAM_CIRC_BUFFER,(void*)(&y)))
      throw pvpException("Cannot set CIRC_BUFFER");
  }
}

void pvpCamera::reportCircBuffer() throw(pvpException) {
  if (availCircBuffer()) {
    pvpAccess a = accessCircBuffer();
    printf("CircBuffer: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getCircBuffer() <<"\n";
        ss << "  min value: " << minCircBuffer() <<"\n";
        ss << "  max value: " << maxCircBuffer() <<"\n";
        ss << "  default value: " << defaultCircBuffer() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countCircBuffer());
  } else {
    printf("CircBuffer is not available.\n");
  }
}

bool pvpCamera::availHwAutostop() throw(pvpException) {
  rs_bool avail;
  if (!pl_get_param(camh,PARAM_HW_AUTOSTOP,ATTR_AVAIL,&avail))
    throw pvpException("Cannot get HW_AUTOSTOP availability");
  return avail;
}

pvpAccess pvpCamera::accessHwAutostop() throw(pvpException) {
  uns16 access;
  if (!availHwAutostop())
    throw pvpException("HW_AUTOSTOP not available");
  if (!pl_get_param(camh,PARAM_HW_AUTOSTOP,ATTR_ACCESS,&access))
    throw pvpException("Cannot get HW_AUTOSTOP accessibility");
  return pvpAccess(access);
}

int pvpCamera::countHwAutostop() throw(pvpException) {
  uns32 count;
  if (!availHwAutostop())
    throw pvpException("HW_AUTOSTOP not available");
  if (!pl_get_param(camh,PARAM_HW_AUTOSTOP,ATTR_COUNT,&count))
    throw pvpException("Cannot get HW_AUTOSTOP count");
  return count;
}

short pvpCamera::getHwAutostop() throw(pvpException) {
  pvpAccess a = accessHwAutostop();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    int16 x;
    if (!pl_get_param(camh,PARAM_HW_AUTOSTOP,ATTR_CURRENT,&x))
      throw pvpException("Cannot read HW_AUTOSTOP");
    return x;
  } else {
    throw pvpException("HW_AUTOSTOP not accessible for reading");
  }
}

short pvpCamera::defaultHwAutostop() throw(pvpException) {
  pvpAccess a = accessHwAutostop();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    int16 x;
    if (!pl_get_param(camh,PARAM_HW_AUTOSTOP,ATTR_DEFAULT,&x))
      throw pvpException("Cannot read default of HW_AUTOSTOP");
    return x;
  } else {
    throw pvpException("HW_AUTOSTOP not accessible for reading");
  }
}

short pvpCamera::minHwAutostop() throw(pvpException) {
  pvpAccess a = accessHwAutostop();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    int16 x;
    if (!pl_get_param(camh,PARAM_HW_AUTOSTOP,ATTR_MIN,&x))
      throw pvpException("Cannot read min of HW_AUTOSTOP");
    return x;
  } else {
    throw pvpException("HW_AUTOSTOP not accessible for reading");
  }
}

short pvpCamera::maxHwAutostop() throw(pvpException) {
  pvpAccess a = accessHwAutostop();
  if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
    int16 x;
    if (!pl_get_param(camh,PARAM_HW_AUTOSTOP,ATTR_MAX,&x))
      throw pvpException("Cannot read max of HW_AUTOSTOP");
    return x;
  } else {
    throw pvpException("HW_AUTOSTOP not accessible for reading");
  }
}

void pvpCamera::setHwAutostop(short x) throw(pvpException) {
  pvpAccess a = accessHwAutostop();
  if (a==pvpAccess::WriteOnly || a==pvpAccess::ReadWrite) {
    int16 y = x;
    if (!pl_set_param(camh,PARAM_HW_AUTOSTOP,(void*)(&y)))
      throw pvpException("Cannot set HW_AUTOSTOP");
  }
}

void pvpCamera::reportHwAutostop() throw(pvpException) {
  if (availHwAutostop()) {
    pvpAccess a = accessHwAutostop();
    printf("HwAutostop: %s\n",a.decode());
    if (a==pvpAccess::ReadOnly || a==pvpAccess::ReadWrite) {
      QString s;
      { QTextStream ss(&s);
        ss << "  current value: " << getHwAutostop() <<"\n";
        ss << "  min value: " << minHwAutostop() <<"\n";
        ss << "  max value: " << maxHwAutostop() <<"\n";
        ss << "  default value: " << defaultHwAutostop() <<"\n";
      }
      printf("%s",s.toAscii().data());
    }
    printf("  count: %i\n",countHwAutostop());
  } else {
    printf("HwAutostop is not available.\n");
  }
}

void pvpCamera::reportClass3() throw(pvpException) {
  reportExpTime();
  reportExpRes();
  reportExpMinTime();
  reportExpResIndex();
  reportBofEofEnable();
  reportBofEofCount();
  reportBofEofClr();
  reportCircBuffer();
  reportHwAutostop();
}

