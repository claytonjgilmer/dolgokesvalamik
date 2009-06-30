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

struct json_value_t
{
	bool is_num() const;
	bool is_str() const;
	bool is_bool() const;
	bool is_nil() const;
	bool is_object() const;
	bool is_array() const;

	double get_num() const;
	const string& get_str() const;
	double get_bool() const;
	const json_object_t* get_object() const;
	const json_array_t* get_array() const;


	json_value_type val_type;

	union
	{
		const char* str;
//		string str;
		double num;
		bool b;
		json_object_t* obj;
		json_array_t* arr;
	};
};

struct json_pair_t
{
	string key;
	json_value_t val;
};

struct json_object_t
{
	vector<json_pair_t> pair_list;
};

struct json_array_t
{
	vector<json_value_t> value_list;
};


struct json_item_t
{
	bool is_object() const;
	bool is_array() const;

	json_array_t* get_array() const;
	json_object_t* get_object() const;


	const bool object;

	json_item_t(json_object_t*);
	json_item_t(json_array_t*);

	union
	{
		json_object_t* const obj;
		json_array_t* const arr;
	};
};

typedef vector<json_item_t*> json_map;




void generate_json_map(json_map& map, const string& file);


#endif//_jsonparser_h_