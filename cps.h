#ifndef CPS_H_
#define CPS_H_

#include "cps_api_object_attr.h"
#include "cps_api_operation.h"
#include "cps_class_map.h"

#include <string>

namespace cps
{

class Object
{
private:
    cps_api_object_t   _obj;

public:
    Object() : _obj(cps_api_object_create()) { }

    Object(cps_api_object_t obj) : _obj(cps_api_object_create())
    {
        cps_api_object_clone(_obj, obj);
    }

    ~Object()
    {
        if (_obj != NULL) cps_api_object_delete(_obj);
        _obj = NULL;
    }

    // *************************************************************************
    // Getters
    cps_api_object_t instance()
    {
        return _obj;
    }

    void * array()
    {
        return cps_api_object_array(_obj);
    }

    size_t array_len() const
    {
        return cps_api_object_to_array_len(_obj);
    }

    virtual void attr_get(cps_api_attr_id_t attr_id, uint32_t & u32) const
    {
        cps_api_object_attr_t attr = cps_api_object_attr_get(_obj, attr_id);
        u32 = cps_api_object_attr_data_u32(attr);
    }

    virtual void attr_get(cps_api_attr_id_t attr_id, std::string & s) const
    {
        cps_api_object_attr_t attr = cps_api_object_attr_get(_obj, attr_id);
        s.assign((const char *)cps_api_object_attr_data_bin(attr), cps_api_object_attr_len(attr));
    }

    // *************************************************************************
    // Setters
    virtual bool attr_set(cps_api_attr_id_t attr_id, uint32_t u32)
    {
        cps_api_object_attr_delete(_obj, attr_id);
        return cps_api_object_attr_add_u32(_obj, attr_id, u32);
    }

    virtual bool attr_set(cps_api_attr_id_t attr_id, uint64_t u64)
    {
        cps_api_object_attr_delete(_obj, attr_id);
        return cps_api_object_attr_add_u64(_obj, attr_id, u64);
    }

    virtual bool attr_set(cps_api_attr_id_t attr_id, const std::string &str)
    {
        cps_api_object_attr_delete(_obj, attr_id);
        return cps_api_object_attr_add(_obj, attr_id, str.c_str(), str.length());
    }

protected:
    bool key_from_attr_with_qual(cps_api_attr_id_t id, cps_api_qualifier_t cat)
    {
        return cps_api_key_from_attr_with_qual(cps_api_object_key(_obj), id, cat);
    }

    bool init_from_array(const void * data, size_t len)
    {
        return cps_api_array_to_object(data, len, _obj);
    }
};

}  // namespace cps


#endif  // CPS_H_

