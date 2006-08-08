/*
  translator.hh -- declare Translator

  source file of the GNU LilyPond music typesetter

  (c) 1997--2006 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#ifndef TRANSLATOR_HH
#define TRANSLATOR_HH

#include "global-ctor.hh"
#include "std-string.hh"
#include "lily-proto.hh"
#include "virtual-methods.hh"
#include "input.hh"
#include "smobs.hh"
#include "std-vector.hh"
#include "protected-scm.hh"

struct Acknowledge_information
{
  Protected_scm symbol_;
  Engraver_void_function_engraver_grob_info function_;
};


/*
  Each translator class has a static list of listener records. Each
  record makes one explains how to register one of the class's stream event
  listeners to a context.
*/
typedef struct translator_listener_record {
  Listener (*get_listener_) (void *);
  SCM event_class_;
  struct translator_listener_record *next_;
} translator_listener_record;

#define TRANSLATOR_DECLARATIONS(NAME)					\
private:								\
  static translator_listener_record *listener_list_;			\
  public:								\
  NAME ();								\
  VIRTUAL_COPY_CONSTRUCTOR (Translator, NAME);				\
  static SCM static_description_;					\
  static Drul_array<vector<Acknowledge_information> > acknowledge_static_array_drul_; \
  virtual void fetch_precomputable_methods (Translator_void_method_ptr methods[]); \
  virtual SCM static_translator_description () const;			\
  virtual SCM translator_description () const;				\
  virtual Engraver_void_function_engraver_grob_info get_acknowledger (SCM sym) \
  {									\
    return static_get_acknowledger (sym);				\
  }									\
  virtual Engraver_void_function_engraver_grob_info get_end_acknowledger (SCM sym) \
  {									\
    return static_get_end_acknowledger (sym);				\
  } \
  static Engraver_void_function_engraver_grob_info static_get_acknowledger (SCM sym); \
  static Engraver_void_function_engraver_grob_info static_get_end_acknowledger(SCM); \
public:									\
  virtual translator_listener_record *get_listener_list () const	\
  {									\
    return listener_list_;						\
  }									\
  /* end #define */

#define DECLARE_TRANSLATOR_LISTENER(m)			\
public:							\
inline void listen_ ## m (Stream_event *);		\
/* Should be private */					\
static void _internal_declare_ ## m ();			\
private:						\
static Listener _get_ ## m ## _listener (void *);	\
DECLARE_LISTENER (_listen_scm_ ## m);

#define DECLARE_ACKNOWLEDGER(x) public : void acknowledge_ ## x (Grob_info); protected:
#define DECLARE_END_ACKNOWLEDGER(x) public : void acknowledge_end_ ## x (Grob_info); protected:

enum Translator_precompute_index
  {
    START_TRANSLATION_TIMESTEP,
    STOP_TRANSLATION_TIMESTEP,
    PROCESS_MUSIC,
    PROCESS_ACKNOWLEDGED,
    TRANSLATOR_METHOD_PRECOMPUTE_COUNT,
  };

/*
  Translate music into grobs.
*/
class Translator
{
  void init ();

protected:
  bool must_be_last_;

public:
  bool must_be_last () const;

  Context *context () const { return daddy_context_; }

  Translator (Translator const &);

  SCM internal_get_property (SCM symbol) const;

  virtual Output_def *get_output_def () const;
  virtual Translator_group *get_daddy_translator ()const;
  virtual Moment now_mom () const;

  virtual bool try_music (Music *req);
  virtual void initialize ();
  virtual void finalize ();

  /*should maybe be virtual*/
  void connect_to_context (Context *c);
  void disconnect_from_context (Context *c);

  void stop_translation_timestep ();
  void start_translation_timestep ();
  void process_music ();
  void process_acknowledged ();

  Context *get_score_context () const;
  Global_context *get_global_context () const;

  TRANSLATOR_DECLARATIONS (Translator);
  DECLARE_SMOBS (Translator, dummy);

protected:			// should be private.
  Context *daddy_context_;
  void protect_event (SCM ev);
  virtual void derived_mark () const;
  static void add_translator_listener (translator_listener_record **listener_list, translator_listener_record *r, Listener (*get_listener) (void *), const char *ev_class);

  friend class Translator_group;
};
void add_translator (Translator *trans);

Translator *get_translator (SCM s);
Moment get_event_length (Stream_event *s);
DECLARE_UNSMOB (Translator, translator);
#endif // TRANSLATOR_HH
