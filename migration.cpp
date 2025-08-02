#include "migration.h"

Migration::Migration() {

}

Migration::Migration(const Migration& src) : base(src.base), regions(src.regions), nationalities(src.nationalities) {

}

Migration::~Migration() {

}

Migration& Migration::operator=(const Migration & migr) {
    if (this == &migr) {
        return *this;
    }
    base.clear();
    for (int i{}; i < migr.base.size(); ++i) {
        base[i] = migr.base[i];
    }
    return *this;
}
