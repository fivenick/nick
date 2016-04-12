/*
 * =============================================================================
 *
 *       Filename:  emblist.h
 *
 *    Description:  defination of emblist
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "common.h"

#ifndef __EMBLIST_H__
#define __EMBLIST_H__

/* SPECIFICATION:
 *
 * 1. emblist is a Circular Single Linked List, "struct emblist_node" is used
 *  as a member of a "father-struct" which you can define freely, then a list 
 *  is formed by linking these nodes. Datas can be saved as other members of
 *  "father-struct", you can use macro "emblist_node_entry" to get the pointer 
 *  of father. Methods of "emblist_node(s)_*" give lots of pure operations for 
 *  the most simplest usage of emblist.
 *
 *  Particularlly, we use "rm(remove)" to indicate removing a node from linked
 *  list, but "father-struct" of this node is still alive, you must FREE it if
 *  it is malloced.
 *
 * 2. More convenientlly, we introduce a special struct "emblist". It contains
 *  members for saving useful information of emblist, but you may not use it if
 *  you want to use less memories.
 *
 *  Do you want to free all malloced memories for a emblist? "emblist_destroy"
 *  shows a nice way to do this! Use "emblist_clean" to free all malloced
 *  memories for all removed nodes.
 *
 *  In "struct emblist", "head" and "trash" are empty nodes (no real data so
 *  different from other nodes) used as a head node.
 *
 * 3. We want to use a special pointer of type "struct emblist_spart" in the
 *  "struct emblist" to dynamiclly install extended-functionality for emblist,
 *  to support more amazing usage.
 */

/*---------------------------- part emblist_node -----------------------------*/

/* The simplest struct for saving information of a emblist-node */
struct emblist_node
{
    struct emblist_node *next;  // the pointer of next node
};

/* Get the pointer of next node */
#define emblist_node_next(eln) ((eln)->next)

/* Let the node(@eln) form a list (Circular Linked List) itself */
extern void 
emblist_node_self(struct emblist_node *eln);

/* Add a node(@eln_new) after the giving node(@eln_pos) */
extern void
emblist_node_add_after(struct emblist_node *eln_pos, 
        struct emblist_node *eln_new);

/* Speeding method of removing node(@eln_del) when giving pre-node(@eln_pre) */
extern void 
emblist_node_rm_sp(struct emblist_node *eln_pre, struct emblist_node *eln_del);
/* Speeding method of removing the node after the giving pre-node(@eln_pre) */
extern void 
emblist_node_rm_after_sp(struct emblist_node *eln_pre);

/* Visit each node after and exclude @start using a loop cursor @pos */
#define emblist_node_foreach(pos, head)                                        \
    for (pos = (head)->next; pos != (head); pos = pos->next)
/* @tpos is another loop cursor to use as temporary storage */
#define emblist_node_foreach_safe(pos, tpos, head)                             \
    for (pos = (head)->next, tpos = pos->next; pos != (head);                  \
            pos = tpos, tpos = pos->next)

/* Get the pointer of father-struct 
 * @pnode: pointer of the "emblist_node" inside father-struct
 * @type: type of the father-struct
 * @member: name of the member for "emblist_node" in father-struct
 */
#define emblist_node_entry(pnode, type, member)                                \
    my_container_of(pnode, type, member)

/* Free the father-struct of node @pnode
 * ATTENTION: this operation may corrupt the relation of emblist!
 * @destroyer: destroyer for freeing all malloced memories for father-struct
 * @pnode @type @member refer to "emblist_node_entry"
 */
#define emblist_node_free(pnode, type, member, destroyer)                      \
    (*destroyer)(emblist_node_entry(pnode, type, member))
/* Free all nodes linked after @pnode exclude @pnode
 * @pnode @type @member @destroyer refer to "emblist_node_free"
 */
#define emblist_nodes_free(pnode, type, member, destroyer)                     \
    do                                                                         \
    {                                                                          \
        struct emblist_node *elnp, *elnptmp;                                   \
        emblist_node_foreach_safe(elnp, elnptmp, pnode)                        \
            (*destroyer)(emblist_node_entry(elnp, type, member));              \
    }                                                                          \
    while (false)

/* Find the pre-node of node @elnf by visiting start from node @elns */
extern struct emblist_node *
emblist_nodes_fd(struct emblist_node *elns, struct emblist_node *elnf);

/* Merge two list, linked all nodes after @eln2 to @eln1 but before original
 *  nodes which are linked after @eln1 :
 * @eln1 -> nodes after @eln2 -> nodes after @eln1 
 * @eln2 -> @eln2
 */
extern struct emblist_node *
emblist_nodes_merge(struct emblist_node *eln1, struct emblist_node *eln2);

/* Calculate the size of nodes linked after @elns exclude @elns */
extern int 
emblist_nodes_size_calc(struct emblist_node *elns);

/* Save all pointers of nodes linked after and exclude @elns to @elnpp
 * MAKE SURE size is the same with "emblist_nodes_size_calc(elns)"
 */
extern void 
emblist_nodes_toarray(struct emblist_node **elnpp, int size, 
        struct emblist_node *elns);

/* Rebuild linked relation of emblist using array @elnpp 
 * @size: size of array @elnpp
 * @head: head node of rebuild list, head will be elnpp[0] if it is NULL
 * @return: pointer of head node
 */
extern struct emblist_node * 
emblist_nodes_rebuild(struct emblist_node **elnpp, int size, 
        struct emblist_node *head);

/*------------------------------ part emblist --------------------------------*/

/* Interface of a strengthen-part for removing this part */
typedef void (*fun_emblist_spart_rm)(void *spart);

/* Hold datas for a strengthen-part */
struct emblist_spart
{
    fun_emblist_spart_rm remover;   // the destroyer of strengthen-part
    void *spartcore;                // hold core datas for a strengthen-part
};

/* Extended head node for emblist */
struct emblist
{
    struct emblist_spart *spart;    // pointer of strengthen-part
    struct emblist_node head;       // an empty node use as a head node
    struct emblist_node trash;      // an empty node use as a head node
    int size;                       // real size of list
    int tsize;                      // real size of trashes
};

/* Hold datas for extended head node "struct emblist" */
extern struct emblist *
emblist_new();

/* Used to free "struct emblist" if there is no need to free the father
 *  struct of each node 
 */
extern void 
emblist_free(struct emblist *el);

/* Reverse all elements linked in the list @el */
extern void
emblist_reverse(struct emblist *el);

/* Clean all trashes in emblist(@el)
 * @type @member @destroyer refer to "emblist_node_free"
 */
#define emblist_clean(el, type, member, destroyer)                             \
    emblist_nodes_free(&((el)->trash), type, member, destroyer)
/* Free all memories malloced for saving all datas in the list
 * @type @member @destroyer refer to "emblist_node_free"
 */
#define emblist_destroy(el, type, member, destroyer)                           \
    do                                                                         \
    {                                                                          \
        emblist_spart_uninstall(el);                                           \
        emblist_nodes_free(&((el)->trash), type, member, destroyer);           \
        emblist_nodes_free(&((el)->head), type, member, destroyer);            \
        free(el);                                                              \
    }                                                                          \
    while (false)

/* Visit each node in the emblist(@el) using a loop cursor @pos */
#define emblist_foreach(pos, el)                                               \
    emblist_node_foreach(pos, &((el)->head))
/* @tpos is another loop cursor to use as temporary storage */
#define emblist_foreach_safe(pos, tpos, el)                                    \
    emblist_node_foreach_safe(pos, tpos, &((el)->head))

/* Find the pre-node of node @elnf by visiting each nodes in the list @el */
#define emblist_fd(el, elnf)                                                   \
    emblist_nodes_fd(&((el)->head), elnf)

/* Merge two list, refer to "emblist_nodes_merge" */
#define emblist_merge(el1, el2)                                                \
    do                                                                         \
    {                                                                          \
        emblist_nodes_merge(&((el1)->head), &((el2)->head));                   \
        (el1)->size += (el2)->size;                                            \
        (el2)->size = 0;                                                       \
    }                                                                          \
    while (false)

/* Add the giving node(@eln_new) after the node(@eln_pos) */
extern void 
emblist_add(struct emblist *el, struct emblist_node *eln_pos, 
        struct emblist_node *eln_new);

/* Add the giving node(@eln) at the head of all real nodes */
extern void 
emblist_add_head(struct emblist *el, struct emblist_node *eln);

/* Add all nodes linked after @elns to emblist(@el) */
extern void 
emblist_add_all(struct emblist *el, struct emblist_node *elns);

/* Add the node to list for saving trashes for @el */
extern void 
emblist_trash(struct emblist *el, struct emblist_node *eln);

/* Remove the node from list and move it to trash by giving pre-node */
extern void 
emblist_rm(struct emblist *el, struct emblist_node *eln_pre, 
        struct emblist_node *eln_del);

/* Find and remove the giving node(@eln), move it to trash */
extern void 
emblist_rm_fd(struct emblist *el, struct emblist_node *eln);

/* Save all pointers of nodes to array, refer to "emblist_nodes_toarray" */
#define emblist_toarray(elnpp, size, el)                                       \
    emblist_nodes_toarray(elnpp, size, &((el)->head))

/* Rebuild linked relation of emblist, refer to "embist_nodes_rebuild" */
#define emblist_rebuild(elnpp, gsize, el)                                      \
    do                                                                         \
    {                                                                          \
        emblist_nodes_rebuild(elnpp, gsize, &((el)->head));                    \
        (el)->size = gsize;                                                    \
    }                                                                          \
    while (false)

/* Heap sort emblist @el and reserved only @availnum nodes (from head) 
 *
 * Other nodes will be added to trash, please use "emblist_clean" to
 *  clean all trashes
 */
#define emblist_hsort_part_sp(el, type, member, availnum, comp)                \
    do                                                                         \
    {                                                                          \
        int lsize = (el)->size;                                                \
        int avail = availnum;                                                  \
        if (lsize < 2)                                                         \
            break;                                                             \
        if (avail > lsize)                                                     \
            avail = lsize;                                                     \
        void *vpa[lsize];                                                      \
        int i = 0;                                                             \
        struct emblist_node *elnp;                                             \
        emblist_foreach(elnp, (el))                                            \
            vpa[i++] = emblist_node_entry(elnp, type, member);                 \
        heap_sort(vpa, lsize, avail, comp);                                    \
        for (i = 0; i < lsize; i++)                                            \
            vpa[i] = &(((type *)vpa[i])->member);                              \
        emblist_rebuild((struct emblist_node **)vpa, avail, (el));             \
        for (i = avail; i < lsize; i++)                                        \
            emblist_trash(el, vpa[i]);                                         \
    }                                                                          \
    while (false)

/* Heap sort emblist @el */
#define emblist_hsort_sp(el, type, member, comp)                               \
    emblist_hsort_part_sp(el, type, member, (el)->size, comp)

/* Only reserve nodes according to @need[].
 * MAKE SURE array @need[] have elements more than @el->size.
 */
extern void 
emblist_shrink(struct emblist *el, bool *need);

/* Install a strengthen-part */
extern void 
emblist_spart_install(struct emblist *el, fun_emblist_spart_rm remover);

/* Unistall strenghthen-part if installed any */
extern void 
emblist_spart_uninstall(struct emblist *el);

/*---------------------------- part spart_hsort ------------------------------*/

/* Strengthen-part of heap sort */
struct emblist_spart_hsort
{
    struct emblist_node **sarray;   // sorted pointers satisfy property of heap
    int maxnumheap;                 // max number for the heap
    bool enable;                    // whether is enable -> @sarray is inited
    bool sorted;                    // whether is sorted already
};

#endif /* __EMBLIST_H__ */

