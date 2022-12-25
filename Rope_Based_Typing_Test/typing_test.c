#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

typedef struct rope_node
{
    char *data;
    int length;
    struct rope_node *left;
    struct rope_node *right;
} rope_node;

typedef struct rope
{
    rope_node *root;
} rope;

rope *rope_create()
{
    rope *r = malloc(sizeof(rope));
    r->root = NULL;
    return r;
}

rope_node *rope_node_create(const char *data, int length)
{
    rope_node *n = malloc(sizeof(rope_node));
    n->data = malloc(length + 1);
    strncpy(n->data, data, length);
    n->data[length] = '\0';
    n->length = length;
    n->left = NULL;
    n->right = NULL;
    return n;
}

void rope_node_destroy(rope_node *n)
{
    if (n == NULL)
        return;

    if (n->left != NULL)
    {
        rope_node_destroy(n->left);
    }
    if (n->right != NULL)
    {
        rope_node_destroy(n->right);
    }
    free(n->data);
    free(n);
}

void rope_destroy(rope *r)
{
    if (r == NULL)
        return;

    if (r->root != NULL)
    {
        rope_node_destroy(r->root);
    }
    free(r);
}

void rope_insert(rope *r, int index, const char *data)
{
    if (r == NULL || data == NULL)
        return;

    int length = strlen(data);
    if (length == 0)
        return;

    if (r->root == NULL)
    {
        r->root = rope_node_create(data, length);
        return;
    }

    int offset = 0;
    rope_node *n = r->root;
    while (n->left != NULL || n->right != NULL)
    {
        if (index < offset + n->length)
        {
            n = n->left;
        }
        else
        {
            offset += n->length;
            n = n->right;
        }
    }

    if (index < offset + n->length / 2)
    {
        rope_node *left = rope_node_create(data, length);
        rope_node *right = rope_node_create(n->data + (index - offset), n->length - (index - offset));
        free(n->data);
        n->data = NULL;
        n->left = left;
        n->right = right;
        n->length = index - offset;
    }
    else
    {
        rope_node *left = rope_node_create(n->data, index - offset);
        rope_node *right = rope_node_create(data, length);
        free(n->data);
        n->data = NULL;
        n->left = left;
        n->right = right;
        n->length = index - offset;
    }
}
int rope_node_length(rope_node *n)
{
    if (n == NULL)
        return 0;
    return n->length + rope_node_length(n->left) + rope_node_length(n->right);
}

int rope_length(rope *r)
{
    if (r == NULL || r->root == NULL)
        return 0;
    return rope_node_length(r->root);
}

void rope_node_to_string(rope_node *n, char *s)
{
    if (n == NULL)
        return;
    rope_node_to_string(n->left, s);
    strcat(s, n->data);
    rope_node_to_string(n->right, s);
}

char *rope_to_string(rope *r)
{
    if (r == NULL || r->root == NULL)
        return NULL;
    int length = rope_length(r);
    char *result = malloc(length + 1);
    rope_node_to_string(r->root, result);
    result[length] = '\0';
    return result;
}

int main(void)
{
    struct timeval start, end;
    double elapsed;

    rope *text = rope_create();
    rope_insert(text, 0, "\033[0;32mThe quick brown fox jumped over the lazy dog.\033[0m\n");
    char *text_string = rope_to_string(text);

    printf("Type the following text as quickly and accurately as possible:\n\n%s\n", text_string);

    gettimeofday(&start, NULL);
    int position = 0;
    int correct = 0;
    int incorrect = 0;
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (c == text_string[position])
        {
            correct++;
        }
        else
        {
            incorrect++;
        }
        position++;
    }

    gettimeofday(&end, NULL);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    int total = correct + incorrect;
    double speed = (double)total / elapsed;
    double accuracy = (double)correct / total;

    printf("\nResults:\n");
    printf("Time elapsed: %.2f seconds\n", elapsed);
    printf("Typing speed: %.2f characters per second\n", speed);
    printf("Accuracy: %.2f%%\n", accuracy * 100);

    rope_destroy(text);
    free(text_string);

    return 0;
}
