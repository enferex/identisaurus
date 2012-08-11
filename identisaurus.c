#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <gcc-plugin.h>
#include <coretypes.h>
#include <cgraph.h>
#include <diagnostic-core.h>
#include <ggc.h>
#include <gimple.h>
#include <output.h>
#include <tree.h>
#include <vec.h>


/* Rawr */
#define TAG "identisaurus"


/* Enable debugging (or pass -DDEBUG via gcc/make) */
#define DEBUG 1


#define _P(_desc, ...)                        \
do                                            \
{                                             \
    printf("["TAG"]" _desc": " __VA_ARGS__); \
    putc('\n', stdout);                       \
} while (0);


/* Print to stdout in the most sexxiest of ways */
#define P(...) _P("", __VA_ARGS__)


/* Print debug output */
#ifdef DEBUG
#define D(...) _P("[debug]", __VA_ARGS__)
#else
#define D(...)
#endif


/* Print errors via gcc */
#define E(...) error("["TAG"]: "__VA_ARGS__)


/* Rawr pronouced *rawr!!!* is the filename that the identisaurus emits.  It is
 * a 'hidden' file prefixed with a '.' and ending in '.rawr'.  This is a very
 * simple file and consists of comments prefixed with a '#' character.  Each
 * line is a comma separated value of:
 *     <object_file_name>, <build_count>
 * Where the 'build_count' is an integer value depecting the number of times
 * that the object file named 'object_file_name' has been built with gcc using
 * the identisaurus.
 *
 * Users can modify this file if they wish.  This can be useful if the user
 * wants to increase (or decrease?) the build count for a particular object
 * file.  Note that if the user does break chronological order and decreases the
 * build count, that is totally fine; identisaurus does not care.  This file
 * gets updated every build using the identisaurus plugin.  
 *
 * NOTE: Currently comments are not preserved between updates/builds using the
 * identisaurus.
 *
 * There once was an ancient beast, whose role was classifying object files, he
 * was only known as an identisaurus.  His bones have never been found, but his
 * legacy lives in all object files like fossils from a bygone era... *Rawr!*
 */
#define DEFAULT_RAWR_FILE ".identisaurus.rawr"


/* Comment character: For use in the count file (the ferocious .rawr file) */
#define COMMENT_CHAR '#'


/* Required for GCC plugins to work */
int plugin_is_GPL_compatible = 0;


/* Help info about the plugin if one were to use gcc's --version --help */
static struct plugin_info identisaurus_info =
{
    .version = "0.1",
    .help = "Identisaurus uniquely identifies object files by including a "
            "monotonically increasing build number.  Plans are on the table "
            "for also including optional code management identifiers "
            "(e.g. git hashes or svn revision numbers) into an object file.",
};


/* Simple structure defining what makes up the count file */
typedef unsigned long count_t;
typedef struct { const char *fname; count_t build_count; } build_count_t;
DEF_VEC_O(build_count_t);
DEF_VEC_ALLOC_O(build_count_t, gc);


static void add_build_count(
    VEC(build_count_t,gc) *counts,
    const char            *fname,
    count_t                build_count)
{
    build_count_t *entry;
        
    entry = VEC_safe_push(build_count_t, gc, counts, NULL);
    entry->fname = ggc_strdup(fname);
    entry->build_count = build_count;
}


/* Open and load the .rawr data */
static VEC(build_count_t,gc) *load_build_count_file(const char *fname)
{
    int lineno;
    char *c, *name, line[1024];
    FILE *fp;
    count_t build_count;
    VEC(build_count_t,gc) *counts;

    if (!(fp = fopen(fname, "a+")))
      E("Could not load file '%s' containing build counts.", fname);

    counts = VEC_alloc(build_count_t, gc, 1);
    lineno = 0;
    while (fgets(line, sizeof(line), fp))
    {
        ++lineno;

        /* Skip whitespace */
        c = line;
        while (ISSPACE(*c))
          ++c;

        /* Line starts with a comment... skip and go to next line */
        if (!*c || *c == COMMENT_CHAR)
          continue;

        name = strtok(c, ",");
        build_count = strtoll(strtok(NULL, ","), NULL, 10);
        if (!name || build_count < 0)
          E("Bad count file entry at line %d of count file '%s'",lineno,fname);
        add_build_count(counts, name, build_count);
    }

    fclose(fp);

#ifdef DEBUG
    unsigned i;
    build_count_t *count;
    FOR_EACH_VEC_ELT(build_count_t, counts, i, count)
      D("%s: %lu", count->fname, count->build_count)
#endif

    return counts;
}


/* Save the build count file (.rawr file) based on data in 'counts' */
static void save_build_count_file(
    const char            *fname,
    VEC(build_count_t,gc) *counts)
{
    FILE *fp;
    unsigned i;
    build_count_t *count;

    if (!(fp = fopen(fname, "w")))
      E("Could not open file '%s' to emit build counts.", fname);

    FOR_EACH_VEC_ELT(build_count_t, counts, i, count)
      fprintf(fp, "%s, %lu\n", count->fname, count->build_count);

    fclose(fp);
}


static void update_build_counts(
    VEC(build_count_t,gc) *counts,
    const char            *base_fname)
{
    unsigned i, updated;
    build_count_t *count;

    /* Update the data for this file, if it doesn't exist... add it! */
    updated = 0;
    FOR_EACH_VEC_ELT(build_count_t, counts, i, count)
    {
        if (strncmp(count->fname, base_fname, strlen(base_fname)) == 0)
        {
            ++count->build_count;
            D("Updating count for file '%s'", count->fname);
            updated = 1;
            break;
        }
    }

    /* If the data was not updated (we could not find it), add the file */
    if (!updated)
      add_build_count(counts, base_fname, 1);
}


/* Open/Create file for storing build numbers per object file */
static unsigned long get_build_number(
    VEC(build_count_t,gc) *counts,
    const char            *base_fname)
{
    unsigned i;
    build_count_t *count;

    FOR_EACH_VEC_ELT(build_count_t, counts, i, count)
      if (strncmp(count->fname, base_fname, strlen(base_fname)) == 0)
        return count->build_count;

    return 0;
}


static void insert_build_string(
    VEC(build_count_t,gc) *counts,
    const char            *base_fname)
{
    char *str;
    tree decl, lit;
    count_t count;
    
    count = get_build_number(counts, base_fname);
    asprintf(&str, "<"TAG">Build %lu at %lu</"TAG">", count, time(NULL));
    P("Inserting build string: '%s' into file '%s'", str, base_fname);

    /* Create a global string constant and have this baby plop into .ro */
    decl = build_decl(
        BUILTINS_LOCATION, VAR_DECL, get_identifier("__"TAG), ptr_type_node);
    lit = build_string_literal(strlen(str), str);
    DECL_ARTIFICIAL(decl) = 1;
    DECL_EXTERNAL(decl) = 0;
    DECL_PRESERVE_P(decl) = 1;
    DECL_INITIAL(decl) = lit;
    TREE_PUBLIC(decl) = 0;
    TREE_STATIC(decl) = 1;
    TREE_USED(decl) = 1;
    varpool_finalize_decl(decl);

    free(str);
}


/* Called once per function to analyze.  We look at each basic block and each
 * statement in that basic block and each operand in that statement.  The
 * operands might be constant strings, which we then spell check.
 */
static void identisaurus_exec(void *gcc_data, void *user_data)
{
    char *base_fname;
    VEC(build_count_t,gc) *counts;

    /* gcc does not provide us with access to the information defining the name
     * of the executable or object file being compiled.  So we lop-off file
     * extensions and just do our build-count matching on the base of the
     * filename.
     */
    base_fname = (char *)alloca(strlen(main_input_filename));
    strcpy(base_fname, main_input_filename);
    if (strrchr(base_fname, '.'))
      *strrchr(base_fname, '.') = '\0';

    counts = load_build_count_file(DEFAULT_RAWR_FILE);
    update_build_counts(counts, base_fname);
    insert_build_string(counts, base_fname);
    save_build_count_file(DEFAULT_RAWR_FILE, counts);
}


int plugin_init(struct plugin_name_args   *info, /* Argument info  */
                struct plugin_gcc_version *ver)  /* Version of GCC */
{
    register_callback(TAG, PLUGIN_INFO, NULL, &identisaurus_info);
    register_callback(
        TAG, PLUGIN_EARLY_GIMPLE_PASSES_START, identisaurus_exec, NULL);
    return 0;
}
