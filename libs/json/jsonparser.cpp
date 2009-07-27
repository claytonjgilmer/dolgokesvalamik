#include "jsonparser.h"


void json_value_t::free()
{
	switch (val_type)
	{
	case type_str:
		delete str;
		break;
	case type_obj:
		obj->free();
		delete obj;
		break;
	case type_arr:
		arr->free();
		delete arr;
		break;
	}
}


enum token_type
{
	tok_str,
	tok_number,
	tok_comma,
	tok_colon,
	tok_left_bracket,
	tok_right_bracket,
	tok_left_brace,
	tok_right_brace,
	tok_bool,
	tok_nil,
	tok_eof
};



struct json_parser_t
{
	json_parser_t(const char* text);




	json_map_t* proc_json();
	json_object_t* proc_object();
	json_array_t* proc_array();
	json_pair_t proc_pair();
	json_value_t proc_value();



	void get_next_token();

	void get_number();
	void get_string();
	void get_left_bracket();
	void get_right_bracket();
	void get_left_brace();
	void get_right_brace();
	void get_comma();
	void get_colon();
	void get_true_val();
	void get_false_val();
	void get_nil_val();
	void get_eof();

	bool accept(token_type s);
	void expect(token_type s);

	const char* text;
	json_map_t* map;
	int text_size;
	int act_index;

	token_type act_token;
	bool bool_val;
	char* str_val;
	double number_val;
};

bool json_parser_t::accept(token_type s)
{
	if (act_token==s)
	{
		get_next_token();
		return 1;
	}
	else
	{
		return 0;
	}
}



void json_parser_t::expect(token_type s)
{
	if (accept(s))
		return;

	assertion(0,"unexpected token");
}

json_parser_t::json_parser_t(const char* text):
text(text)
{
	text_size=strlen(text);
	act_index=0;

	get_next_token();

	map=proc_json();
}

json_map_t* json_parser_t::proc_json()
{
	json_map_t* map=new json_map_t;

	while (act_token!=tok_eof)
	{
		switch (act_token)
		{
		case tok_left_brace:
			{
			accept(tok_left_brace);
			json_object_t* obj=proc_object();
			map->item_array.push_back(json_item_t(obj));
			expect(tok_right_brace);
			break;
			}
		case tok_left_bracket:
			{
			accept(tok_left_bracket);
			json_array_t* arr=proc_array();
			map->item_array.push_back(json_item_t(arr));
			expect(tok_right_bracket);
			break;
			}
		default:
			assertion(0,"unexpected token");
		}

		if (act_token!=tok_eof)
			expect(tok_comma);
	}

	return map;
}

json_object_t* json_parser_t::proc_object()
{
	json_object_t* obj=new json_object_t;

	while (1)
	{
		json_pair_t pair=proc_pair();
		obj->pair_list.push_back(pair);

		if (act_token==tok_comma)
			accept(tok_comma);
		else
			break;
	}

	return obj;
}

json_pair_t json_parser_t::proc_pair()
{
	json_pair_t pair;

	if (act_token==tok_str)
		pair.key=str_val;

	expect(tok_str);
	expect(tok_colon);
	pair.val=proc_value();

	return pair;
}

json_value_t json_parser_t::proc_value()
{
	json_value_t val;
	switch (act_token)
	{
	case tok_str:
		val.set_str(str_val);
		accept(tok_str);
		break;
	case tok_number:
		val.set_num(number_val);
		accept(tok_number);
		break;
	case tok_bool:
		val.set_bool(bool_val);
		accept(tok_bool);
		break;
	case tok_nil:
		val.set_nil();
		accept(tok_nil);
		break;
	case tok_left_brace:
		accept(tok_left_brace);
		val.set_object(proc_object());
		expect(tok_right_brace);
		break;
	case tok_left_bracket:
		accept(tok_left_bracket);
		val.set_array(proc_array());
		expect(tok_right_bracket);
		break;

	default:
		assertion(0,"unexpected token");
	}

	return val;
}

json_array_t* json_parser_t::proc_array()
{
	json_array_t* arr=new json_array_t;

	while (1)
	{
		json_value_t val=proc_value();
		arr->value_list.push_back(val);

		if (act_token==tok_comma)
			accept(tok_comma);
		else
			break;
	}

	return arr;
}





bool is_white_space(char c)
{
	return c==' ' || c=='\t' || c=='\n' || c=='\r';
}

bool is_number_prefix(const char c)
{
	return (c>='0' && c<='9') || c=='+' || c=='-';
}

bool is_number_character(const char c)
{
	return (c>='0' && c<='9') || c=='+' || c=='-'|| c=='.' || c=='e' || c=='E';
}

void json_parser_t::get_next_token()
{
	while (act_index<text_size && is_white_space(text[act_index]))
		++act_index;

	if (is_number_prefix(text[act_index]))
		get_number();
	else if (text[act_index]=='\"')
		get_string();
	else if (text[act_index]=='[')
		get_left_bracket();
	else if (text[act_index]==']')
		get_right_bracket();
	else if (text[act_index]=='{')
		get_left_brace();
	else if (text[act_index]=='}')
		get_right_brace();
	else if (text[act_index]==',')
		get_comma();
	else if (text[act_index]==':')
		get_colon();
	else if (text[act_index]=='t')
		get_true_val();
	else if (text[act_index]=='f')
		get_false_val();
	else if (text[act_index]=='n')
		get_nil_val();
	else if (text[act_index]==0)
		get_eof();
	else
		assertion(0,"unexpected token");
}

void json_parser_t::get_number()
{
	char tmp[128];
	int i=0;

	while (is_number_character(text[act_index]))
		tmp[i++]=text[act_index++];

	tmp[i]=0;

	float fp;
	int ret=sscanf(tmp,"%f",&fp);
	number_val=fp;

#ifdef NEED_ASSERT
	if (!ret)
	{
		char msg[128];
		sprintf (msg,"%s nem szam",tmp);
		assertion(ret,msg);
	}
#endif

	act_token=tok_number;
}

void json_parser_t::get_string()
{
	++act_index;
	char tmp[128];
	int i=0;

	while (text[act_index]!='\"')
		tmp[i++]=text[act_index++];
	tmp[i]=0;
	++act_index;

	str_val=new char[i+1]; strcpy(str_val,tmp);
	act_token=tok_str;
}

void json_parser_t::get_comma()
{
	act_token=tok_comma;
	++act_index;
}

void json_parser_t::get_colon()
{
	act_token=tok_colon;
	++act_index;
}

void json_parser_t::get_left_bracket()
{
	act_token=tok_left_bracket;
	++act_index;
}

void json_parser_t::get_right_bracket()
{
	act_token=tok_right_bracket;
	++act_index;
}

void json_parser_t::get_left_brace()
{
	act_token=tok_left_brace;
	++act_index;
}

void json_parser_t::get_right_brace()
{
	act_token=tok_right_brace;
	++act_index;
}

void json_parser_t::get_nil_val()
{
	if (!strncmp(text+act_index,"null",4))
	{
		act_token=tok_nil;
		act_index+=4;
	}
	else
	{
		assertion(0);
	}
}


void json_parser_t::get_false_val()
{
	if (!strncmp(text+act_index,"false",5))
	{
		act_token=tok_bool;
		bool_val=false;
		act_index+=5;
	}
	else
	{
		assertion(0);
	}
}

void json_parser_t::get_true_val()
{
	if (!strncmp(text+act_index,"true",4))
	{
		act_token=tok_bool;
		bool_val=true;
		act_index+=4;
	}
	else
	{
		assertion(0);
	}
}

void json_parser_t::get_eof()
{
	act_token=tok_eof;
}






json_map_t* generate_json_map(const char* file)
{
	json_parser_t j(file);
	return j.map;
}


#if 0
char * StrStart = mBufPtr;
char * StrEnd;

while (true)
{
CheckUnexpectedEof();

if (*mBufPtr=='+' || *mBufPtr=='-' || *mBufPtr=='.' || *mBufPtr=='e' || *mBufPtr=='E'
|| (*mBufPtr>='0' && *mBufPtr<='9'))
{
++mBufPtr;
}
else
{
StrEnd = mBufPtr;
break;
}

char Str[64];
char ExtraStr[64];
if (StrEnd-StrStart >= sizeof(Str)-1)
{
Error("number is too long (>=%d)", sizeof(Str)-1);
fread
}

strncpy(Str, StrStart, StrEnd-StrStart);
Str[StrEnd-StrStart] = '\0';
int IntScanfed = sscanf(Str,"%d%s", &Number.mIntValue, ExtraStr);
int FloatScanfed = sscanf(Str,"%e%s", &Number.mFloatValue, ExtraStr);

if (IntScanfed!=1 && FloatScanfed!=1)
{
Error("can't scanf number as int or float");
}
#endif


