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
// Created by zzu_softboy on 26/07/2017.

#include <ostream>
#include "zapi/vm/Callable.h"
#include "zapi/vm/internal/CallablePrivate.h"
#include "zapi/lang/Parameters.h"
#include "zapi/kernel/OrigException.h"

namespace zapi
{
namespace vm
{

namespace internal
{

CallablePrivate::CallablePrivate(const char *name, ZendCallable callable, const lang::Arguments &arguments)
   : m_callable(callable),
     m_name(name),
     m_argc(arguments.size()),
     m_argv(new zend_internal_arg_info[arguments.size() + 2])
{
   // first entry record function infomation, so skip it
   int i = 1;
   for (auto &argument : arguments) {
      if (argument.isRequired()) {
         m_required++;
      }
      setupCallableArgInfo(&m_argv[i++], argument);
   }
   // last entry, we save extra infomation about self
   // this just for m_callable = nullptr and self::invoke been set for entry->handler
   m_argv[i].class_name = nullptr;
   m_argv[i].name = nullptr;
}

CallablePrivate::CallablePrivate(const char *name, const lang::Arguments &arguments)
   : CallablePrivate(name, nullptr,  arguments)
{}

void CallablePrivate::initialize(zend_function_entry *entry, const char *className, int flags) const
{
   if (m_callable) {
      entry->handler = m_callable;
   } else {
      // install ourselves in the extra argument
      m_argv[m_argc + 1].class_name = reinterpret_cast<const char*>(this);
      // we use our own invoke method, which does a lookup
      // in the map we just installed ourselves in
      entry->handler = &Callable::invoke;
   }
   entry->fname = m_name.data();
   entry->arg_info = m_argv.get();
   entry->num_args = m_argc;
   entry->flags = flags;
   initialize(reinterpret_cast<zend_internal_function_info *>(m_argv.get()), className);
}

void CallablePrivate::initialize(zend_internal_function_info *info, const char *className) const
{
   info->class_name = className;
   info->required_num_args = m_required;
   info->type_hint = static_cast<unsigned char>(m_return);
   // current we don't support return by reference
   info->return_reference = false;
   // since php 5.6 there are _allow_null and _is_variadic properties. It's
   // not exactly clear what they do (@todo find this out) so for now we set
   // them to false
   info->allow_null = false;
   info->_is_variadic = false;
}

void CallablePrivate::initialize(const std::string &prefix, zend_function_entry *entry)
{
   // if there is a namespace prefix, we should adjust the name
   if (!prefix.empty()) {
      m_name = prefix + '\\' + m_name;
   }
   // call base initialize
   CallablePrivate::initialize(entry);
}

void CallablePrivate::setupCallableArgInfo(zend_internal_arg_info *info, const lang::Argument &arg) const
{
   info->name = arg.getName();
   if (arg.getType() == Type::Object) {
      info->class_name = arg.getClassName();
   } else {
      info->class_name = nullptr;
   }
   switch (arg.getType()) {
   case Type::Undefined:
      info->type_hint = IS_UNDEF;
      break;
   case Type::Null:
      // here we accept everything when is null? is good for this, any good choice?
      info->type_hint = IS_UNDEF;
      break;
   case Type::Boolean:
   case Type::True:
   case Type::False:
      info->type_hint = _IS_BOOL;
      break;
   case Type::Numeric:
      info->type_hint = IS_LONG;
      break;
   case Type::Double:
      info->type_hint = IS_DOUBLE;
      break;
   case Type::String:
      info->type_hint = IS_STRING;
      break;
   case Type::Array:
      info->type_hint = IS_ARRAY;
      break;
   case Type::Object:
      info->type_hint = IS_OBJECT;
      break;
   case Type::Callable:
      info->type_hint = IS_CALLABLE;
      break;
   default:
      info->type_hint = IS_UNDEF;
      break;
   }
   // from PHP 5.6 and onwards, an is_variadic property can be set, this
   // specifies whether this argument is the first argument that specifies
   // the type for a variable length list of arguments. For now we only
   // support methods and functions with a fixed number of arguments.
   info->is_variadic       = arg.isVariadic();
   info->allow_null        = arg.isNullable();
   info->pass_by_reference = arg.isReference();
}

} // internal

using zapi::kernel::Exception;

Callable::Callable()
{}

Callable::Callable(const char *name, ZendCallable callable, const lang::Arguments &arguments)
   : m_implPtr(new CallablePrivate(name, callable, arguments))
{
}

Callable::Callable(const char *name, const lang::Arguments &arguments)
   : Callable(name, nullptr, arguments)
{}

Callable::Callable(Callable &&other) ZAPI_DECL_NOEXCEPT
   : m_implPtr(std::move(other.m_implPtr))
{
}

Callable::Callable(CallablePrivate *implPtr)
   : m_implPtr(implPtr)
{}

Callable::Callable(const Callable &other)
   : m_implPtr(other.m_implPtr)
{}


Callable::~Callable()
{}

Callable &Callable::operator=(const Callable &other)
{
   if (this != &other) {
      m_implPtr = other.m_implPtr;
   }
   return *this;
}

Callable &Callable::operator=(Callable &&other) ZAPI_DECL_NOEXCEPT
{
   assert(this != &other);
   m_implPtr = std::move(other.m_implPtr);
   return *this;
}

void Callable::setupCallableArgInfo(zend_internal_arg_info *info, const lang::Argument &arg) const
{
   getImplPtr()->setupCallableArgInfo(info, arg);
}

void Callable::invoke(INTERNAL_FUNCTION_PARAMETERS)
{
   uint32_t argc       = EX(func)->common.num_args;
   zend_arg_info *info = EX(func)->common.arg_info;
   assert(info[argc].class_name != nullptr && info[argc].name == nullptr);
   Callable *callable = reinterpret_cast<Callable *>(info[argc].class_name);
   
   // check if sufficient parameters were passed (for some reason this check
   // is not done by Zend, so we do it here ourselves)
   if (ZEND_NUM_ARGS() < callable->m_implPtr->m_required) {
      zapi::warning << get_active_function_name() << "() expects at least "
                    << callable->m_implPtr->m_required << " parameter(s)," << ZEND_NUM_ARGS() 
                    << " given" << std::flush;
      RETURN_NULL();
   } else {
      Parameters params(getThis(), ZEND_NUM_ARGS());
      // the function we called may throw exception
      try {
         Variant result(callable->invoke(params));
         RETVAL_ZVAL(&result.getZval(), 1, 0);
      } catch (Exception &exception) {
         kernel::process_exception(exception);
      }
   }
}

void Callable::initialize(zend_function_entry *entry, const char *className, int flags) const
{
   ZAPI_D(const Callable);
   implPtr->initialize(entry, className, flags);
}

void Callable::initialize(zend_internal_function_info *info, const char *className) const
{
   ZAPI_D(const Callable);
   implPtr->initialize(info, className);
}

void Callable::initialize(const std::string &prefix, zend_function_entry *entry)
{
   ZAPI_D(Callable);
   implPtr->initialize(prefix, entry);
}

} // vm
} // zapi