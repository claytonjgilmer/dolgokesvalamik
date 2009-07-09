#include "jsonparser.h"

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
	json_parser_t(json_map* map,const char* text);
	void get_next_token();
	void proc_json();
	void proc_json2();
	void proc_object();
	void proc_object2();
	void proc_array();
	void proc_array2();
	void proc_pairseq();
	void proc_pair();
	void proc_valueseq();
	void proc_value();

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
	json_map* map;
	int text_size;
	int act_index;

	token_type act_token;
	bool bool_val;
	string str_val;
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

json_parser_t::json_parser_t(json_map* map,const char* text):
map(map),
text(text)
{
	text_size=strlen(text);
	act_index=0;

	get_next_token();

	if (act_token!=tok_eof)
	{
		proc_json();
	}
}

void json_parser_t::proc_json()
{
	switch (act_token)
	{
	case tok_eof:
		return;
	case tok_left_brace:
		accept(tok_left_brace);
		proc_object();
		expect(tok_right_brace);
		break;
	case tok_left_bracket:
		proc_array();
	default:
		assertion(0,"unexpected token");
	}

	proc_json2();
}

void json_parser_t::proc_json2()
{
	switch (act_token)
	{
	case tok_eof:
		return;
	case tok_comma:
		accept(tok_comma);
		proc_json();
		return;
	default:
		assertion(0,"unexpected token");
	}
}

void json_parser_t::proc_object()
{
	map->push_back(json_item_t(new json_object_t));
	proc_pair();
	proc_object2();
}

void json_parser_t::proc_object2()
{
	switch (act_token)
	{
	case tok_comma:
		accept(tok_comma);
		proc_object();
		return;
	case tok_right_brace:
		return;
	default:
		assertion(0,"unexpected token");
	}
}

void json_parser_t::proc_pair()
{
	json_item_t& last_item(map->back());
	assertion(last_item->is_object());
	last_item.obj->pair_list.push_back(json_pair_t());
	json_pair_t& new_pair=last_item.obj->pair_list.back();
	if (act_token==tok_str)
		new_pair.key=str_val;

	expect(tok_str);
	expect(tok_colon);
	proc_value();
}

void json_parser_t::proc_value()
{
	switch (act_token)
	{
	case tok_str:
		accept(tok_str);
		break;
	case tok_number:
		accept(tok_number);
		break;
	case tok_bool:
		accept(tok_bool);
		break;
	case tok_nil:
		accept(tok_nil);
		break;
	case tok_left_brace:
		accept(tok_left_brace);
		proc_object();
		expect(tok_right_brace);
		break;
	case tok_left_bracket:
		accept(tok_left_bracket);
		proc_array();
		expect(tok_right_bracket);
		break;

	default:
		assertion(0,"unexpected token");
	}
}

void json_parser_t::proc_array()
{
	map->push_back(json_item_t(new json_array_t));
	proc_value();
	proc_array2();
}

void json_parser_t::proc_array2()
{
	switch (act_token)
	{
	case tok_comma:
		accept(tok_comma);
		proc_array();
		return;
	case tok_right_bracket:
		return;
	default:
		assertion(0,"unexpected token");
	}
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

	str_val=tmp;
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






void generate_json_map(json_map& map, const string& file)
{
	json_parser_t j(file.c_str());
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


