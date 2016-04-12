/*
 * =============================================================================
 *
 *       Filename:  famhmap.h
 *
 *    Description:  defination of Flexible Array Member Hashmap
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *  
 * =============================================================================
 */

#include "common.h"

#ifndef __FAMHMAP_H__
#define __FAMHMAP_H__

/********************************* DEFINATIONS ********************************/

/* Default number of slots */
#define FAMHMAP_SLOTSNUM_DFT 128

/* Size of one box of saving slot's id */
#define FAMHMAP_SLOTIDBOX_SIZE (1024 * 128)
/* Max valid number we can provide for type "numlimit_t" */
#define FAMHMAP_MAX_VALID_NUM (((long)INT_MAX) * FAMHMAP_SLOTIDBOX_SIZE)

/* Declare first */
struct famhmap;
struct famhmap_slot;

/* Flexible Array Member */
struct famhmap_node
{
    struct emblist_node chain;      // form a chain which having the same hash value
    char core[];                    // flexible array member to save the core
};

/* Memory allocator for Flexiable Array Member of struct "famhmap_node" */
#define famhmap_node_malloc(core_size)                                         \
    ( (struct famhmap_node *)                                                  \
      my_malloc(sizeof(struct famhmap_node) + (core_size)) )

/* Get the member "core" in struct "famhmap_node" */
#define famhmap_node_core(fhn) ((fhn)->core)

/* Get the address of the member "chain" in struct "famhmap_node" */
#define famhmap_node_pchain(fhn) (&((fhn)->chain))

/* Get pointer of "famhmap_node" by giving pointer of "emblist_node"(@pchain) */
#define famhmap_node_entry(pchain)                                             \
    emblist_node_entry(pchain, struct famhmap_node, chain)

/* Get the famhmap_node linked after the giving pointer of node(@fhn) */
#define famhmap_node_next(fhn)                                                 \
    famhmap_node_entry(emblist_node_next(famhmap_node_pchain(fhn)))

/* Common-part of core-genarator for core of type "elm1-elm2" */
#define famhmap_node_cgen_common(elm1, elm1size, elm2, elm2size)               \
    do                                                                         \
    {                                                                          \
        struct famhmap_node *fhn = famhmap_node_malloc(elm1size + elm2size);   \
        char *core = famhmap_node_core(fhn);                                   \
        memcpy(core, elm1, elm1size);                                          \
        memcpy(core + elm1size, elm2, elm2size);                               \
        return fhn;                                                            \
    }                                                                          \
    while (false)

/* Defination of a slot in famhmap */
struct famhmap_slot
{
    struct famhmap_node ehead;      // empty head node
    char spart[];                   // strengthen-part, may for concurrent
};

/* Get the address of the member "ehead"(empty head) in struct "famhmap_slot" */
#define famhmap_slot_pehead(pslot) (&((pslot)->ehead))
#define famhmap_slotid_pehead(fh, slotID)                                      \
    famhmap_slot_pehead((fh)->slots + slotID)
/* Get the pointer of "emblist_node" which points to head of slot's chain */
#define famhmap_slot_pchain(pslot) (&((pslot)->ehead.chain))
#define famhmap_slotid_pchain(fh, slotID)                                      \
    famhmap_slot_pchain((fh)->slots + slotID)

/* Get the address of "famhmap_slot" by giving pointer of it's member "ehead" */
#define famhmap_slot_entry(pehead)                                             \
    emblist_node_entry(pehead, struct famhmap_slot, ehead)

/* Get the size of chain for giving the pointer of the slot(@pslot) */
#define famhmap_slot_chain_size_calc(pslot)                                    \
    emblist_nodes_size_calc(&(pslot->ehead.chain))

/* Interface of how to get element from core */
typedef void *(*fun_famhmap_eget)(char *core);

/* Interface of how to compare two elements */
typedef int (*fun_famhmap_ecmp)(void *elm1, void *elm2);

/* Interface of how to genarate the core and so the struct "famhmap_node" */
typedef struct famhmap_node *(*fun_famhmap_cgen)(void *elm1, int elm1size, 
        void *elm2, int elm2size);

/* Interface of how to clean members or links inside the @core, not including 
 *  free the memories malloced for @core */
typedef void (*fun_famhmap_ccln)(char *core);

/* Interface of how to hash the key */
typedef unsigned long (*fun_famhmap_hash)(void *key);

/* Interface of the usual operation "get" of hashmap */
typedef void *(*fun_famhmap_hget)(struct famhmap *fh, void *key);

/* Return value of opeartion "hdel" */
#define FAMHMAP_RET_DELOK   0
#define FAMHMAP_RET_DELFAIL 1
/* Interface of the usual operation "delete" of hashmap */
typedef int (*fun_famhmap_hdel)(struct famhmap *fh, void *key);

/* Return value of operation "hput" */
#define FAMHMAP_RET_NEWPUT 0
#define FAMHMAP_RET_DELPUT 1
/* Interface of the usual operation "put" of hashmap */
typedef int (*fun_famhmap_hput)(struct famhmap *fh, void *key, void *val);

/* Elements can be set in "struct famhmap" */
enum FAMHMAP_ELEM
{
    FAMHMAP_ELEM_KGET   =   1,
    FAMHMAP_ELEM_VGET   =   2,
    FAMHMAP_ELEM_KCMP   =   3,
    FAMHMAP_ELEM_VCMP   =   4,
    FAMHMAP_ELEM_CGEN   =   5,
    FAMHMAP_ELEM_CCLN   =   6,
    FAMHMAP_ELEM_HASH   =   7,
    FAMHMAP_ELEM_HGBK   =   8,
    FAMHMAP_ELEM_HGBV   =   9,
    FAMHMAP_ELEM_HDEL   =  10,
    FAMHMAP_ELEM_HPUT   =  11
};

/* Struct holding datas for famhmap */
struct famhmap
{
    fun_famhmap_eget kget;          // how to get key from core
    fun_famhmap_eget vget;          // how to get value from core
    fun_famhmap_ecmp kcmp;          // how to compare two keys
    fun_famhmap_ecmp vcmp;          // how to compare two values
    fun_famhmap_cgen cgen;          // how to generate core
    fun_famhmap_ccln ccln;          // how to clean relation of core
    fun_famhmap_hash hash;          // how to hash keys
    fun_famhmap_hget hgbk;          // how to get value by key
    fun_famhmap_hget hgbv;          // how to get key by value
    fun_famhmap_hdel hdel;          // how to delete key-value for a giving key
    fun_famhmap_hput hput;          // how to put key-value pair into map
    void *spart;                    // main strengthen-part
    struct famhmap_slot *slots;     // pointer of slots-array
    long rsize;                     // real number of elements
    numlimit_t slotsnum;            // number of slots
    int ksize;                      // Fixed size of key, or '-1' means default
    int vsize;                      // Fixed size of value, or '-1' means default
    char htype;                     // hash type: key-value type
    char mtype;                     // map type: bimap or other 
    char ctype;                     // concurrent type
};

/* Get the real number of elements(K-V nodes) in @fh now */
#define famhmap_rsize(fh)   ((fh)->rsize)

/* Translet the type to detail string 
 * @whichtype: h|m|c
 * @type: %c describe in h|m|c
 */
extern char *
famhmap_type_translet(struct famhmap *fh, char whichtype, char type);

/* Visit all slots in famhmap(@fh) using a loop cursor @pslot */
#define famhmap_foreach_slot(pslot, fh)                                        \
    for (pslot = (fh)->slots + (fh)->slotsnum - 1;                             \
            pslot >= (fh)->slots; pslot--)

/* Visit all famhmap_node linked as a chain for giving slot(@pslot) using a loop
 *  cursor @fhn, and another loop cursor @fhn_tmp as temporary storage.
 * Attension that these mechods are not the fastest! */
#define famhmap_foreach_node_inslot(fhn, pslot)                                \
    for (fhn = famhmap_slot_pehead(pslot), fhn = famhmap_node_next(fhn);       \
            fhn != famhmap_slot_pehead(pslot); fhn = famhmap_node_next(fhn))
#define famhmap_foreach_node_inslot_safe(fhn, fhn_tmp, pslot)                  \
    for (fhn = famhmap_node_next(famhmap_slot_pehead(pslot)),                  \
            fhn_tmp = famhmap_node_next(fhn);                                  \
         fhn != famhmap_slot_pehead(pslot);                                    \
         fhn = fhn_tmp, fhn_tmp = famhmap_node_next(fhn))

/* Visit all famhmap_node in all slots of famhmap(@fh) (from back) using a loop
 *  cursor @fhn, and another loop cursor @fhn_tmp as temporary flag(not safe) */
#define famhmap_foreach_kvnode(fhn, fhn_tmp, fh)                               \
    for ( fhn_tmp = famhmap_slotid_pehead(fh, (fh)->slotsnum - 1),             \
            fhn = famhmap_node_next(fhn_tmp);                                  \
          fhn != fhn_tmp || ({                                                 \
            do                                                                 \
            {                                                                  \
                struct famhmap_slot *pslot = famhmap_slot_entry(fhn_tmp) - 1;  \
                fhn_tmp = pslot >= (fh)->slots ?                               \
                    famhmap_slot_pehead(pslot) : NULL;                         \
                if (fhn_tmp == NULL)                                           \
                    break;                                                     \
            } while ( (fhn = famhmap_node_next(fhn_tmp)) == fhn_tmp );         \
            fhn_tmp; });                                                       \
          fhn = famhmap_node_next(fhn) )

/* Print all types of famhmap(@fh) */
#define famhmap_print_type(fh)                                                 \
    do                                                                         \
    {                                                                          \
        printf("htype: %s\n", famhmap_type_translet(fh, 'h', (fh)->htype));    \
        printf("mtype: %s\n", famhmap_type_translet(fh, 'm', (fh)->mtype));    \
        printf("ctype: %s\n", famhmap_type_translet(fh, 'c', (fh)->ctype));    \
    }                                                                          \
    while (false)

/* Print status of famhmap(@fh) */
#define famhmap_print_status(fh)                                               \
    do                                                                         \
    {                                                                          \
        printf("number of slots: %ld\n", (long)(fh)->slotsnum);                \
        printf("number of K-Vs : %ld\n", famhmap_rsize(fh));                   \
    }                                                                          \
    while (false)

/* Print all size of slots in famhmap(@fh) 
 * @command
 *      true    ->  all sizes
 *      false   ->  only max size
 */
#define famhmap_print_slots_size(fh, command)                                  \
    do                                                                         \
    {                                                                          \
        struct famhmap_slot *pslot;                                            \
        int max_size = 0, now_size;                                            \
        famhmap_foreach_slot(pslot, fh)                                        \
        {                                                                      \
            now_size = famhmap_slot_chain_size_calc(pslot);                    \
            max_size = now_size > max_size ? now_size : max_size;              \
            if (command)                                                       \
                printf("%d\t", now_size);                                      \
        }                                                                      \
        if (command)                                                           \
            printf("\n");                                                      \
        printf("max slot size: %d\n", max_size);                               \
    }                                                                          \
    while (false)

/* DO NOT USE if you are not assure the usage is right */
#define famhmap_fun_check(fh, fun, funstr)                                     \
    if ((fh)->fun == NULL)                                                     \
        my_error_exit(                                                         \
                "famhmap htype %c mtype %c ctype %c has NO such function %s!", \
                (fh)->htype, (fh)->mtype, (fh)->ctype, funstr);

struct famhmap_spart_idbox
{
    numlimit_t slotids[FAMHMAP_SLOTIDBOX_SIZE];
};

struct famhmap_spart_idbimap
{
    struct famhmap_spart_idbox **boxes;
    int boxnum;
};

/****************************** EXTERN FUNCTIONS ******************************/

/* Get a new inited famhmap 
 * @slotsnum: expected number of slots, it will be set to "FAMHMAP_SLOTSNUM_DFT"
 *  if it is less than 1
 * @htype: type of key-value pair
 *      'd' ->  string-int (but int was saved before string in "core")
 *      'i' ->  int-any_fixed_type
 *      'I' ->  int-string
 *      's' ->  string-any_fixed_type
 *      'S' ->  string-string (default)
 * @mtype: type of mapping mechanism     
 *      'd' ->  bi-directional map for K-V of string-int (V is treated as auto-incremented ID)
 *      's' ->  single-directional map (default)
 * @ctype: type of concurrent support
 *      'c' ->  slot-lock concurrent support
 *      's' ->  single-thread support (default)
 * @ksize & @vsize: fixed-size of key or value, it is not used in many htype but
 *  recommend to set it to -1 for more clearly
 *      -1  ->  default, not reference to the giving parameter
 */
extern struct famhmap *
famhmap_init(int slotsnum, char htype, char mtype, char ctype, int ksize, int vsize);

/* Free all resources for holding such a famhmap(@fh) */
extern void 
famhmap_destroy(struct famhmap *fh);

/* Set elements(most are call-back functions) in "struct famhmap"
 * @fh: The struct need to modify elements
 * @fhe: indicates element
 *      FAMHMAP_ELEM_KGET   ->  how to get key from core 
 *      FAMHMAP_ELEM_VGET   ->  how to get value from core
 *      FAMHMAP_ELEM_KCMP   ->  how to compare two keys
 *      FAMHMAP_ELEM_VCMP   ->  how to compare two values
 *      FAMHMAP_ELEM_CGEN   ->  how to generate core from key-value
 *      FAMHMAP_ELEM_CCLN   ->  how to clean outter relations in core
 *      FAMHMAP_ELEM_HASH   ->  how to hash keys
 *      FAMHMAP_ELEM_HGBK   ->  how to get value by key
 *      FAMHMAP_ELEM_HGBV   ->  how to get key by value
 *      FAMHMAP_ELEM_HDEL   ->  how to delete a pair of key-value
 *      FAMHMAP_ELEM_HPUT   ->  how to put key-value into hashmap
 * @new_value: new value of element(@fhe)    
 */
extern void 
famhmap_set(struct famhmap *fh, enum FAMHMAP_ELEM fhe, void *new_value);

/* Get value by key */
#ifndef NDEBUG
#define famhmap_getv(fh, key)                                                  \
    ({                                                                         \
        famhmap_fun_check(fh, hgbk, "getv(hgbk)");                             \
        (*(fh)->hgbk)(fh, key);                                                \
    })
#else
#define famhmap_getv(fh, key)                                                  \
    (*(fh)->hgbk)(fh, key)
#endif

/* Get key by value */
#ifndef NDEBUG
#define famhmap_getk(fh, val)                                                  \
    ({                                                                         \
        famhmap_fun_check(fh, hgbv, "getk(hgbv)");                             \
        (*(fh)->hgbv)(fh, val);                                                \
    })
#else
#define famhmap_getk(fh, val)                                                  \
    (*(fh)->hgbv)(fh, val)
#endif

/* Delect giving key and corresponding value */
#ifndef NDEBUG
#define famhmap_delete(fh, key)                                                \
    ({                                                                         \
        famhmap_fun_check(fh, hdel, "delete(hdel)");                           \
        (*(fh)->hdel)(fh, key);                                                \
    })
#else
#define famhmap_delete(fh, key)                                                \
    (*(fh)->hdel)(fh, key)
#endif

/* Put key and value into the famhmap */
#ifndef NDEBUG
#define famhmap_put(fh, key, value)                                            \
    ({                                                                         \
        famhmap_fun_check(fh, hgbk, "put(hput)");                              \
        (*(fh)->hput)(fh, key, value);                                         \
    })
#else
#define famhmap_put(fh, key, value)                                            \
    (*(fh)->hput)(fh, key, value)
#endif

/*get value from node*/
#ifndef NDEBUG
#define famhmap_node_getv(fh, fhn)                                                 \
    ({                                                                         \
        famhmap_fun_check(fh, vget, "ngetv(vget)");                            \
        (*(fh)->vget)(famhmap_node_core(fhn));                                 \
    })
#else
#define famhmap_node_getv(fh, fhn)                                                 \
    (*(fh)->vget)(famhmap_node_core(fhn))                                     
#endif

/*get key from node*/
#ifndef NDEBUG
#define famhmap_node_getk(fh, fhn)                                                 \
    ({                                                                         \
        famhmap_fun_check(fh, kget, "ngetk(kget)");                            \
        (*(fh)->kget)(famhmap_node_core(fhn));                                 \
    })
#else
#define famhmap_node_getk(fh, fhn)                                                 \
    (*(fh)->kget)(famhmap_node_core(fhn))                                     
#endif
/***************************** CALL-BACK FUNCTIONS ****************************/

#endif /* __FAMHMAP_H__ */

