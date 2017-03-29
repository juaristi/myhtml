#include <stdio.h>
#include <stdlib.h>
#include <myhtml/api.h>

static void print_node(myhtml_tree_t *tree, myhtml_tree_node_t *node)
{
    myhtml_tag_id_t tid = myhtml_node_tag_id(node);
    const char *tag_name = myhtml_tag_name_by_id(tree, tid, NULL);
    printf("\tTag: %s\n", tag_name ? tag_name : "<NULL>");

    size_t len;
    const char *text = myhtml_node_text(node, &len);
    printf("\tText: %s\n", text ? text : "<NULL>");
}

static void print_token(myhtml_tree_t *tree, myhtml_token_node_t *token)
{
    myhtml_tag_id_t tid = myhtml_token_node_tag_id(token);
    const char *tag_name = myhtml_tag_name_by_id(tree, tid, NULL);
    printf("\tTag: %s\n", tag_name ? tag_name : "<not provided>");

    for (myhtml_tree_attr_t *attr = myhtml_token_node_attribute_first(token); attr; attr = myhtml_attribute_next(attr)) {
        size_t len;
        const char *attr_key = myhtml_attribute_key(attr, &len);
        const char *attr_value = myhtml_attribute_value(attr, &len);

        printf("\tAttr: %s = %s\n",
                attr_key ? attr_key : "<NULL>",
                attr_value ? attr_value : "<NULL>");
    }
}

static void *cb_token_before(myhtml_tree_t *tree, myhtml_token_node_t *token, void *ctx)
{
    printf("Token BEFORE\n");
    print_token(tree, token);
    return NULL;
}

static void *cb_token_after(myhtml_tree_t *tree, myhtml_token_node_t *token, void *ctx)
{
    printf("Token AFTER\n");
    print_token(tree, token);
    return NULL;
}

static void cb_node_insert(myhtml_tree_t *tree, myhtml_tree_node_t *node, void *ctx)
{
    printf("Node INSERT\n");
    print_node(tree, node);
}

static void cb_node_remove(myhtml_tree_t *tree, myhtml_tree_node_t *node, void *ctx)
{
    printf("Node REMOVE\n");
    print_node(tree, node);
}

int main(int argc, char **argv)
{
    char html[][64] = {
        "<!DOCT",
        "YPE htm",
        "l>",
        "<html><head>",
        "<ti",
        "tle>HTML chun",
        "ks parsing</",
        "title>",
        "</head><bod",
        "y><div cla",
        "ss=",
        "\"bestof",
        "class",
        "\">",
        "good for me",
        "</div>",
        "\0"
    };

    myhtml_t *mh = myhtml_create();
    myhtml_init(mh, MyHTML_OPTIONS_DEFAULT, 1, 0);

    myhtml_tree_t *t = myhtml_tree_create();
    myhtml_tree_init(t, mh);

    myhtml_encoding_set(t, MyENCODING_UTF_8);

//    myhtml_callback_before_token_done_set(t, cb_token_before, NULL);
//    myhtml_callback_after_token_done_set(t, cb_token_after, NULL);
    myhtml_callback_tree_node_insert_set(t, cb_node_insert, NULL);
    myhtml_callback_tree_node_remove_set(t, cb_node_remove, NULL);

    for (size_t i = 0; html[i][0]; i++) {
        printf("Parsing chunk: %s\n", html[i]);
        myhtml_parse_chunk(t, html[i], strlen(html[i]));
    }
    myhtml_parse_chunk_end(t);

    myhtml_tree_destroy(t);
    myhtml_destroy(mh);

    return 0;
}
