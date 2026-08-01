#define REINTERPRET(new_name, cast_me, new_type) \
   new_type new_name = (new_type)cast_me

#define REINTERPRET_CAST(new_type, cast_me) \
   (new_type) cast_me
