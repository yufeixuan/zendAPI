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
// Created by softboy on 2017/07/26.

#include "zapi/lang/Argument.h"

namespace zapi
{
namespace lang
{

namespace internal
{

class ArgumentPrivate
{
public:
   ArgumentPrivate(const char *name, Type type, bool required = true, bool byReference = false, 
                   bool variadic = false);
   ArgumentPrivate(const char *name, const char *className, bool nullable = true, 
                   bool required = true, bool byReference = false, bool variadic = false);
   ArgumentPrivate(const ArgumentPrivate &other);
   Type m_type = Type::Null;
   bool m_nullable = false;
   bool m_required = true;
   bool m_byReference = false;
   bool m_variadic = false;
   const char *m_name = nullptr;
   const char *m_className = nullptr;
};

ArgumentPrivate::ArgumentPrivate(const char *name, Type type, bool required, 
                                 bool byReference, bool isVariadic)
   : m_type(type), 
     m_required(required), 
     m_byReference(byReference),
     m_variadic(isVariadic),
     m_name(name)
{}

ArgumentPrivate::ArgumentPrivate(const char *name, const char *className, bool nullable, 
                                 bool required, bool byReference, bool isVariadic)
   : m_nullable(nullable), 
     m_required(required), 
     m_byReference(byReference),
     m_variadic(isVariadic),
     m_name(name), 
     m_className(className)
{}

ArgumentPrivate::ArgumentPrivate(const ArgumentPrivate &other)
   : m_nullable(other.m_nullable), 
     m_required(other.m_required), 
     m_byReference(other.m_byReference),
     m_variadic(other.m_variadic),
     m_name(other.m_name), 
     m_className(other.m_className)
{}

} // internal


Argument::Argument(const char *name, Type type, bool required, 
                   bool byReference, bool isVariadic)
   : m_implPtr(new ArgumentPrivate(name, type, required, byReference, isVariadic))
{}

Argument::Argument(const char *name, const char *className, bool nullable, 
                   bool required, bool byReference, bool isVariadic)
   : m_implPtr(new ArgumentPrivate(name, className, nullable, required, byReference, isVariadic))
{}

Argument::Argument(const Argument &other)
   : m_implPtr(new ArgumentPrivate(*other.m_implPtr))
{
}

Argument::Argument(Argument &&other) ZAPI_DECL_NOEXCEPT
   : m_implPtr(std::move(other.m_implPtr))
{
}

Argument &Argument::operator=(const Argument &other)
{
   if (this != &other) {
      m_implPtr.reset(new ArgumentPrivate(*other.m_implPtr));
   }
   return *this;
}

Argument &Argument::operator=(Argument &&other) ZAPI_DECL_NOEXCEPT
{
   assert(this != &other);
   m_implPtr = std::move(other.m_implPtr);
   return *this;
}

Argument::~Argument()
{}

bool Argument::isNullable() const
{
   ZAPI_D(const Argument);
   return implPtr->m_nullable;
}

bool Argument::isReference() const
{
   ZAPI_D(const Argument);
   return implPtr->m_byReference;
}

bool Argument::isRequired() const
{
   ZAPI_D(const Argument);
   return implPtr->m_required;
}

bool Argument::isVariadic() const
{
   ZAPI_D(const Argument);
   return implPtr->m_variadic;
}

const char *Argument::getName() const
{
   ZAPI_D(const Argument);
   return implPtr->m_name;
}

Type Argument::getType() const
{
   ZAPI_D(const Argument);
   return implPtr->m_type;
}

const char *Argument::getClassName() const
{
   ZAPI_D(const Argument);
   return implPtr->m_className;
}

ValueArgument::~ValueArgument()
{}

RefArgument::~RefArgument()
{}

VariadicArgument::~VariadicArgument()
{}

} // lang
} // zapi
