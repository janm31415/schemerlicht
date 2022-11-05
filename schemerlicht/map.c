#include "map.h"
#include "memory.h"
#include "limits.h"
#include "error.h"
#include "context.h"

#include <string.h>

#define hashmod(m,n) (get_node(m, ((n) % ((node_size(m)-1)|1))))

#define fxnumints cast(schemerlicht_memsize, sizeof(schemerlicht_fixnum)/sizeof(schemerlicht_memsize))
#define flnumints cast(schemerlicht_memsize, sizeof(schemerlicht_flonum)/sizeof(schemerlicht_memsize))

#define schemerlicht_max_bits (schemerlicht_mem_bits - 2)
#define toobig(x) ((((x)-1) >> schemerlicht_max_bits) != 0)
#define twoto(x) ((schemerlicht_memsize)1<<(x))

static void schemerlicht_set_nil(schemerlicht_object* obj)
  {
  obj->type = schemerlicht_object_type_undefined;
  }

static void schemerlicht_create_array_vector(schemerlicht_context* ctxt, schemerlicht_map* map, schemerlicht_memsize array_size)
  {
  map->array = schemerlicht_newvector(ctxt, array_size, schemerlicht_object);
  for (schemerlicht_memsize i = map->array_size; i < array_size; ++i)
    schemerlicht_set_nil(&map->array[i]);
  map->array_size = array_size;
  }

static void schemerlicht_create_node_vector(schemerlicht_context* ctxt, schemerlicht_map* map, schemerlicht_memsize log_node_size)
  {
  schemerlicht_memsize size = twoto(log_node_size);
  if (log_node_size > schemerlicht_max_bits)
    schemerlicht_runerror(ctxt, "map overflow");
  if (log_node_size == 0)
    {
    map->node = ctxt->global->dummy_node;
    schemerlicht_assert(get_key(map->node)->type == schemerlicht_object_type_undefined);
    schemerlicht_assert(get_value(map->node)->type == schemerlicht_object_type_undefined);
    schemerlicht_assert(map->node->next == NULL);
    }
  else
    {
    map->node = schemerlicht_newvector(ctxt, size, schemerlicht_map_node);
    for (schemerlicht_memsize i = 0; i < size; ++i)
      {
      map->node[i].next = NULL;
      schemerlicht_set_nil(get_key(get_node(map, i)));
      schemerlicht_set_nil(get_value(get_node(map, i)));
      }
    }
  map->log_node_size = log_node_size;
  map->first_free = get_node(map, size - 1);
  }

static void computesizes(schemerlicht_memsize nums[], schemerlicht_memsize ntotal, schemerlicht_memsize* narray, schemerlicht_memsize* nhash)
  {
  schemerlicht_memsize i;
  schemerlicht_memsize a = nums[0];  /* number of elements smaller than 2^i */
  schemerlicht_memsize na = a;  /* number of elements to go to array part */
  schemerlicht_memsize n = (na == 0) ? schemerlicht_mem_invalid_size : 0;  /* (log of) optimal size for array part */
  for (i = 1; (a < *narray) && (*narray >= twoto(i - 1)); ++i)
    {
    if (nums[i] > 0)
      {
      a += nums[i];
      if (a >= twoto(i - 1))
        {  /* more than half elements in use? */
        n = i;
        na = a;
        }
      }
    }
  schemerlicht_assert(na <= *narray && *narray <= ntotal);
  *nhash = ntotal - na;
  *narray = (n == schemerlicht_mem_invalid_size) ? 0 : twoto(n);
  schemerlicht_assert(na <= *narray && na >= *narray / 2);
  }

static schemerlicht_memsize arrayindex(const schemerlicht_object* key)
  {
  if (key->type == schemerlicht_object_type_fixnum)
    {
    schemerlicht_memsize index = (schemerlicht_memsize)key->value.fx;
    if (cast(schemerlicht_fixnum, index) == key->value.fx && !toobig(index))
      return index;
    }
  return schemerlicht_mem_invalid_size;  /* `key' did not match some condition */
  }


static void numuse(const schemerlicht_map* t, schemerlicht_memsize* narray, schemerlicht_memsize* nhash)
  {
  schemerlicht_memsize nums[schemerlicht_max_bits + 1];
  schemerlicht_memsize i, lg;
  schemerlicht_memsize totaluse = 0;
  /* count elements in array part */
  for (i = 0, lg = 0; lg <= schemerlicht_max_bits; ++lg)
    {  /* for each slice [2^(lg-1) to 2^lg) */
    schemerlicht_memsize ttlg = twoto(lg);  /* 2^lg */
    if (ttlg > t->array_size)
      {
      ttlg = t->array_size;
      if (i >= ttlg)
        break;
      }
    nums[lg] = 0;
    for (; i < ttlg; ++i)
      {
      if (t->array[i].type != schemerlicht_object_type_undefined)
        {
        ++nums[lg];
        ++totaluse;
        }
      }
    }
  for (; lg <= schemerlicht_max_bits; ++lg)
    nums[lg] = 0;  /* reset other counts */
  *narray = totaluse;  /* all previous uses were in array part */
  /* count elements in hash part */
  i = node_size(t);
  while (i--)
    {
    schemerlicht_map_node* n = &t->node[i];
    if (get_value(n)->type != schemerlicht_object_type_undefined)
      {
      schemerlicht_memsize k = arrayindex(get_key(n));
      if (k != schemerlicht_mem_invalid_size)
        {  /* is `key' an appropriate array index? */
        ++nums[schemerlicht_log2(k - 1) + 1];  /* count as such */
        ++(*narray);
        }
      ++totaluse;
      }
    }
  computesizes(nums, totaluse, narray, nhash);
  }

static void resize(schemerlicht_context* ctxt, schemerlicht_map* t, schemerlicht_memsize nasize, int nhsize)
  {
  schemerlicht_memsize i;
  schemerlicht_memsize oldasize = t->array_size;
  schemerlicht_memsize oldhsize = t->log_node_size;
  schemerlicht_map_node* nold;
  schemerlicht_map_node temp[1];
  if (oldhsize)
    nold = t->node;  /* save old hash ... */
  else
    {  /* old hash is `dummynode' */
    schemerlicht_assert(t->node == ctxt->global->dummy_node);
    temp[0] = t->node[0];  /* copy it to `temp' */
    nold = temp;
    get_key(ctxt->global->dummy_node)->type = schemerlicht_object_type_undefined; /* restate invariant */
    get_value(ctxt->global->dummy_node)->type = schemerlicht_object_type_undefined;
    schemerlicht_assert(ctxt->global->dummy_node->next == NULL);
    }
  if (nasize > oldasize)  /* array part must grow? */
    schemerlicht_create_array_vector(ctxt, t, nasize);
  /* create new hash part with appropriate size */
  schemerlicht_create_node_vector(ctxt, t, nhsize);
  /* re-insert elements */
  if (nasize < oldasize)
    {  /* array part must shrink? */
    t->array_size = nasize;
    /* re-insert elements from vanishing slice */
    for (i = nasize; i < oldasize; ++i)
      {
      if (t->array[i].type != schemerlicht_object_type_undefined)
        {
        schemerlicht_object* obj = schemerlicht_map_insert_indexed(ctxt, t, i + 1);
        schemerlicht_set_object(obj, &t->array[i]);
        }
      }
    /* shrink array */
    schemerlicht_reallocvector(ctxt, t->array, oldasize, nasize, schemerlicht_object);
    }
  /* re-insert elements in hash part */
  for (i = twoto(oldhsize); i >= 1; --i)
    {
    schemerlicht_map_node* old = nold + i - 1;
    if (get_value(old)->type != schemerlicht_object_type_undefined)
      {
      schemerlicht_object* obj = schemerlicht_map_insert(ctxt, t, get_key(old));
      schemerlicht_set_object(obj, get_value(old));
      }
    }
  if (oldhsize)
    schemerlicht_freevector(ctxt, nold, twoto(oldhsize), schemerlicht_map_node); /* free old array */
  }


static void schemerlicht_rehash(schemerlicht_context* ctxt, schemerlicht_map* map)
  {
  schemerlicht_memsize nasize, nhsize;
  numuse(map, &nasize, &nhsize);  /* compute new sizes for array and hash parts */
  resize(ctxt, map, nasize, schemerlicht_log2(nhsize) + 1);
  }


schemerlicht_map* schemerlicht_map_new(schemerlicht_context* ctxt, schemerlicht_memsize array_size, schemerlicht_memsize log_node_size)
  {
  schemerlicht_map* m = schemerlicht_new(ctxt, schemerlicht_map);
  m->array = NULL;
  m->node = NULL;
  m->array_size = 0;
  m->log_node_size = 0;
  schemerlicht_create_array_vector(ctxt, m, array_size);
  schemerlicht_create_node_vector(ctxt, m, log_node_size);
  return m;
  }

void schemerlicht_map_keys_free(schemerlicht_context* ctxt, schemerlicht_map* map)
  {
  schemerlicht_memsize size = node_size(map);
  for (schemerlicht_memsize i = 0; i < size; ++i)
    {
    if (map->node[i].key.type == schemerlicht_object_type_string)
      schemerlicht_string_destroy(ctxt, &(map->node[i].key.value.s));
    if (map->node[i].key.type == schemerlicht_object_type_symbol)
      schemerlicht_string_destroy(ctxt, &(map->node[i].key.value.s));
    }
  }

void schemerlicht_map_values_free(schemerlicht_context* ctxt, schemerlicht_map* map)
  {
  schemerlicht_memsize size = node_size(map);
  for (schemerlicht_memsize i = 0; i < size; ++i)
    {
    if (map->node[i].value.type == schemerlicht_object_type_string)
      schemerlicht_string_destroy(ctxt, &(map->node[i].value.value.s));
    if (map->node[i].value.type == schemerlicht_object_type_symbol)
      schemerlicht_string_destroy(ctxt, &(map->node[i].value.value.s));
    }
  }

void schemerlicht_map_free(schemerlicht_context* ctxt, schemerlicht_map* map)
  {
  //schemerlicht_memsize size = node_size(map);
  //for (schemerlicht_memsize i = 0; i < size; ++i)
  //  {
  //  if (map->node[i].key.type == schemerlicht_object_type_string)
  //    schemerlicht_string_destroy(ctxt, &(map->node[i].key.value.s));
  //  if (map->node[i].key.type == schemerlicht_object_type_symbol)
  //    schemerlicht_string_destroy(ctxt, &(map->node[i].key.value.s));
  //  }
  schemerlicht_freevector(ctxt, map->node, node_size(map), schemerlicht_map_node);
  schemerlicht_freevector(ctxt, map->array, map->array_size, schemerlicht_object);
  schemerlicht_delete(ctxt, map);
  }

static schemerlicht_map_node* schemerlicht_hash_fixnum(const schemerlicht_map* m, schemerlicht_fixnum n)
  {
  schemerlicht_memsize a[fxnumints];
  n += 1;
  memcpy(a, &n, sizeof(a));
  for (int i = 0; i < fxnumints; ++i)
    a[0] += a[i];
  return hashmod(m, a[0]);
  }

static schemerlicht_map_node* schemerlicht_hash_flonum(const schemerlicht_map* m, schemerlicht_flonum n)
  {
  schemerlicht_memsize a[flnumints];
  n += 1;
  memcpy(a, &n, sizeof(a));
  for (int i = 0; i < flnumints; ++i)
    a[0] += a[i];
  return hashmod(m, a[0]);
  }

static schemerlicht_map_node* schemerlicht_hash_pointer(const schemerlicht_map* m, void* ptr)
  {
  return hashmod(m, (uintptr_t)ptr);
  }

static schemerlicht_memsize hash_string(const char* str)
  {
  int l = cast(int, strlen(str));
  schemerlicht_memsize h = cast(schemerlicht_memsize, l);
  int step = (l >> 5) + 1; // if string is too long, don't hash all its chars
  for (int i = l; i >= step; i -= step)
    h = h ^ ((h << 5) + (h >> 2) + (unsigned char)(str[i - 1]));
  return h;
  }

static schemerlicht_map_node* schemerlicht_hash_string(const schemerlicht_map* m, const char* str)
  {
  schemerlicht_memsize h = hash_string(str);
  schemerlicht_assert((node_size(m) & (node_size(m) - 1)) == 0);
  return get_node(m, h & (node_size(m) - 1));
  }

static schemerlicht_map_node* schemerlicht_main_position(const schemerlicht_map* m, const schemerlicht_object* key)
  {
  switch (key->type)
    {
    case schemerlicht_object_type_void:
    case schemerlicht_object_type_blocking:
    case schemerlicht_object_type_true:
    case schemerlicht_object_type_false:
    case schemerlicht_object_type_nil:
      return schemerlicht_hash_fixnum(m, key->type);
    case schemerlicht_object_type_primitive:
    case schemerlicht_object_type_primitive_object:
    case schemerlicht_object_type_fixnum:
      return schemerlicht_hash_fixnum(m, key->value.fx);
    case schemerlicht_object_type_char:
      return schemerlicht_hash_fixnum(m, cast(schemerlicht_fixnum, key->value.ch));
    case schemerlicht_object_type_flonum:
      return schemerlicht_hash_flonum(m, key->value.fl);
    case schemerlicht_object_type_symbol:
    case schemerlicht_object_type_string:
      return schemerlicht_hash_string(m, key->value.s.string_ptr);
    case schemerlicht_object_type_lambda:
      return schemerlicht_hash_pointer(m, key->value.ptr);
    default:
      return schemerlicht_hash_pointer(m, key->value.v.vector_ptr);
    }
  }

schemerlicht_object* schemerlicht_map_get_indexed(schemerlicht_map* map, schemerlicht_memsize index)
  {
  if (index < map->array_size)
    return &map->array[index];
  else
    {
    schemerlicht_fixnum nr = cast(schemerlicht_fixnum, index);
    schemerlicht_map_node* n = schemerlicht_hash_fixnum(map, nr);
    do
      {
      if (get_key(n)->type == schemerlicht_object_type_fixnum && get_key(n)->value.fx == nr)
        {
        return get_value(n);
        }
      else
        n = n->next;
      } while (n);
      return NULL;
    }
  }

static schemerlicht_object* schemerlicht_map_get_any(schemerlicht_context* ctxt, schemerlicht_map* map, const schemerlicht_object* key)
  {
  if (key->type == schemerlicht_object_type_undefined)
    return NULL;
  else
    {
    schemerlicht_map_node* n = schemerlicht_main_position(map, key);
    do
      {
      if (schemerlicht_objects_equal(ctxt, get_key(n), key))
        return get_value(n);
      else
        n = n->next;
      } while (n);
      return NULL;
    }
  }

schemerlicht_object* schemerlicht_map_get_string(schemerlicht_map* map, const char* str)
  {
  schemerlicht_map_node* n = schemerlicht_hash_string(map, str);
  do
    {
    if ((get_key(n)->type == schemerlicht_object_type_string) && (strcmp(get_key(n)->value.s.string_ptr, str)==0))
      return get_value(n);
    else
      n = n->next;
    } while (n);
    return NULL;
  }

schemerlicht_object* schemerlicht_map_get(schemerlicht_context* ctxt, schemerlicht_map* map, const schemerlicht_object* key)
  {
  switch (key->type)
    {
    case schemerlicht_object_type_string:
      return schemerlicht_map_get_string(map, key->value.s.string_ptr);
    case schemerlicht_object_type_fixnum:
    {
    schemerlicht_memsize index = (schemerlicht_memsize)key->value.fx;
    if (cast(schemerlicht_fixnum, index) == key->value.fx)
      {
      return schemerlicht_map_get_indexed(map, index); // use specialized version, else go to default
      }
    }
    default:
      return schemerlicht_map_get_any(ctxt, map, key);
    }
  }

static schemerlicht_object* new_key(schemerlicht_context* ctxt, schemerlicht_map* map, const schemerlicht_object* key)
  {
  schemerlicht_map_node* main = schemerlicht_main_position(map, key);
  if (get_value(main)->type != schemerlicht_object_type_undefined) // main position is not free
    {
    schemerlicht_map_node* other = schemerlicht_main_position(map, get_key(main)); // main position of colliding node
    schemerlicht_map_node* n = map->first_free; // get a free place
    if (other != main) // if colliding node is out of its main position
      {
      //move colliding node into free position
      while (other->next != main)
        other = other->next;
      other->next = n; //redo the chain with n in place of main
      *n = *main; // copy colliding node into ree position
      main->next = NULL;
      get_value(main)->type = schemerlicht_object_type_undefined;
      }
    else // colliding node is in its own main position
      {
      // new node will go into free position
      n->next = main->next; // chain
      main->next = n;
      main = n;
      }
    }
  schemerlicht_set_object(get_key(main), key);
  schemerlicht_assert(get_value(main)->type == schemerlicht_object_type_undefined);
  for (;;) // set first_free correctly
    {
    if (get_key(map->first_free)->type == schemerlicht_object_type_undefined) // map still has a free place
      return get_value(main);
    else if (map->first_free == map->node) //cannot decrement from here
      break;
    else
      --(map->first_free);
    }
  // there are no free places anymore.
  get_value(main)->type = schemerlicht_object_type_fixnum;
  schemerlicht_rehash(ctxt, map); // grow map
  schemerlicht_object* val = cast(schemerlicht_object*, schemerlicht_map_get(ctxt, map, key));
  schemerlicht_assert(val->type == schemerlicht_object_type_fixnum);
  val->type = schemerlicht_object_type_undefined;
  return val;
  }

schemerlicht_object* schemerlicht_map_insert_indexed(schemerlicht_context* ctxt, schemerlicht_map* map, schemerlicht_memsize index)
  {
  schemerlicht_object* p = schemerlicht_map_get_indexed(map, index);
  if (p != NULL)
    return p;
  else
    {
    schemerlicht_object k;
    k.type = schemerlicht_object_type_fixnum;
    k.value.fx = cast(schemerlicht_fixnum, index);
    return new_key(ctxt, map, &k);
    }
  }

schemerlicht_object* schemerlicht_map_insert(schemerlicht_context* ctxt, schemerlicht_map* map, const schemerlicht_object* key)
  {
  schemerlicht_object* p = schemerlicht_map_get(ctxt, map, key);
  if (p != NULL)
    return p;
  else
    {
    if (key->type == schemerlicht_object_type_undefined)
      schemerlicht_runerror(ctxt, "table index is nil");
    return new_key(ctxt, map, key);
    }
  }

schemerlicht_object* schemerlicht_map_insert_string(schemerlicht_context* ctxt, schemerlicht_map* map, const char* str)
  {
  schemerlicht_object* p = schemerlicht_map_get_string(map, str);
  if (p != NULL)
    return p;
  else
    {
    schemerlicht_object key = make_schemerlicht_object_string(ctxt, str);
    schemerlicht_object* obj = new_key(ctxt, map, &key);
    return obj;
    }  
  }
