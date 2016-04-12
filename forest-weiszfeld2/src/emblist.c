/*
 * =============================================================================
 *
 *       Filename:  emblist.c
 *
 *    Description:  implementation of emblist
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/emblist.h"

/* Let the node(@eln) form a list (Circular Linked List) itself */
inline void emblist_node_self(struct emblist_node *eln)
{
    eln->next = eln;
}

/* Add a node(@eln_new) after the giving node(@eln_pos) */
inline void emblist_node_add_after(struct emblist_node *eln_pos, struct emblist_node *eln_new)
{
    eln_new->next = eln_pos->next;
    eln_pos->next = eln_new;
}

/* Speeding method of removing a node(@eln_del) when giving the pre-node(@eln_pre) */
inline void emblist_node_rm_sp(struct emblist_node *eln_pre, struct emblist_node *eln_del)
{
    eln_pre->next = eln_del->next;
}

/* Speeding method of removing the node after the giving pre-node(@eln_pre) */
inline void emblist_node_rm_after_sp(struct emblist_node *eln_pre)
{
    eln_pre->next = eln_pre->next->next;
}

/* Find the pre-node of node @elnf by visiting start from node @elns */
inline struct emblist_node *emblist_nodes_fd(struct emblist_node *elns, struct emblist_node *elnf)
{
    struct emblist_node *last = elns;
    struct emblist_node *elnp;
    // at-least one node, and because the nodes formed a circular linked list
    emblist_node_foreach(elnp, elns)
    {
        if(elnp == elnf)
            return last;
        last = elnp;
    }
    if (elnp == elnf)
        return last;
    else
        return NULL;
}

/* Merge two list, linked all nodes after @eln2 to @eln1 but before original
 *  nodes which are linked after @eln1 :
 * @eln1 -> nodes after @eln2 -> nodes after @eln1 
 * @eln2 -> @eln2
 */
inline struct emblist_node *emblist_nodes_merge(struct emblist_node *eln1, struct emblist_node *eln2)
{
    struct emblist_node *eln2tail = emblist_nodes_fd(eln2, eln2); 
    eln2tail->next = eln1->next;
    eln1->next = eln2->next;
    emblist_node_self(eln2);
    return eln1;
}

/* Calculate the size of nodes linked after @elns exclude @elns */
inline int emblist_nodes_size_calc(struct emblist_node *elns)
{
    int size = 0;
    struct emblist_node *elnp;
    emblist_node_foreach(elnp, elns)
        size++;
    return size;
}

/* Save all pointers of nodes linked after and exclude @elns to @elnpp
 * MAKE SURE size is the same with "emblist_nodes_size_calc(elns)"
 */
inline void emblist_nodes_toarray(struct emblist_node **elnpp, int size, struct emblist_node *elns)
{
    int i = 0;
    struct emblist_node *elnp;
    // assert(size == emblist_nodes_size_calc(elns));
    emblist_node_foreach(elnp, elns)
        elnpp[i++] = elnp;
}

/* Rebuild linked relation of emblist using array @elnpp 
 * @size: size of array @elnpp
 * @head: head node of rebuild list, head will be elnpp[0] if it is NULL
 * @return: pointer of head node
 */
inline struct emblist_node *emblist_nodes_rebuild(struct emblist_node **elnpp, int size, struct emblist_node *head)
{
    int i = 0;
    struct emblist_node *ret, *elnp;
    if (head != NULL)
        ret = head;
    else if (size < 1)
        return NULL;
    else
        ret = elnpp[i++];
    elnp = ret;
    for (; i < size; i++)
    {
        elnp->next = elnpp[i];
        elnp = elnp->next;
    }
    elnp->next = ret;
    return ret;
}

/* Hold datas for extended head node "struct emblist" */
inline struct emblist *emblist_new()
{
    struct emblist *el = my_malloc_type(struct emblist);
    el->spart = NULL;
    emblist_node_self(&(el->head));
    emblist_node_self(&(el->trash));
    el->size = 0;
    el->tsize = 0;
    return el;
}

/* Used to free "struct emblist" if there is no need to free the father
 *  struct of each node 
 */
inline void emblist_free(struct emblist *el)
{
    emblist_spart_uninstall(el);
    free(el);
}

/* Reverse all elements linked in the list @el */
inline void emblist_reverse(struct emblist *el)
{
    struct emblist_node *eln, *elntmp, *elnlast;
    elnlast = &(el->head);
    emblist_foreach_safe(eln, elntmp, el)
    {
        emblist_node_next(eln) = elnlast;
        elnlast = eln;
    }
    emblist_node_next(&(el->head)) = elnlast;
}

/* Add the giving node(@eln_new) after the node(@eln_pos) */
inline void emblist_add(struct emblist *el, struct emblist_node *eln_pos, struct emblist_node *eln_new)
{
    emblist_node_add_after(eln_pos, eln_new);
    (el->size)++;
}

/* Add the giving node(@eln) at the head of all real nodes */
inline void emblist_add_head(struct emblist *el, struct emblist_node *eln)
{
    emblist_add(el, &(el->head), eln);
}

/* Add all nodes linked after @elns to emblist(@el) */
inline void emblist_add_all(struct emblist *el, struct emblist_node *elns)
{
    struct emblist_node *pos, *tpos;
    emblist_node_foreach_safe(pos, tpos, elns)
        emblist_add_head(el, pos);
}

/* Add the node to list for saving trashes for @el */
inline void emblist_trash(struct emblist *el, struct emblist_node *eln)
{
    emblist_node_add_after(&(el->trash), eln);
    (el->tsize)++;
}

/* Remove the node from list and move it to trash by giving pre-node */
inline void emblist_rm(struct emblist *el, struct emblist_node *eln_pre, struct emblist_node *eln_del)
{
    emblist_node_rm_sp(eln_pre, eln_del);
    emblist_trash(el, eln_del);
    (el->size)--;
}

/* Find and remove the giving node(@eln), move it to trash */
inline void emblist_rm_fd(struct emblist *el, struct emblist_node *eln)
{
    struct emblist_node *elnfpre = emblist_fd(el, eln);
    // if found this node's pre-node
    if (elnfpre != NULL && eln != &(el->head))
        emblist_rm(el, elnfpre, eln);
}

/* Only reserve nodes according to @need[].
 * MAKE SURE array @need[] have elements more than @el->size.
 */
inline void emblist_shrink(struct emblist *el, bool *need)
{
    int i = 0;
    el->size = 0;
    struct emblist_node *elnp, *elnptmp;
    struct emblist_node *last = &(el->head);
    emblist_foreach_safe(elnp, elnptmp, el)
    {
        if (need[i]) 
        {
            last->next = elnp;
            last = elnp;
            (el->size)++;
        }
        else
            emblist_trash(el, elnp);
        i++;
    }
    last->next = &(el->head);
}

/* Install a strengthen-part */
inline void emblist_spart_install(struct emblist *el, fun_emblist_spart_rm remover)
{
    // uninstall old spart
    emblist_spart_uninstall(el);
    // install new spart
    el->spart = my_malloc_type(struct emblist_spart);
    el->spart->remover = remover;
}

/* Unistall strenghthen-part if installed any */
inline void emblist_spart_uninstall(struct emblist *el)
{
    if (el->spart != NULL)
        (*el->spart->remover)(el->spart->spartcore);
    free(el->spart);
}

/* Install spart "heap sort" */
inline void emblist_spart_hsort_install(struct emblist *el, fun_emblist_spart_rm remover)
{
    emblist_spart_install(el, remover);
    el->spart->spartcore = my_malloc_type(struct emblist_spart_hsort);
    //TODO
}

