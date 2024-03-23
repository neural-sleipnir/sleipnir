# Naming Conventions

The naming convention was mainly inspired by the Vulkan API.

## General Naming Rules

Names of identifiers should generally be written with full words, as a concise description of what that identifier is.
For example, the type of structure containing information about how to create some struct info: `SpCreateStructInfo`.

Sometimes uses abbreviations and prefixes as long as they do not interfere with clarity.

## Preprocessor Defines

Preprocessor definitions include an underscore _ as a delimiter between words, with every character in upper case.

Each definition is prefixed with `SP_`, followed by the name.

Example: `SP_HEAP_SIZE`

## Type Names

Type names are declared with no separator between words. Each word starts with a capital letter,
and every other character in each word is lower case.

Each type name is prefixed with Vg. This rule applies to all types struct, union, enum.

Example:
```c
enum SpResult {
    ...
};

struct SpString {
    ...
};
```

## Enum Names

Enumerants include an underscore _ as a delimiter between words, with every character in upper case.

Each enumerant name is prefixed with `SP_`.

Value of each enumerant must be specified explicitly.

Example:

```c
enum SpResult {
    SP_SUCCESS = 0,
    ...
};
```

Types must be specified explicitly everywhere

Example:

```c
enum SpResult result;
struct SpString string;
```

## Function Names

Function names are declared with no separator between words. Each word starts with a capital letter, and every other
character in each word is lower case.

The structure of function declaration name should be as follows:

`prefix Verb Object`

* `prefix` - starts with `sp`
* `Verb` - describes action.
* `Object` - name of the object being acted upon by the command.

Example:

```c
enum SpResult spCreateStruct( ... );
```

## Command Verbs

Below is a list of many of the verbs currently in use in vigenere_decrypter:

| Verb     | Meaning                                                                              |
|----------|--------------------------------------------------------------------------------------|
| Allocate | Allocates memory heap and creates object - paired with C standard "free" function    |
| Create   | Creates an object - paired with "Destroy"                                            |
| Destroy  | Destroys an object - paired with "Create"                                            |
| Compute  | Computes an object                                                                   |
| Get      | Gets the state of an object or queries appropriate data of an object                 |
| Init     | Initializes object but do not allocate some memory, user have to pass allocated data |
| Convert  | Converts type to another data type                                                   |


## Standard Prefixes

Prefixes are used in the application to denote specific semantic meaning of vigenere_decrypter names

* SP/Sp/sp
  All types, commands, enumerants and C macro definitions in project are prefixed with these two characters,
  according to the rules defined above.
* p
  Pointer
  Variable is a pointer.

## Formatting


Google Style is used for formatting. `clang-format` is used to format the file.

Here you can see the project formatting rules:

```
Language:           Cpp
BasedOnStyle:       Google
```