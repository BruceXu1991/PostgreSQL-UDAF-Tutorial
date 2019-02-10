drop type trans_obj CASCADE;
drop function my_float8_avg(trans_obj) CASCADE;
drop function my_float8_accum(trans_obj, float) CASCADE
drop aggregate myavg(float8);

create or replace function trans_obj_in(cstring) returns trans_obj as '/home/gpadmin/libmyavg', 'trans_obj_in' language C strict;
create or replace function trans_obj_out(trans_obj) returns cstring as '/home/gpadmin/libmyavg', 'trans_obj_out' language C strict;

CREATE TYPE trans_obj (
    INTERNALLENGTH = VARIABLE,
    INPUT = trans_obj_in,
    OUTPUT = trans_obj_out,
    STORAGE = EXTENDED
 );



create or replace FUNCTION my_float8_accum(trans_obj, float8) returns trans_obj as '/home/gpadmin/libmyavg', 'my_float8_accum' language C strict;
create or replace FUNCTION my_float8_avg(trans_obj) returns float8 as '/home/gpadmin/libmyavg', 'my_float8_avg' language C strict;


CREATE AGGREGATE myavg (float8)
(
    sfunc = my_float8_accum,
    stype = trans_obj,
    finalfunc = my_float8_avg,
    initcond = '(0,0)'
);
