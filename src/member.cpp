#include "member.hpp"

Member::~Member() { 
        
            // remove from any Members in the pool
            for (auto m : pool) m->each([this](Member& m_) { m_.remove(this);  });
            
            // remove from pool
            pool.erase(this); 

            // delete typed() a.k.a Data members
            for (auto x : members) if (x->typed()) delete x;
            
            PLOGV << "~" << name(); 
            
    }


    Member& Member::add(Member* s) {

        PLOGV << "add " << s->name() << " to " << name();

        members.push_back(s);

        size_v += members.back()->footprint_all();

        update();

        return *this;

    } 