/*
  includable-lexer.cc -- implement Includable_lexer

  source file of the LilyPond music typesetter

  (c) 1997 Han-Wen Nienhuys <hanwen@stack.nl>
*/
#include "includable-lexer.hh"
#include "source-file.hh"
#include "source.hh"

Includable_lexer::Includable_lexer()
{
    yy_current_buffer = 0;
}

/** set the  new input to s, remember old file.
*/
void
Includable_lexer::new_input(String s, Sources  * global_sources)
{
    Source_file * sl = global_sources->get_file_l(s);
    if (!sl) {
	LexerError("can't find file");
	return; 
    }
    char_count_stack_.push(0);
    if (yy_current_buffer) 
	state_stack_.push(yy_current_buffer);
    cout << "[" << s<<flush;
    include_stack_.push(sl);    
    
    // ugh. We'd want to create a buffer from the bytes directly.
    yy_switch_to_buffer(yy_create_buffer( sl->istream_l(), sl->length_off() )); 
}

/** pop the inputstack.  conceptually this is a destructor, but it
  does not destruct the Source_file it creates.  */
bool
Includable_lexer::close_input()
{
    include_stack_.pop();
    char_count_stack_.pop();
    cout << "]"<<flush;
    yy_delete_buffer(yy_current_buffer );
    if (state_stack_.empty()) {
	yy_current_buffer = 0;
	return false;
    }else {
	yy_switch_to_buffer(state_stack_.pop());
	return true;
    }    
}

char const*
Includable_lexer::here_ch_C()
{
    if (include_stack_.empty())
	return 0;
    return include_stack_.top()->ch_C() + char_count_stack_.top();
}

Includable_lexer::~Includable_lexer()
{
    while (!include_stack_.empty()) {
	close_input();
    };
}
/** 
  Since we don't create the buffer state from the bytes directly, we
  don't know about the location of the lexer. Add this as a
  YY_USER_ACTION */
void
Includable_lexer::add_lexed_char(int count)
{
    char_count_stack_.top() += count;
}
