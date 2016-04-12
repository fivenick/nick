/*
 * =============================================================================
 *
 *       Filename:  famhmap.c
 *
 *    Description:  implementation of Flexible Array Member Hashmap
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/famhmap.h"

/***************************** INTERNAL FUNCTIONS *****************************/

static void *
famhmap_eget_1st(char *core);
static void *
famhmap_eget_2nd_i1st(char *core);
static void *
famhmap_eget_2nd_s1st(char *core);

static int 
famhmap_ecmp_str(void *elm1, void *elm2);
static int 
famhmap_ecmp_int(void *elm1, void *elm2);

static struct famhmap_node *
famhmap_cgen_ss(void *elm1, int elm1size, void *elm2, int elm2size);
static struct famhmap_node *
famhmap_cgen_si(void *elm1, int elm1size, void *elm2, int elm2size);
static struct famhmap_node *
famhmap_cgen_sa(void *elm1, int elm1size, void *elm2, int elm2size);
static struct famhmap_node *
famhmap_cgen_is(void *elm1, int elm1size, void *elm2, int elm2size);
static struct famhmap_node *
famhmap_cgen_ii(void *elm1, int elm1size, void *elm2, int elm2size);
static struct famhmap_node *
famhmap_cgen_ia(void *elm1, int elm1size, void *elm2, int elm2size);
static struct famhmap_node *
famhmap_cgen_aa(void *elm1, int elm1size, void *elm2, int elm2size);

static unsigned long 
famhmap_hash_str(void *key);
static unsigned long 
famhmap_hash_int(void *key);

static void 
famhmap_spart_idbimap_expand(struct famhmap_spart_idbimap *fhsp, int boxid);
static void 
famhmap_spart_idbimap_standby(struct famhmap *fh, long val);
static numlimit_t 
famhmap_spart_idbimap_getid(struct famhmap *fh, long val);
static void 
famhmap_spart_idbimap_saveid(struct famhmap *fh, long val, numlimit_t slotID);

static struct famhmap_node *
famhmap_hgbk_prefhn_sp(struct famhmap *fh, numlimit_t slotID, void *key);
static struct famhmap_node *
famhmap_hgbk_fhn_sp(struct famhmap *fh, numlimit_t slotID, void *key);
static void *
famhmap_hgbk_norm(struct famhmap *fh, void *key);

static struct famhmap_node *
famhmap_hgbv_prefhn_sp(struct famhmap *fh, numlimit_t slotID, void *val);
static struct famhmap_node *
famhmap_hgbv_fhn_sp(struct famhmap *fh, numlimit_t slotID, void *val);
static void *
famhmap_hgbv_norm_dds(struct famhmap *fh, void *val);

static int 
famhmap_hdel_after_sp(struct famhmap *fh, struct famhmap_node *fhn_pre);
static int 
famhmap_hdel_norm(struct famhmap *fh, void *key);
static int 
famhmap_hdel_norm_dds(struct famhmap *fh, void *key);

static int 
famhmap_hput_sp(struct famhmap *fh, numlimit_t slotID, void *key, void *val);
static int 
famhmap_hput_norm(struct famhmap *fh, void *key, void *val);
static int 
famhmap_hput_norm_dds(struct famhmap *fh, void *key, void *val);

/******************************** SOUCE CODES *********************************/

/* Get address of element inside @core when element is first in @core */
void *famhmap_eget_1st(char *core)
{
    return core;
}

/* Get address of element inside @core when element is second and the first
 * element is a int
 */ 
void *famhmap_eget_2nd_i1st(char *core)
{
    return core + sizeof(int);
}

/* Get address of element inside @core when element is second and the first
 * element is a string 
 */
void *famhmap_eget_2nd_s1st(char *core)
{
    return core + strlen(core) + 1;
}

/* Comparator for two string */
int famhmap_ecmp_str(void *elm1, void *elm2)
{
    return strcmp((char *)elm1, (char *)elm2);
}

/* Comparator for two int */
int famhmap_ecmp_int(void *elm1, void *elm2)
{
    return *((int *)elm1) - *((int *)elm2); 
}

/* Core genarator for core of type "string-string" */
struct famhmap_node *famhmap_cgen_ss(void *elm1, int elm1size, void *elm2, int elm2size)
{
    famhmap_node_cgen_common(elm1, strlen(elm1) + 1, elm2, strlen(elm2) + 1);
}

/* Core genarator for core of type "string-int" */
struct famhmap_node *famhmap_cgen_si(void *elm1, int elm1size, void *elm2, int elm2size)
{
    famhmap_node_cgen_common(elm1, strlen(elm1) + 1, elm2, sizeof(int));
}

/* Core genarator for core of type "string-anytype" */
struct famhmap_node *famhmap_cgen_sa(void *elm1, int elm1size, void *elm2, int elm2size)
{
    if (elm2size < 0)
        my_error_exit("famhmap_cgen_sa: elm2size can not be less than 0!");
    famhmap_node_cgen_common(elm1, strlen(elm1) + 1, elm2, elm2size);
}

/* Core genarator for core of type "int-string" */
struct famhmap_node *famhmap_cgen_is(void *elm1, int elm1size, void *elm2, int elm2size)
{
    famhmap_node_cgen_common(elm1, sizeof(int), elm2, strlen(elm2) + 1);
}

/* Core genarator for core of type "int-int" */
struct famhmap_node *famhmap_cgen_ii(void *elm1, int elm1size, void *elm2, int elm2size)
{
    famhmap_node_cgen_common(elm1, sizeof(int), elm2, sizeof(int));
}

/* Core genarator for core of type "int-anytype" */
struct famhmap_node *famhmap_cgen_ia(void *elm1, int elm1size, void *elm2, int elm2size)
{
    if (elm2size < 0)
        my_error_exit("famhmap_cgen_sa: elm2size can not be less than 0!");
    famhmap_node_cgen_common(elm1, sizeof(int), elm2, elm2size);
}

/* Core genarator for core of type "anytype-anytype" */
struct famhmap_node *famhmap_cgen_aa(void *elm1, int elm1size, void *elm2, int elm2size)
{
    if (elm1size < 0)
        my_error_exit("famhmap_cgen_sa: elm1size can not be less than 0!");
    if (elm2size < 0)
        my_error_exit("famhmap_cgen_sa: elm2size can not be less than 0!");
    famhmap_node_cgen_common(elm1, elm1size, elm2, elm2size);
}

/* Classic simple string-hash algorithm */
unsigned long famhmap_hash_str(void *key)
{
    register unsigned long h = 0;
    register unsigned char *p;
    for(h = 0, p = (unsigned char *)key; *p; p++)
        h = h * 31 + *p;
    return h;
}

/* Simple int-hash method */
unsigned long famhmap_hash_int(void *key)
{
    return (unsigned long)(*((int *)key));
}

/* Expand the idboxes' pointer-array for getting more boxes that enough to save @boxid.
 * Assert that boxid >= fhsp->boxnum */
inline void famhmap_spart_idbimap_expand(struct famhmap_spart_idbimap *fhsp, int boxid)
{
    assert(boxid >= fhsp->boxnum);
    int newboxnum = boxid + 1;
    struct famhmap_spart_idbox **tmpboxes = my_calloc_ntype(newboxnum, struct famhmap_spart_idbox *);
    memcpy(tmpboxes, fhsp->boxes, fhsp->boxnum * sizeof(struct famhmap_spart_idbox *));
    free(fhsp->boxes);
    fhsp->boxes = tmpboxes;
    fhsp->boxnum = newboxnum;
}

/* Try to prepare enough spaces to save @boxid */
inline void famhmap_spart_idbimap_standby(struct famhmap *fh, long val)
{
    int boxid = (int)(val / FAMHMAP_SLOTIDBOX_SIZE);
    struct famhmap_spart_idbimap *fhsp = (struct famhmap_spart_idbimap *)fh->spart;
    if (boxid >= fhsp->boxnum)
        famhmap_spart_idbimap_expand(fhsp, boxid);
    if ((fhsp->boxes)[boxid] == NULL)
        (fhsp->boxes)[boxid] = my_calloc_type(struct famhmap_spart_idbox);
}

/* Get the corresponding slotID when giving the value(@val) of K-V pair */
inline numlimit_t famhmap_spart_idbimap_getid(struct famhmap *fh, long val)
{
    assert(FAMHMAP_MAX_VALID_NUM > val && val >= 0);
    int boxid = (int)(val / FAMHMAP_SLOTIDBOX_SIZE);
    assert(boxid >= 0);
    struct famhmap_spart_idbimap *fhsp = (struct famhmap_spart_idbimap *)fh->spart;
    if (boxid >= fhsp->boxnum)
        return -1;
    struct famhmap_spart_idbox *fhspbox = fhsp->boxes[boxid];
    if (fhspbox == NULL)
        return -1;
    else
    {
        numlimit_t minorid = (numlimit_t)(val % FAMHMAP_SLOTIDBOX_SIZE);
        assert(minorid >= 0);
        // value "-1" means invalid(initialized with 0)
        return (fhspbox->slotids)[minorid] - 1;
    }
}

/* Save the corresponding slotID for giving value(@val) of K-V pair */
inline void famhmap_spart_idbimap_saveid(struct famhmap *fh, long val, numlimit_t slotID)
{
    assert(FAMHMAP_MAX_VALID_NUM > val && val >= 0);
    int boxid = (int)(val / FAMHMAP_SLOTIDBOX_SIZE);
    assert(boxid >= 0);
    numlimit_t minorid = (numlimit_t)(val % FAMHMAP_SLOTIDBOX_SIZE);
    assert(minorid >= 0);
    struct famhmap_spart_idbimap *fhsp = (struct famhmap_spart_idbimap *)fh->spart;
    struct famhmap_spart_idbox *fhspbox = fhsp->boxes[boxid];
    // value "-1" means invalid(initialized with 0), slotID == 0 indicates value == 1
    (fhspbox->slotids)[minorid] = slotID + 1;
}

/* Use hash method to get the hash value and then transform to ID of slot */
#define toslotID_common(fh, key)                                               \
    ( (numlimit_t)( (*(fh)->hash)(key) % ((fh)->slotsnum) ) )

/* Speedly find the pre-node of the node have @key when giving slot's ID(@slotID) */
inline struct famhmap_node *famhmap_hgbk_prefhn_sp(struct famhmap *fh, numlimit_t slotID, void *key)
{
    struct emblist_node *ehead = famhmap_slotid_pchain(fh, slotID);
    struct emblist_node *now, *pre;
    pre = ehead;
    emblist_node_foreach(now, ehead)
    {
        struct famhmap_node *fhn = famhmap_node_entry(now);
        if ( (*fh->kcmp)((*fh->kget)(fhn->core), key) == 0 ) 
            return famhmap_node_entry(pre); 
        pre = now;
    }
    return NULL;
}

/* Speedly find the node have @key when giving slot's ID(@slotID) */
inline struct famhmap_node *famhmap_hgbk_fhn_sp(struct famhmap *fh, numlimit_t slotID, void *key)
{
    struct emblist_node *ehead = famhmap_slotid_pchain(fh, slotID);
    struct emblist_node *now;
    emblist_node_foreach(now, ehead)
    {
        struct famhmap_node *fhn = famhmap_node_entry(now);
        if ( (*fh->kcmp)((*fh->kget)(fhn->core), key) == 0 ) 
            return fhn; 
    }
    return NULL;
}

/* Normal mechod of get the value from famhmap(@fh) by giving @key */
void *famhmap_hgbk_norm(struct famhmap *fh, void *key)
{
    numlimit_t slotID = toslotID_common(fh, key);
    struct famhmap_node *fhn = famhmap_hgbk_fhn_sp(fh, slotID, key);
    if (fhn == NULL)
        return NULL;
    else
        return (*fh->vget)(fhn->core);
}

/* Speedly find the pre-node of the node have @val when giving slot's ID(@slotID) */
inline struct famhmap_node *famhmap_hgbv_prefhn_sp(struct famhmap *fh, numlimit_t slotID, void *val)
{
    struct emblist_node *ehead = famhmap_slotid_pchain(fh, slotID);
    struct emblist_node *now, *pre;
    pre = ehead;
    emblist_node_foreach(now, ehead)
    {
        struct famhmap_node *fhn = famhmap_node_entry(now);
        if ( (*fh->vcmp)((*fh->vget)(fhn->core), val) == 0 ) 
            return famhmap_node_entry(pre); 
        pre = now;
    }
    return NULL;
}

/* Speedly find the node have @val when giving slot's ID(@slotID) */
inline struct famhmap_node *famhmap_hgbv_fhn_sp(struct famhmap *fh, numlimit_t slotID, void *val)
{
    struct emblist_node *ehead = famhmap_slotid_pchain(fh, slotID);
    struct emblist_node *now;
    emblist_node_foreach(now, ehead)
    {
        struct famhmap_node *fhn = famhmap_node_entry(now);
        if ( (*fh->vcmp)((*fh->vget)(fhn->core), val) == 0 ) 
            return fhn; 
    }
    return NULL;
}

/* Normal mechod of get the key from famhmap(@fh) by giving @val of type "d(h)d(m)s(c)" */
void *famhmap_hgbv_norm_dds(struct famhmap *fh, void *val)
{
    long valtmp = (long)*((numlimit_t *)val);
    assert(valtmp >= 0);
    numlimit_t slotID = famhmap_spart_idbimap_getid(fh, valtmp);
    if (slotID == -1)
        return NULL;
    struct famhmap_node *fhn = famhmap_hgbv_fhn_sp(fh, slotID, val);
    if (fhn == NULL)
        return NULL;
    else
        return (*fh->kget)(fhn->core);
}

/* Speedly delete the node linked after the giving node(@fhn_pre) */
inline int famhmap_hdel_after_sp(struct famhmap *fh, struct famhmap_node *fhn_pre)
{
    struct emblist_node *eln_now = famhmap_node_pchain(fhn_pre);
    struct famhmap_node *fhn_del = famhmap_node_entry(emblist_node_next(eln_now));
    if (fh->ccln != NULL)
        (*fh->ccln)(fhn_del->core);
    emblist_node_rm_after_sp(eln_now);
    free(fhn_del);
    (fh->rsize)--;
}

/* Normal method of deleting the key-value pair which the key is @key */
int famhmap_hdel_norm(struct famhmap *fh, void *key)
{
    numlimit_t slotID = toslotID_common(fh, key);    
    struct famhmap_node *fhn_pre = famhmap_hgbk_prefhn_sp(fh, slotID, key);
    if (fhn_pre == NULL)
        return FAMHMAP_RET_DELFAIL;
    else
    {
        famhmap_hdel_after_sp(fh, fhn_pre);
        return FAMHMAP_RET_DELOK;
    }   
}

/* Normal method of deleting the key-value pair which the key is @key of type "d(h)d(m)s(c)" */
int famhmap_hdel_norm_dds(struct famhmap *fh, void *key)
{
    numlimit_t slotID = toslotID_common(fh, key);    
    struct famhmap_node *fhn_pre = famhmap_hgbk_prefhn_sp(fh, slotID, key);
    if (fhn_pre == NULL)
        return FAMHMAP_RET_DELFAIL;
    else
    {
        // delete relation for idbimap
        struct famhmap_node *fhn_del = famhmap_node_next(fhn_pre);
        void *val = (*fh->vget)(fhn_del->core);
        long valtmp = (long)*((numlimit_t *)val);
        assert(valtmp >= 0);
        numlimit_t slotID = famhmap_spart_idbimap_getid(fh, valtmp);
        if (slotID != -1)
            famhmap_spart_idbimap_saveid(fh, valtmp, -1);
        // delete node in famhmap
        famhmap_hdel_after_sp(fh, fhn_pre);
        return FAMHMAP_RET_DELOK;
    }   
}

/* Speedly put key-value into the slot have ID(@slotID) without concerning anything */
inline int famhmap_hput_sp(struct famhmap *fh, numlimit_t slotID, void *key, void *val)
{
    struct famhmap_node *fhn = (*fh->cgen)(key, fh->ksize, val, fh->vsize);
    struct emblist_node *ehead = famhmap_slotid_pchain(fh, slotID);
    emblist_node_add_after(ehead, famhmap_node_pchain(fhn));
    (fh->rsize)++;
}

/* Normal method of putting key-value into the famhmap */
int famhmap_hput_norm(struct famhmap *fh, void *key, void *val)
{
    numlimit_t slotID = toslotID_common(fh, key);
    struct famhmap_node *fhn_pre = famhmap_hgbk_prefhn_sp(fh, slotID, key);
    if (fhn_pre == NULL)
    {
        famhmap_hput_sp(fh, slotID, key, val);
        return FAMHMAP_RET_NEWPUT;
    }
    else
    {
        famhmap_hdel_after_sp(fh, fhn_pre);
        famhmap_hput_sp(fh, slotID, key, val);
        return FAMHMAP_RET_DELPUT;
    }
}

/* Normal method of putting key-value into the famhmap of type "d(h)d(m)s(c)" */
int famhmap_hput_norm_dds(struct famhmap *fh, void *key, void *val)
{
    numlimit_t slotID = toslotID_common(fh, key);
    // save idbimap
    long valtmp = (long)*((numlimit_t *)val);
    famhmap_spart_idbimap_standby(fh, valtmp);
    famhmap_spart_idbimap_saveid(fh, valtmp, slotID);
    // put K-V pair
    struct famhmap_node *fhn_pre = famhmap_hgbk_prefhn_sp(fh, slotID, key);
    if (fhn_pre == NULL)
    {
        famhmap_hput_sp(fh, slotID, val, key);
        return FAMHMAP_RET_NEWPUT;
    }
    else
    {
        famhmap_hdel_after_sp(fh, fhn_pre);
        famhmap_hput_sp(fh, slotID, val, key);
        return FAMHMAP_RET_DELPUT;
    }
}

/* Translet the type to detail string 
 * @whichtype: h|m|c
 * @type: %c describe in h|m|c
 */
char *famhmap_type_translet(struct famhmap *fh, char whichtype, char type)
{
    switch (whichtype)
    {
        case 'h':
            switch (type)
            {
                case 'd':
                    return "string-int (but int was saved before string in \"core\")";
                case 'i':
                    return "int-any_fixed_type";
                case 'I':
                    return "int-string";
                case 's':
                    return "string-any_fixed_type";
                case 'S':
                    return "string-string";
                default:
                    return "Undefined";
            }
        case 'm':
            switch (type)
            {
                case 'd':
                    return "bi-directional map for K-V of string-int (V is treated as auto-incremented ID)";
                case 's':
                    return "single-directional map";
                default:
                    return "Undefined";
            }
        case 'c':
            switch (type)
            {
                case 'c':
                    return "slot-lock concurrent support";
                case 's':
                    return "single-thread support";
                default:
                    return "Undefined";
            }
        default:
            return "Undefined";
    }
}

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
struct famhmap *famhmap_init(int slotsnum, char htype, char mtype, char ctype, int ksize, int vsize)
{
    if (slotsnum < 1)
        slotsnum = FAMHMAP_SLOTSNUM_DFT;
    struct famhmap *fh = NULL;
    // handle mtype and ctype
    switch (mtype)
    {
        case 'd':
            fh = my_malloc_type(struct famhmap);
            if (htype != 'd')
                my_error_exit("mtype == 'd' only surpport htype == 'd'!");
            switch (ctype)
            {
                case 'c':
                    // TODO
                    my_error_exit("Giving type is not supported now!");
                    break;
                case 's':
                default:
                    fh->slots = my_malloc_ntype(slotsnum, struct famhmap_slot);
                    fh->hgbk = famhmap_hgbk_norm;
                    fh->hgbv = famhmap_hgbv_norm_dds;
                    fh->hdel = famhmap_hdel_norm_dds;
                    fh->hput = famhmap_hput_norm_dds;
                    fh->spart = my_calloc(1, sizeof(struct famhmap_spart_idbimap));
            }
            break;
        case 's':
        default:
            fh = my_malloc_type(struct famhmap);
            switch (ctype)
            {
                case 'c':
                    // TODO
                    my_error_exit("Giving type is not supported now!");
                    break;
                case 's':
                default:
                    fh->slots = my_malloc_ntype(slotsnum, struct famhmap_slot);
                    fh->hgbk = famhmap_hgbk_norm;
                    fh->hgbv = NULL;
                    fh->hdel = famhmap_hdel_norm;
                    fh->hput = famhmap_hput_norm;
                    fh->spart = NULL;
            }
    }
    assert(fh != NULL);
    assert(fh->slots != NULL);
    // init types and fixed-size
    fh->htype = htype;
    fh->mtype = mtype;
    fh->ctype = ctype;
    fh->ksize = ksize;
    fh->vsize = vsize;
    // handle htype
    switch (htype)
    {
        case 'd':
            fh->kget = famhmap_eget_2nd_i1st;
            fh->vget = famhmap_eget_1st;
            fh->kcmp = famhmap_ecmp_str;
            fh->vcmp = famhmap_ecmp_int;
            fh->cgen = famhmap_cgen_is;
            fh->ccln = NULL;
            fh->hash = famhmap_hash_str;
            break;
        case 'i':
            fh->kget = famhmap_eget_1st;
            fh->vget = famhmap_eget_2nd_i1st;
            fh->kcmp = famhmap_ecmp_int;
            fh->vcmp = NULL;
            fh->cgen = famhmap_cgen_ia;
            fh->ccln = NULL;
            fh->hash = famhmap_hash_int;
            break;
        case 'I':
            fh->kget = famhmap_eget_1st;
            fh->vget = famhmap_eget_2nd_i1st;
            fh->kcmp = famhmap_ecmp_int;
            fh->vcmp = famhmap_ecmp_str;
            fh->cgen = famhmap_cgen_is;
            fh->ccln = NULL;
            fh->hash = famhmap_hash_int;
            break;
        case 's':
            fh->kget = famhmap_eget_1st;
            fh->vget = famhmap_eget_2nd_s1st;
            fh->kcmp = famhmap_ecmp_str;
            fh->vcmp = NULL;
            fh->cgen = famhmap_cgen_sa;
            fh->ccln = NULL;
            fh->hash = famhmap_hash_str;
            break;
        case 'S':
        default:
            fh->kget = famhmap_eget_1st;
            fh->vget = famhmap_eget_2nd_s1st;
            fh->kcmp = famhmap_ecmp_str;
            fh->vcmp = famhmap_ecmp_str;
            fh->cgen = famhmap_cgen_ss;
            fh->ccln = NULL;
            fh->hash = famhmap_hash_str;
    }
    // init slots
    fh->slotsnum = slotsnum;
    struct famhmap_slot *fhs;
    famhmap_foreach_slot(fhs, fh)
        emblist_node_self(famhmap_slot_pchain(fhs));
    fh->rsize = 0;
    return fh;
}

/* Free all resources for holding such a famhmap(@fh) */
void famhmap_destroy(struct famhmap *fh)
{
    struct famhmap_slot *fhs;
    struct famhmap_node *fhn;
    struct emblist_node *eln, *elntmp;
    if (fh->ccln != NULL)
    {
        famhmap_foreach_slot(fhs, fh)
        {
            emblist_node_foreach_safe(eln, elntmp, famhmap_slot_pchain(fhs))
            {
                fhn = famhmap_node_entry(eln);
                (*fh->ccln)(fhn->core);
                free(fhn);
            }
        }
    }
    else
    {
        famhmap_foreach_slot(fhs, fh)
        {
            struct famhmap_node *fhn_tmp;
            emblist_node_foreach_safe(eln, elntmp, famhmap_slot_pchain(fhs))
            {
                fhn = famhmap_node_entry(eln);
                free(fhn);
            }
        }
    }
    free(fh->slots);
    if (fh->mtype == 'd')
    {
        int i;
        struct famhmap_spart_idbimap *fhsp = (struct famhmap_spart_idbimap *)fh->spart;
        for (i = 0; i < fhsp->boxnum; i++)
            free((fhsp->boxes)[i]);
        free(fhsp->boxes);
        free(fhsp);
    }
    free(fh);
}

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
inline void famhmap_set(struct famhmap *fh, enum FAMHMAP_ELEM fhe, void *new_value)
{
    switch (fhe)
    {
        case FAMHMAP_ELEM_KGET:
            fh->kget = new_value;
            break;
        case FAMHMAP_ELEM_VGET:
            fh->vget = new_value;
            break;
        case FAMHMAP_ELEM_KCMP:
            fh->kcmp = new_value;
            break;
        case FAMHMAP_ELEM_VCMP:
            fh->vcmp = new_value;
            break;
        case FAMHMAP_ELEM_CGEN:
            fh->cgen = new_value;
            break;
        case FAMHMAP_ELEM_CCLN:
            fh->ccln = new_value;
            break;
        case FAMHMAP_ELEM_HASH:
            fh->hash = new_value;
            break;
        case FAMHMAP_ELEM_HGBK:
            fh->hgbk = new_value;
            break;
        case FAMHMAP_ELEM_HGBV:
            fh->hgbv = new_value;
            break;
        case FAMHMAP_ELEM_HDEL:
            fh->hdel = new_value;
            break;
        case FAMHMAP_ELEM_HPUT:
            fh->hput = new_value;
            break;
        default:
            my_error_exit("famhmap_set: Unknown element in struct famhmap!");
    }
}

