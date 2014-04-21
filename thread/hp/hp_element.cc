#include "thread/hp/hp_element.h"

namespace ucf {
namespace thread {
namespace hp {
/* This function performs a hazard pointer watch on a descriptor
 * 
 * @param the slot number to place the watch value, the value to place, the
 * address the object was read from, and the expected value for the object
 */

bool watch(slot_id, HPElement *descr, std::atomic<void *> *address,
           void *expected) {
  tl_thread_info.hazardPointer->watch(slot, descr);

  if (address->load() != expected) {
    tl_thread_info.hazardPointer->clear_watch(slot);
    return false;
  } else {
    bool res = descr->advance_watch(address, expected);
    if (res) {
      return true;
    } else {
      tl_thread_info.hazardPointer->clear_watch(slot);
      return false;
    }
  }
}

bool watch(slot_id slot, void *value, std::atomic<void *> *address,
           void *expected) {
  tl_thread_info.hazardPointer->watch(slot, descr);

  if (address->load() != expected) {
    tl_thread_info.hazardPointer->clear_watch(slot);
    return false;
  } else {
    return true;
  }
}

/* This function performs a hazard pointer unwatch on a descriptor
 * 
 * @param the slot number to remove the watch value, the descriptor to unwatch
 */
void unwatch(slot_id slot, HPElement *descr) {
  tl_thread_info.hazardPointer->clear_watch(slot);
  descr->advance_unwatch();
}


/* This function checks for a hazard pointer watch on a descriptor
 * 
 * @param the descriptor to check
 */
bool is_watched(HPElement *descr) {
  if (tl_thread_info.hazardPointer->contains(descr)) {
    return descr->advance_is_watched();
  } else {
    return true;
  }
}

bool is_watched(void *value) {
  return tl_thread_info.hazardPointer->contains(value);
}


/* This function removes a descriptor that was placed at an address
 * It protects it by using hazard pointers
 *
 * @param 
 */

void * remove_hp_element(void *expected, std::atomic<void *> *address) {
  RecursiveAction recurse();
  void *newValue;
  if (tl_thread_info.recursive_return) {
    newValue = nullptr;  // result not used
  } else {
    HPElement *descr = unmark(expected);
    if (watch(descr, address, expected)) {
      newValue = descr->complete(t, address);
      unwatch(descr);
    } else {
      newValue = address->load();
    }
  }
  return newValue;
}

}  // namespace hp
}  // namespace thread
}  // namespace ucf
