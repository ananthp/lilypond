/*
  my-lily-parser.hh -- declare 

  source file of the LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/


#ifndef MY_LILY_PARSER_HH
#define MY_LILY_PARSER_HH
#include "proto.hh"
#include "duration.hh"
#include "string.hh"
#include "varray.hh"
#include "lily-proto.hh"
#include "proto.hh"
#include "duration.hh"
#include "string.hh"
#include "varray.hh"

class My_lily_parser {
public:
    friend int yyparse( void*);
   
    int default_octave_i_;
    Duration default_duration_;
    String textstyle_str_;
    
    bool last_duration_mode ;

    Array<Request*> pre_reqs, post_reqs;
    Array<const char *> define_spot_array_;
    char const* defined_ch_C_;
    int fatal_error_i_;
    Sources * source_l_;
    int error_level_i_;
    bool init_parse_b_;
    My_lily_lexer * lexer_p_;
 
    Moment plet_mom();
    void add_requests( Voice_element*v);

    Voice_element * get_note_element(Note_req * ,Duration *);
    Voice_element* get_rest_element(String,Duration *);
    Voice_element* get_word_element(Text_def*, Duration*);
    void set_last_duration(Duration const *);
    void set_duration_mode(String s);
    char const* here_ch_C()const;
    void remember_spot();
    Paper_def*default_paper();
    void do_yyparse();
    void parser_error(String);

    Request* get_parens_request(char c);
    
    void set_debug();
    void set_yydebug(bool);
    void print_declarations();
public:
    void parse_file ( String init_str, String file_str);
    My_lily_parser(Sources * sources_l);
};

#endif // MY_LILY_PARSER_HH
