
#include <algorithm>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>
#include <memory>
#include <set>

#include <boost/type_index.hpp>
#include <map>

using TypeIndex = boost::typeindex::type_index;

void set(TypeIndex id, char* ptr, float val) { 

    if (id == typeid(float)) 
        (float&)*ptr = val;

    else if (id == typeid(double))
        (double&)*ptr =  val;

    else if (id == typeid(uint8_t))
        (uint8_t&)*ptr =  val;

    else if (id == typeid(uint16_t))
        (uint16_t&)*ptr =  val;

    else if (id == typeid(uint32_t)) 
        (uint32_t&)*ptr = val;

    else if (id == typeid(int8_t))
        (int8_t&)*ptr =  val;

    else if (id == typeid(int16_t))
        (int16_t&)*ptr =  val;

    else if (id == typeid(int32_t)) 
        (int32_t&)*ptr = val;

    else if (id == typeid(std::string))
        (std::string&)*ptr =  std::to_string(val);

    else if (id == typeid(const char*))
        (const char*&)*ptr =  std::to_string(val).c_str();

}

struct Definition;

struct Member : std::enable_shared_from_this<Member>  {

    std::string name;

    std::set<std::shared_ptr<Member>> observers;

    TypeIndex type_v = typeid(Member);

    std::shared_ptr<Member> clone_v = nullptr;

    struct Definition : std::enable_shared_from_this<Definition>{
        
        std::shared_ptr<Member> type_v;
        std::string label;
        int quantity_v;
    
        std::shared_ptr<Definition> clone_v = nullptr;
        
        Definition( std::shared_ptr<Member> type, const char* name = nullptr, int q = 1, float from = 0, float to = 0, float def = 0) : label(name?name:type->name), quantity_v(q) {

            this->type(type);

            if (from || to || def)
                range(from, to, def); 
        }

        std::vector<char> rangedef; 
        std::vector<char> temprangedef; 

        uint32_t footprint_all() { return type_v->footprint() *  quantity_v ; }

        void type(std::shared_ptr<Member> type) {

            type_v = type;
            

        }

        bool quantity(int q);

        void range(float from, float to, float def) {

            if (!type_v->size()) 
                return;

            rangedef.resize(type_v->size()*3);
            memset(&rangedef[0],0,rangedef.size());

            set(type_v->type_v, this->from(), from);
            set(type_v->type_v, this->to(), to);
            set(type_v->type_v, this->def(), def);
   
        }

        char* from() { return rangedef.size()?rangedef.data():nullptr; }

        char* to() { 
            
            if (rangedef.size())
                return rangedef.data()+(type_v->size());
            
            static uint64_t ui64 = 0xffffffffffffffffu;
            static int64_t i64   = 0xffffffffffffffff;
            static double f32    = 0xfffffffffffff;

            if (type_v->type_v == typeid(float)) return (char*)&f32;
            if (type_v->type_v == typeid(double)) return (char*)&f32;
            if (type_v->type_v == typeid(uint64_t)) return (char*)&ui64;
            if (type_v->type_v == typeid(uint32_t)) return (char*)&ui64;            
            if (type_v->type_v == typeid(uint16_t)) return (char*)&ui64;
            if (type_v->type_v == typeid(uint8_t)) return (char*)&ui64;
            if (type_v->type_v == typeid(int64_t)) return (char*)&i64;
            if (type_v->type_v == typeid(int32_t)) return (char*)&i64;            
            if (type_v->type_v == typeid(int16_t)) return (char*)&i64;
            if (type_v->type_v == typeid(int8_t)) return (char*)&i64;

            return nullptr;

        }

        char* def() { 

            if (rangedef.size()) return rangedef.data()+(type_v->size()*2);
            
            if (type_v->type_v == typeid(Member)) {

                if (!type_v->size())
                    return nullptr;

                temprangedef.resize(type_v->size());

                int offset = 0;

                for (auto def : type_v->members) 

                    for (int i = 0; i < def->quantity_v; i++) {

                        memcpy(temprangedef.data()+offset, def->def(), def->type_v->size());    

                        offset += def->type_v->size();

                }

                return temprangedef.data();

            }

            return nullptr; 
            
        }

    };

    std::vector<std::shared_ptr<Definition>> members;
    
    std::shared_ptr<Definition> moving_v = nullptr;

    virtual ~Member() {
    
        if (!clone_v)
            for (auto def : members) 
                def->type_v->observers.erase(shared_from_this());

    }

    virtual int size() {
        int size = 0;
        for (auto def : members) 
            size+= def->footprint_all();
        return size;
    }
        
    uint32_t stride() { return 0 ; }
    uint32_t footprint() { return size() + stride() ; }
  
    virtual void pre(std::shared_ptr<Member> changing) {}
    virtual void post(int diff,int compoffset, int compsize, char* def, int q) {}

    std::set<std::shared_ptr<Member>> observe() {

        std::set<std::shared_ptr<Member>> out;

        if (!observers.size()) 
            out.insert(shared_from_this());
        
        for (auto observer : observers)     
            for (auto x :   observer->observe())
                out.insert(x);

        return out;

    }
};

struct Data : Member {
    
    static inline std::map<TypeIndex, int> type_sizes;

    Data(const TypeIndex& type, const char* name)  { 

        type_v = type; 

        this->name = name ? name : type_v.pretty_name();

    }

    int size() override {
        return type_sizes[type_v];
    }

};

template <typename T>
struct TYPE : TypeIndex { 
    
    TYPE() : TypeIndex(typeid(T)) { 
        
        if (Data::type_sizes.find(typeid(T)) == Data::type_sizes.end())
            Data::type_sizes[typeid(T)] = sizeof(T);

        // if (std::is_arithmetic<T>::value) Data::tofloat[typeid(T)] = [&](void* ptr){ return (float)(*(T*)ptr); }; // this wont compile :'( plz hlp

    }
};

struct Struct : Member {

    std::shared_ptr<Definition> add(std::shared_ptr<Member> type, int quantity = 1, const char* name = nullptr, float from = 0, float to = 0, float def = 0) {

        type->observers.insert(shared_from_this());

        auto observers = observe();
        
        for (auto x :  observers)
            x->pre(shared_from_this());

        int compoffset = footprint(); // or pos

        auto definition = std::make_shared<Definition>(type, name, quantity, from, to, def);

        int compsize = definition->type_v->footprint();

        auto defval = definition->def();

        std::string cout = this->name+ "[" +std::to_string(footprint()) + "] add  " + definition->label + "(" + definition->type_v->name + ":" + std::to_string(compsize);
        if (definition->quantity_v > 1)
            cout += "*" + std::to_string(definition->quantity_v)  + ":" + std::to_string(definition->footprint_all());
        cout += ")";
        std::cout << cout << std::endl;

        members.emplace_back(definition);

        for (auto x :   observers)
            x->post(definition->quantity_v,compoffset,compsize, defval, definition->quantity_v);

        return definition;

    }

    bool quantity(std::shared_ptr<Definition> definition, int q) {

        auto observers = observe();

        for (auto x :  observers)
            x->pre(shared_from_this());

        int compoffset = footprint(); // or pos

        int compsize = definition->type_v->footprint(); // or before definition->quantity_v = q; ?

        auto defval = definition->def();

        std::string cout = this->name+ " resize " + definition->label + " from " + std::to_string(definition->quantity_v) + " to " + std::to_string(q) ;
        cout += "";
        std::cout << cout << std::endl;
        
        int diff = q-definition->quantity_v;

        definition->quantity_v = q;

        for (auto x :   observers)
            x->post(diff,compoffset,compsize, defval, definition->quantity_v);

        return true;

    }

    std::shared_ptr<Definition> add(const TypeIndex& type, int quantity, const char* name, float from, float to, float def);

    template <typename T, int q = 1>
    std::shared_ptr<Definition> add(const char* name, float from = 0, float to = 0, float def = 0) {

        return add(TYPE<T>(), q, name, from, to, def);
        
    }
    
  

};

struct Register {

    std::set<std::shared_ptr<Data>> datatypes;
    std::set<std::shared_ptr<Struct>> structtypes;

    template <typename T>
    std::shared_ptr<Data> create(const char* name) {

        return create(TYPE<T>(), name);
        
    }

    std::shared_ptr<Data> create(const TypeIndex& type, const char* name = nullptr) {

        auto d = std::make_shared<Data>(type, name);

        // std::cout << "create " << d->quantity_v << " " << d->name << (d->quantity_v>1?"s":"")<<  " " << Type(type).name() << " - " << Type(type).size() << "\n";

        datatypes.insert(d);

        return d;
        
    }

    template <typename T>
    std::shared_ptr<Data> create() {

        return create<T>("");
        
    }

    std::shared_ptr<Struct> create(const char* name) {

        auto s = std::make_shared<Struct>();

        s->name = name;

        // std::cout << "create " << s->quantity_v << " " << s->name << (s->quantity_v>1?"s":"")<<  ""  << "\n";

        structtypes.insert(s);

        return s;
        
    }
 
    std::shared_ptr<Member> find(const TypeIndex& type) {

        // type.type_info().name()
        for (const auto& x : datatypes) 
            if (x.get()->type_v == type) 
                return x;

        return create(type);
        
    }
 
    template <typename T>
    std::shared_ptr<Member> find() {
        return find(TYPE<T>());
    }

    std::shared_ptr<Member> changing = nullptr;

};

static std::string str(std::vector<std::shared_ptr<Member>>& stl) {

    std::string out;

    for (auto x : stl) {
        
        out +=  x->name;

        if (x != stl.back()) 
            out +=  "::";

    }

    return out;

}

struct Buffer : Struct {

    Buffer(std::string name = "") { 
        this->name = name;
    }

    std::vector<char> data = {};

    std::vector<uint32_t> changing_offsets;

    void find(std::shared_ptr<Member> x, std::vector<std::shared_ptr<Member>> stl , int offset= 0) {

        // std::cout <<  str(stl) << " " << offset << "\n";

        auto m = stl.back();
        stl.resize(stl.size()+1);

        for (auto def : m->members) {

            stl.back() = def->type_v;
            
            for (int i = 0; i < def->quantity_v; i++) {

                if (def->type_v == x) 
                    changing_offsets.emplace_back(offset);

                find(x,stl,offset);
                
                offset+=def->type_v->footprint();
            }


        }

    }


    void pre(std::shared_ptr<Member> changing) override { 
        
        // find offset of each new instance 
        
        changing_offsets.clear();

        if (changing.get() == this)
            changing_offsets = {footprint()};
        else
            find(changing,{shared_from_this()}); 
        
        std::reverse(changing_offsets.begin(), changing_offsets.end());


    }

    void post(int diff,int compoffset, int compsize, char* def, int q) override { 

        std::cout << "found "  ;
        for (auto x : changing_offsets) 
            std::cout << (unsigned int) (x+compoffset) << " , ";
        std::cout << "\n";

        auto old_cursor = data.size();
        data.resize(footprint());
        auto new_cursor = footprint();

        auto comp_fullsize = compsize * diff;

        std::cout << "old_cursor: " << old_cursor << "\n"  ;
        std::cout << "new_cursor: " << new_cursor << "\n"  ;
        std::cout << "compoffset: " << compoffset << "\n"  ;
        std::cout << "diff: " << diff << "\n"  ;
        std::cout << "compsize: " << compsize << "\n"  ;
        std::cout << "q: " << q << "\n"  ;
        std::cout << "comp_fullsize: " << comp_fullsize << "\n"  ;

        print();
        
        std::cout << "\n"  ;

        for (auto &offset : changing_offsets) {

            int segsize = old_cursor - offset-compoffset;

            auto told_cursor = old_cursor;
            
            // if (segsize) {

                old_cursor -= segsize;

                new_cursor -= segsize;

                std::cout 
                <<  "- offset "  <<  offset 
                <<  " segsize "  <<  segsize 
                <<  " from "  <<  old_cursor 
                <<  " to "  <<  told_cursor 
                <<  " @ "  <<  new_cursor  
                << std::endl;

                std::move(data.begin()+old_cursor, data.begin()+told_cursor, data.begin()+new_cursor); 

                print();

            // }

    // 1 1 2 2 3 3 3 3 3 3 1 1 2 2 3 3 3 3 3 3 // should be that

    // 1 1 2 2 3 3 1 1 2 2 3 3 0 0 0 0 0 0 0 0 

            new_cursor -=  comp_fullsize;

            std::cout 
            <<  "- default "  <<  new_cursor
            <<  " for "  <<  comp_fullsize
            << std::endl;
        
            if (def) 
                for (int i = 0; i < diff; i++) 
                    memcpy(&data[new_cursor]+i*compsize, def, compsize);

            else
                memset(&data[new_cursor], 55, comp_fullsize);

            print();

        }

     }

    void print() {

        for (auto x : data)
            std::cout << std::to_string(x) << " ";
        
        std::cout << "\n";

    }

    Buffer(Buffer& other) : Struct(other), data(other.data) { }

};

Register reg;

std::shared_ptr<Member::Definition> Struct::add(const TypeIndex& type, int quantity, const char* name, float from, float to, float def) {

    return add(reg.find(type), quantity, name, from, to, def);

}

int main() {

    // remove

    // instance ( aka Member::tracking_counter ?)
    
    auto aa = reg.create("aa");
    aa->add<uint8_t>("A", 1,1,1);
    
    auto bb = reg.create("bb");
    bb->add<uint8_t>("B", 2,2,2);
    
    auto cc = reg.create("cc");
    auto cval = cc->add<uint8_t>("C", 3,3,3);
    
    auto trick = reg.create("trick");

    auto t1 = reg.create("t1");
    t1->add<uint8_t>("B", 21,21,21);
    trick->add(t1);
    auto t2 = reg.create("t2");
    auto t21 = reg.create("t21");
    t21->add<uint8_t>("B", 121,121,121);
    t2->add(t21);
    auto t22 = reg.create("t22");
    t22->add<uint8_t>("B", 122,122,122);
    t2->add(t22);
    auto t23 = reg.create("t23");
    t23->add<uint8_t>("B", 123,123,123);
    t2->add(t23);
    trick->add(t2);
    auto t3 = reg.create("t3");
    t3->add<uint8_t>("B", 23,23,23);
    trick->add(t3);


    auto test = reg.create("test");
    test->add(aa, 2);
    test->add(trick, 2);
    test->add(bb, 2);
    test->add(cc, 2);

    auto buffer = std::make_shared<Buffer>("Buffy");
    buffer->add(test,2);

    buffer->print();

    std::cout << "############\n";



    trick->add<uint8_t,2>("T", 4,4,4);
    // buffer->print();
    cc->quantity(cval,3);
    // cc->quantity(cval,1);


    // auto dd = reg.create("dd");
    // dd->add<uint8_t,3>("D", 5,5,5);
    // trick->add(dd,2);


    buffer->print();

    std::cout << "DONE\n";


}
