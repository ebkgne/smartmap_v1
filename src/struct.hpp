#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <set>

#include "member.hpp"
#include "data.hpp"
#include "log.hpp"



    struct Struct : Member {

        Struct(std::string name = "", uint32_t quantity = 1) : Member(name) { quantity_v = quantity; if (!name.length()) quantity_v = 0; }

        static inline std::set<Struct*> owned;

        static Struct& create(std::string name, uint32_t quantity = 1) { return **owned.insert(new Struct(name, quantity)).first; }

        static Struct* exist(std::string name) { for (auto &s : owned) if (s->name() == name) return s; return nullptr; }

        static Struct& id(std::string name) {

            auto x = exist(name);

            if (x) return *x;

            return create(name);

        }

        static void clear() { for ( auto s : owned ) delete s;  }

        static bool destroy(std::string name) {

            Struct* found = nullptr;

            for (auto &s : owned) if (s->name() == name) { found = s; break; }

            if (found) { owned.erase(found); delete found; return true; }

            return false;

        }

        Struct& add(Struct& s) { return (Struct&)Member::add(&s); }

        Struct& add(const char* name) {

            for (auto s : owned) if (!strcmp(name,s->name().c_str())) return add((*s));

            PLOGW << " noadd" << name; return *this;

        }

        Struct& remove(Struct& s) { Member::remove(&s); return *this; }

        template <typename T>
        Struct& add(std::string name = "") { return (Struct&)Member::add(new Data<T>(name)); }

        Struct& range(float from, float to, float def) {

            auto a = members.back();
            if (typeid(*a) == typeid(Data<float>)) {
                ((Data<float>*)members.back())->range_from = from;
                ((Data<float>*)members.back())->range_to = to;
                // PLOGD << " ----- is : float";
            }

            if (typeid(*a) == typeid(Data<uint32_t>)) {
                ((Data<uint32_t>*)members.back())->range_from = (uint32_t) from;
                ((Data<uint32_t>*)members.back())->range_to = (uint32_t) to;
                // PLOGD << " ----- is : uint32_t";
            }

            return *this;

        }

        uint32_t size() override {

            if (members.size() == 1 && members[0]->typed()) { return members[0]->size(); }

            return size_v;

        }

        uint32_t footprint() override {

            if (striding()) return nextFactor2(size_v,16);

            return size_v;

        }
        std::type_index type() override { if (typed()) { return members[0]->type(); } return typeid(Struct); }

        bool owns(Member& m) override {

            for (auto &s : members) if (s == &m) return true;

            return false;

        }

        void update() override {

            size_v = 0;

            for (auto &m : members) size_v += m->footprint_all();

            Member::update();

         }

        Member* copy(Member* x = nullptr) override {

            if (!x) x = new Struct(name_v);

            return Member::copy(x);

        }


         void each(std::function<void(Member& m, int offset, int depth)> cb, int offset, int depth, std::function<void(Member&)> after_cb) override {

            for (int i = 0 ; i < quantity(); i++) {

                cb(*this, offset,depth);

                int size = 0;

                if ((members.size() > 1 && members[0]->name().length()) ||  striding()) {

                    for (auto &m :members) {

                        m->each(cb, offset+size, depth+1, after_cb);

                        size+=m->footprint_all();

                    }

                    if (i!=quantity()-1) offset+=footprint();

                }

                if (after_cb) after_cb(*this);

            }

        }

    };
