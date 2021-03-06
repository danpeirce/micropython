/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "emitglue.h"

#if MICROPY_EMIT_NATIVE

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

// convert a Micro Python object to a valid native value based on type
mp_uint_t mp_convert_obj_to_native(mp_obj_t obj, mp_uint_t type) {
    DEBUG_printf("mp_convert_obj_to_native(%p, " UINT_FMT ")\n", obj, type);
    switch (type & 3) {
        case MP_NATIVE_TYPE_OBJ: return (mp_uint_t)obj;
        case MP_NATIVE_TYPE_BOOL:
        case MP_NATIVE_TYPE_INT:
        case MP_NATIVE_TYPE_UINT: return mp_obj_get_int(obj);
        default: assert(0); return 0;
    }
}

// convert a native value to a Micro Python object based on type
mp_obj_t mp_convert_native_to_obj(mp_uint_t val, mp_uint_t type) {
    DEBUG_printf("mp_convert_native_to_obj(" UINT_FMT ", " UINT_FMT ")\n", val, type);
    switch (type & 3) {
        case MP_NATIVE_TYPE_OBJ: return (mp_obj_t)val;
        case MP_NATIVE_TYPE_BOOL: return MP_BOOL(val);
        case MP_NATIVE_TYPE_INT: return mp_obj_new_int(val);
        case MP_NATIVE_TYPE_UINT: return mp_obj_new_int_from_uint(val);
        default: assert(0); return mp_const_none;
    }
}

// wrapper that accepts n_args and n_kw in one argument
// (native emitter can only pass at most 3 arguments to a function)
mp_obj_t mp_native_call_function_n_kw(mp_obj_t fun_in, mp_uint_t n_args_kw, const mp_obj_t *args) {
    return mp_call_function_n_kw(fun_in, n_args_kw & 0xff, (n_args_kw >> 8) & 0xff, args);
}

// wrapper that makes raise obj and raises it
NORETURN void mp_native_raise(mp_obj_t o) {
    nlr_raise(mp_make_raise_obj(o));
}

// these must correspond to the respective enum in runtime0.h
void *const mp_fun_table[MP_F_NUMBER_OF] = {
    mp_convert_obj_to_native,
    mp_convert_native_to_obj,
    mp_load_const_int,
    mp_load_const_dec,
    mp_load_const_str,
    mp_load_const_bytes,
    mp_load_name,
    mp_load_global,
    mp_load_build_class,
    mp_load_attr,
    mp_load_method,
    mp_store_name,
    mp_store_global,
    mp_store_attr,
    mp_obj_subscr,
    mp_obj_is_true,
    mp_unary_op,
    mp_binary_op,
    mp_obj_new_tuple,
    mp_obj_new_list,
    mp_obj_list_append,
    mp_obj_new_dict,
    mp_obj_dict_store,
#if MICROPY_PY_BUILTINS_SET
    mp_obj_new_set,
    mp_obj_set_store,
#endif
    mp_make_function_from_raw_code,
    mp_native_call_function_n_kw,
    mp_call_method_n_kw,
    mp_getiter,
    mp_iternext,
    nlr_push,
    nlr_pop,
    mp_native_raise,
    mp_import_name,
    mp_import_from,
    mp_import_all,
#if MICROPY_PY_BUILTINS_SLICE
    mp_obj_new_slice,
#endif
    mp_unpack_sequence,
    mp_unpack_ex,
    mp_delete_name,
    mp_delete_global,
};

/*
void mp_f_vector(mp_fun_kind_t fun_kind) {
    (mp_f_table[fun_kind])();
}
*/

#endif // MICROPY_EMIT_NATIVE
