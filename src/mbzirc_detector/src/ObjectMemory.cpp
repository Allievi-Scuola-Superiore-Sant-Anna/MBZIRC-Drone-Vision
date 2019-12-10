#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iostream>

#include "object_memory/ObjectMemory.hpp"


int clamp (int n, int lower, int upper)
{
    return std::max(lower, std::min(n, upper));
}

/* Compute a heuristic distance between two bounding bboxes */
unsigned int ObjectMemory::computeBBoxDistance(const BBox& b1, const BBox& b2)
{
    // TODO: improve this
    int l1 = std::max(b1.w, b1.h);
    int l2 = std::max(b2.w, b2.h);
    return std::abs(b1.x - b2.x) + std::abs(b1.y - b2.y);
}


/* Decrease the counter of all histories */
void ObjectMemory::decreaseAllCounters()
{
    for (auto &h : histories)
        h.counter = clamp(h.counter-dec, min_counter, max_counter);
}


/* Return true if the history counter is 0 or less */
bool counter_expired(const ObjHistory& h)
{
    return (h.counter <= 0);
}


/* Cleanup histories with counter <= 0 */
void ObjectMemory::cleanExpiredHistories()
{
    histories.remove_if(counter_expired);
}


/* Returns an iterator to the closest history w.r.t object b.
 * Returns histories.end() if distance is larger than max value. */
std::list<ObjHistory>::iterator ObjectMemory::findClosestHistory(const BBox& b)
{
    unsigned int closest_dist = UINT_MAX;
    std::list<ObjHistory>::iterator closest = histories.end();
    std::list<ObjHistory>::iterator h;

    // For each history
    for (h=histories.begin(); h != histories.end(); h++) {
        // If type matches and minimum distance so far, update closest iterator
        if (b.obj_class.compare(h->bbox.obj_class) == 0) {
            unsigned int hist_dist = computeBBoxDistance(b, h->bbox);
            if (hist_dist < closest_dist) {
                closest_dist = hist_dist;
                closest = h;
            }
        }
    }

    // Return closest (if not too far)
    if (closest_dist  < max_dist) {
        return closest;
    } else {
        return histories.end();
    }
}


bool ObjectMemory::addNewHistory(BBox b)
{
    if (histories.size() < max_objects) {
        ObjHistory oh = {(int)inc, b};
        histories.push_back(oh);
        return true;
    } else {
        return false;
    }
}


ObjectMemory::ObjectMemory(unsigned max_objects, unsigned max_dist, unsigned inc, unsigned dec,
    unsigned min_counter, unsigned max_counter, unsigned thr_counter)
{
    this->max_objects = max_objects;
    this->max_dist = max_dist;
    this->inc = inc;
    this->dec = dec;
    this->min_counter = min_counter;
    this->max_counter = max_counter;
    this->thr_counter = thr_counter;
}


ObjectMemory::~ObjectMemory() {}


/* Return a vector containing the bounding boxes from histories */
std::vector<BBox> ObjectMemory::getObjects() {

    std::vector<BBox> ret_bboxes;

    for (const auto& h: histories) {
        if (h.counter >= thr_counter)
            ret_bboxes.push_back(h.bbox);
    }

    return ret_bboxes;
}


void ObjectMemory::putObjects(std::vector<BBox> new_bboxes)
{
    std::list<ObjHistory>::iterator closest;

    // Decrement the counter of each history
    decreaseAllCounters();

    // For each detected bounding box
    for (const auto &b : new_bboxes) {
        // Find the closest history
        closest = findClosestHistory(b);
        if (closest != histories.end()) {
            // if a close history exists
            std::cout << "Found a close history!" << std::endl;
            closest->counter = clamp(closest->counter + inc + 1, min_counter, max_counter);
            closest->bbox = b;
        } else {
            // if no close history exists, add a new one (if enough space)
            std::cout << "No close history!" << std::endl;
            addNewHistory(b);
        }
    }

    // Remove histories whose counter is 0
    cleanExpiredHistories();
}


std::ostream& operator<<(std::ostream& os, const ObjectMemory& om)
{
    os << "Histories: " << om.histories.size() << std::endl;
    for (const auto &h : om.histories) {
        os << "  cnt: " << h.counter << " (x,y,w,h): " 
            << h.bbox.x << ", "  << h.bbox.y << ", "
            << h.bbox.w << ", "  << h.bbox.h << ", "
            << "class: " << h.bbox.obj_class << std::endl;
    }
    return os;
}
