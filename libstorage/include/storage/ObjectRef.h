#pragma once

#ifndef STORAGE_OBJECTREF_H
#define STORAGE_OBJECTREF_H

#include <stdexcept>
#include <iostream>

namespace tmk::storage {

/**
 * @brief ObjectRef's purpose is to _temporarily_ store reference to object
 * allocated in shared memory, it can be copied, stacked in containers, etc.
 * Function valid() can check correct assignment of reference
 * Сontrol of the shared memory _segment_ lifetime is not considered here.
 */
template <typename ObjTy> struct ObjectRef {
  ObjectRef() = default;
  ObjectRef(ObjTy &f) : pObj(&f) {}
  operator bool() { return valid(); }
  bool valid() { return pObj != nullptr; }
  operator ObjTy&() { return get(); }
  ObjTy &operator()() { return get(); }
  ObjTy &get() {
    if (!valid()) {
      throw std::logic_error("Reference is not initialized");
    }
    return *pObj;
  }


//==================  add some func [1]=========================================
// change files   .h: ObjectDescriptor.h   ObjectRef.h,  Frameset.h, Ipc.h, 
// change files .cpp: ObjectDescriptor.cpp Frameset.cpp, Ipc.cpp, 
 
  void reset() {pObj = nullptr;}
  void showPTR() const {std::cout<< "\nObj ptr: "<< pObj;} 
  ObjTy* getPTR() const {return pObj;}
//==================  add some func [1]=========================================

 protected:
  ObjTy *pObj = nullptr;
};

} // namespace tmk::storage
#endif // !STORAGE_OBJECTREF_H

