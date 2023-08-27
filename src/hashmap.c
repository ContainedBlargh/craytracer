typedef struct _CharPtrstruct json_array_sHashmapEntry
{
    CharPtr key;
    struct json_array_s value;
    struct CharPtrstruct json_array_sHashmapEntry *next;
} CharPtrstruct json_array_sHashmapEntry;
typedef struct _CharPtrstruct json_array_sHashmap
{
    CharPtrstruct json_array_sHashmapEntry *buckets;
    u32 size;
} CharPtrstruct json_array_sHashmap;
CharPtrstruct json_array_sHashmap *new_Kstruct json_array_sHashmap(u32 capacity)
{
    CharPtrstruct json_array_sHashmap *map = malloc(sizeof(CharPtrstruct json_array_sHashmap));
    if (map == ((void *)0))
    {
        do
        {
            fprintf((__acrt_iob_func(2)), "\x1b[31m");
            fprintf((__acrt_iob_func(2)), "Failed to allocate memory for hashmap\n");
            fprintf((__acrt_iob_func(2)), "\x1b[0m");
            trace_stack();
            exit(1);
        } while (0);
    }
    map->buckets = calloc(capacity, sizeof(CharPtrstruct json_array_sHashmapEntry *));
    return map;
}
void CharPtrstruct json_array_sHashmap_put(CharPtrstruct json_array_sHashmap *map, CharPtr key,
                                           struct json_array_s value)
{
    size_t index = strhash(key) % map->size;
    CharPtrstruct json_array_sHashmapEntry *newEntry = malloc(sizeof(CharPtrstruct json_array_sHashmapEntry));
    if (newEntry == ((void *)0))
    {
        do
        {
            fprintf((__acrt_iob_func(2)), "\x1b[31m");
            fprintf((__acrt_iob_func(2)), "Failed to allocate memory for hashmap entry\n");
            fprintf((__acrt_iob_func(2)), "\x1b[0m");
            trace_stack();
            exit(1);
        } while (0);
    }
    newEntry->key = key;
    newEntry->value = value;
    newEntry->next = map->buckets[index];
    map->buckets[index] = newEntry;
    map->size++;
}
struct json_array_s CharPtrstruct json_array_sHashmap_get(CharPtrstruct json_array_sHashmap *map, CharPtr key)
{
    size_t index = strhash(key) % HASHMAP_SIZE;
    CharPtrstruct json_array_sHashmapEntry *entry = map->buckets[index];
    while (entry != ((void *)0))
    {
        if (key_compare_T(entry->key, key) == 0)
        {
            return entry->value;
        }
        entry = entry->next;
    }
    do
    {
        fprintf((__acrt_iob_func(2)), "\x1b[31m");
        fprintf((__acrt_iob_func(2)), "Entry not found!\n");
        fprintf((__acrt_iob_func(2)), "\x1b[0m");
        trace_stack();
        exit(1);
    } while (0);
    return (struct json_array_s){0};
}