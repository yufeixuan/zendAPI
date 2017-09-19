// Copyright 2017-2018 zzu_softboy <zzu_softboy@163.com>
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Created by zzu_softboy on 2017/08/09.

#include "zapi/ds/NumericVariant.h"
#include "zapi/ds/DoubleVariant.h"
#include "zapi/ds/ArrayItemProxy.h"
#include <cmath>

namespace zapi
{
namespace ds
{

NumericVariant::NumericVariant()
   : Variant(0)
{}

NumericVariant::NumericVariant(std::int8_t value)
   : Variant(value)
{}

NumericVariant::NumericVariant(std::int16_t value)
   : Variant(value)
{}

NumericVariant::NumericVariant(std::int32_t value)
   : Variant(value)
{}

#if SIZEOF_ZEND_LONG == 8
NumericVariant::NumericVariant(std::int64_t value)
   : Variant(value)
{}
#endif

NumericVariant::NumericVariant(zval &other, bool isRef)
   : NumericVariant(&other, isRef)
{}

NumericVariant::NumericVariant(zval &&other, bool isRef)
   : NumericVariant(&other, isRef)
{}

NumericVariant::NumericVariant(zval *other, bool isRef)
{
   zval *self = getUnDerefZvalPtr();
   if (nullptr != other) {
      if ((isRef && (Z_TYPE_P(other) == IS_LONG ||
                    (Z_TYPE_P(other) == IS_REFERENCE && Z_TYPE_P(Z_REFVAL_P(other)) == IS_LONG))) ||
          (!isRef && (Z_TYPE_P(other) == IS_REFERENCE && Z_TYPE_P(Z_REFVAL_P(other)) == IS_LONG))) {
         // for support pass ref arg when pass variant args
         ZVAL_MAKE_REF(other);
         zend_reference *ref = Z_REF_P(other);
         ++GC_REFCOUNT(ref);
         ZVAL_REF(self, ref);
      } else {
         // here the zval of other pointer maybe not initialize
         ZVAL_DUP(self, other);
         convert_to_long(self);
      }
   } else {
      ZVAL_LONG(self, 0);
   }
}

NumericVariant::NumericVariant(const NumericVariant &other)
   : Variant(other)
{}

NumericVariant::NumericVariant(NumericVariant &other, bool isRef)
{
   zval *self = getUnDerefZvalPtr();
   if (!isRef) {
      ZVAL_LONG(self, other.toLong());
   } else {
      zval *source = other.getUnDerefZvalPtr();
      ZVAL_MAKE_REF(source);
      ZVAL_COPY(self, source);
   }
}

NumericVariant::NumericVariant(NumericVariant &&other) ZAPI_DECL_NOEXCEPT
   : Variant(std::move(other))
{}

NumericVariant::NumericVariant(const Variant &other)
{
   zval *from = const_cast<zval *>(other.getZvalPtr());
   zval *self = getZvalPtr();
   if (other.getType() == Type::Numeric) {
      ZVAL_LONG(self, zval_get_long(from));
   } else {
      zval temp;
      ZVAL_DUP(&temp, from);
      convert_to_long(&temp);
      ZVAL_COPY_VALUE(self, &temp);
   }
}

NumericVariant::NumericVariant(Variant &&other)
   : Variant(std::move(other))
{
   if (getType() != Type::Long) {
      convert_to_long(getUnDerefZvalPtr());
   }
}

//NumericVariant::operator zapi_long () const
//{
//   return zval_get_long(const_cast<zval *>(getZvalPtr()));
//}

//NumericVariant::operator int () const
//{
//   return zval_get_long(const_cast<zval *>(getZvalPtr()));
//}

zapi_long NumericVariant::toLong() const ZAPI_DECL_NOEXCEPT
{
   return zval_get_long(const_cast<zval *>(getZvalPtr()));
}

bool NumericVariant::toBool() const ZAPI_DECL_NOEXCEPT
{
   return zval_get_long(const_cast<zval *>(getZvalPtr()));
}

NumericVariant &NumericVariant::operator =(std::int8_t other)
{
   ZVAL_LONG(getZvalPtr(), static_cast<zapi_long>(other));
   return *this;
}

NumericVariant &NumericVariant::operator =(std::int16_t other)
{
   ZVAL_LONG(getZvalPtr(), static_cast<zapi_long>(other));
   return *this;
}

NumericVariant &NumericVariant::operator =(std::int32_t other)
{
   ZVAL_LONG(getZvalPtr(), static_cast<zapi_long>(other));
   return *this;
}

NumericVariant &NumericVariant::operator =(std::int64_t other)
{
   ZVAL_LONG(getZvalPtr(), static_cast<zapi_long>(other));
   return *this;
}

NumericVariant &NumericVariant::operator =(double other)
{
   ZVAL_LONG(getZvalPtr(), static_cast<zapi_long>(other));
   return *this;
}

NumericVariant &NumericVariant::operator =(const NumericVariant &other)
{
   if (this != &other) {
      ZVAL_LONG(getZvalPtr(), other.toLong());
   }
   return *this;
}

NumericVariant &NumericVariant::operator =(const DoubleVariant &other)
{
   ZVAL_LONG(getZvalPtr(), static_cast<zapi_long>(other.toDouble()));
   return *this;
}

NumericVariant &NumericVariant::operator =(ArrayItemProxy &&other)
{
   return operator =(other.toNumericVariant());
}

NumericVariant &NumericVariant::operator =(const Variant &other)
{
   zval *self = getZvalPtr();
   zval *from = const_cast<zval *>(other.getZvalPtr());
   if (other.getType() == Type::Long) {
      ZVAL_LONG(self, Z_LVAL_P(from));
   } else {
      zval temp;
      ZVAL_DUP(&temp, from);
      convert_to_long(&temp);
      ZVAL_COPY_VALUE(self, &temp);
   }
   return *this;
}

NumericVariant &NumericVariant::operator ++()
{
   ZVAL_LONG(getZvalPtr(), toLong() + 1);
   return *this;
}

NumericVariant NumericVariant::operator ++(int)
{
   NumericVariant ret(toLong());
   ZVAL_LONG(getZvalPtr(), toLong() + 1);
   return ret;
}

NumericVariant &NumericVariant::operator --()
{
   ZVAL_LONG(getZvalPtr(), toLong() - 1);
   return *this;
}

NumericVariant NumericVariant::operator --(int)
{
   NumericVariant ret(toLong());
   ZVAL_LONG(getZvalPtr(), toLong() - 1);
   return ret;
}

NumericVariant &NumericVariant::operator +=(double value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() + std::lround(value));
   return *this;
}

NumericVariant &NumericVariant::operator +=(const NumericVariant &value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() + value.toLong());
   return *this;
}

NumericVariant &NumericVariant::operator -=(double value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() - std::lround(value));
   return *this;
}

NumericVariant &NumericVariant::operator -=(const NumericVariant &value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() - value.toLong());
   return *this;
}

NumericVariant &NumericVariant::operator *=(double value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() * std::lround(value));
   return *this;
}

NumericVariant &NumericVariant::operator *=(const NumericVariant &value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() * value.toLong());
   return *this;
}

NumericVariant &NumericVariant::operator /=(double value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() / std::lround(value));
   return *this;
}

NumericVariant &NumericVariant::operator /=(const NumericVariant &value) ZAPI_DECL_NOEXCEPT
{
   ZVAL_LONG(getZvalPtr(), toLong() / value.toLong());
   return *this;
}

NumericVariant::~NumericVariant() ZAPI_DECL_NOEXCEPT
{}

bool operator ==(const NumericVariant &lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() == rhs.toLong();
}

bool operator !=(const NumericVariant &lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() != rhs.toLong();
}

bool operator <(const NumericVariant &lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() < rhs.toLong();
}

bool operator <=(const NumericVariant &lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() <= rhs.toLong();
}

bool operator >(const NumericVariant &lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() > rhs.toLong();
}

bool operator >=(const NumericVariant &lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() >= rhs.toLong();
}

bool operator ==(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return (lhs - static_cast<double>(rhs.toLong())) == 0;
}

bool operator !=(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return (lhs - static_cast<double>(rhs.toLong())) != 0;
}

bool operator <(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return (lhs - static_cast<double>(rhs.toLong())) < 0;
}

bool operator <=(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return (lhs - static_cast<double>(rhs.toLong())) <= 0;
}

bool operator >(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return (lhs - static_cast<double>(rhs.toLong())) > 0;
}

bool operator >=(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return (lhs - static_cast<double>(rhs.toLong())) >= 0;
}

bool operator ==(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return (static_cast<double>(lhs.toLong()) - rhs) == 0;
}

bool operator !=(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return (static_cast<double>(lhs.toLong()) - rhs) != 0;
}

bool operator <(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return (static_cast<double>(lhs.toLong()) - rhs) < 0;
}

bool operator <=(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return (static_cast<double>(lhs.toLong()) - rhs) <= 0;
}

bool operator >(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return (static_cast<double>(lhs.toLong()) - rhs) > 0;
}

bool operator >=(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return (static_cast<double>(lhs.toLong()) - rhs) >= 0;
}

double operator +(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs + rhs.toLong();
}

double operator -(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs - rhs.toLong();
}

double operator *(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs * rhs.toLong();
}

double operator /(double lhs, const NumericVariant &rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs / rhs.toLong();
}

double operator +(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() + rhs;
}

double operator -(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() - rhs;
}

double operator *(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() * rhs;
}

double operator /(const NumericVariant &lhs, double rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() / rhs;
}

zapi_long operator +(const NumericVariant &lhs, NumericVariant rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() + rhs.toLong();
}

zapi_long operator -(const NumericVariant &lhs, NumericVariant rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() + rhs.toLong();
}

zapi_long operator *(const NumericVariant &lhs, NumericVariant rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() + rhs.toLong();
}

zapi_long operator /(const NumericVariant &lhs, NumericVariant rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() + rhs.toLong();
}

zapi_long operator %(const NumericVariant &lhs, NumericVariant rhs) ZAPI_DECL_NOEXCEPT
{
   return lhs.toLong() + rhs.toLong();
}

} // ds
} // zapi
