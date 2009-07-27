#ifndef _jsonparser_h_
#define _jsonparser_h_

#include "containers/string.h"
#include "containers/vector.h"

enum json_value_type
{
	type_num,
	type_str,
	type_bool,
	type_nil,
	type_obj,
	type_arr
};

struct json_object_t;
struct json_array_t;

/*
json->({object)|[array])*
object-> pair (,pair)*
pair-> string : value
value -> string | number | bool | nil | {object} | [array]
array -> value (,value)*

terminatorok: { } , string number [ ]*/

struct json_value_t
{
	bool is_num() const
	{
		return val_type==type_num;
	}

	bool is_str() const
	{
		return val_type==type_str;
	}

	bool is_bool() const
	{
		return val_type==type_bool;
	}

	bool is_nil() const
	{
		return val_type==type_nil;
	}

	bool is_object() const
	{
		return val_type==type_obj;
	}

	bool is_array() const
	{
		return val_type==type_arr;
	}

	double get_num() const
	{
		return num;
	}

	const char* get_str() const
	{
		return str;
	}

	bool get_bool() const
	{
		return b;
	}

	const json_object_t* get_object() const
	{
		return obj;
	}

	const json_array_t* get_array() const
	{
		return arr;
	}

	void set_num(double num)
	{
		val_type=type_num;
		this->num=num;
	}

	void set_str(const char* str)
	{
		this->str=str;
		val_type=type_str;
	}

	void set_bool(bool b)
	{
		val_type=type_bool;
		this->b=b;
	}

	void set_object(json_object_t* obj)
	{
		val_type=type_obj;
		this->obj=obj;
	}

	void set_array(json_array_t* arr)
	{
		val_type=type_arr;
		this->arr=arr;
	}

	void set_nil()
	{
		val_type=type_nil;
	}

	void free();



	json_value_type val_type;

	union
	{
		const char* str;
		double num;
		bool b;
		json_object_t* obj;
		json_array_t* arr;
	};
};

struct json_pair_t
{
	void free()
	{
		delete key;
		val.free();
	}
	char* key;
	json_value_t val;
};

struct json_object_t
{
	void free()
	{
		for (uint32 n=0; n<pair_list.size(); ++n)
			pair_list[n].free();
	}

	json_value_t* get_value(const char* name)
	{
		for (unsigned int n=0; n<pair_list.size(); ++n)
			if (_stricmp(name,pair_list[n].key)==0)
				return &pair_list[n].val;

		return NULL;
	}

	vector<json_pair_t> pair_list;
};

struct json_array_t
{
	void free()
	{
		for (uint32 n=0; n<value_list.size(); ++n)
			value_list[n].free();
	}
	vector<json_value_t> value_list;
};


struct json_item_t
{
	bool is_object() const
	{
		return object;
	}
	bool is_array() const
	{
		return !object;
	}

	json_array_t* get_array() const
	{
		return arr;
	}

	json_object_t* get_object() const
	{
		return obj;
	}

	void free()
	{
		if (object)
		{
			obj->free();
			delete obj;
		}
		else
		{
			arr->free();
			delete arr;
		}
	}


	const bool object;

	json_item_t(json_object_t* obj):
	obj(obj),
	object(true)
	{
	}

	json_item_t(json_array_t* arr):
	arr(arr),
	object(false)
	{
	}

	union
	{
		json_object_t* obj;
		json_array_t* arr;
	};
};

struct json_map_t
{
	vector<json_item_t> item_array;


	~json_map_t()
	{
		for (uint32 n=0; n<item_array.size(); ++n)
			item_array[n].free();
	}
};




json_map_t* generate_json_map(const char* file);


#endif//_jsonparser_h_