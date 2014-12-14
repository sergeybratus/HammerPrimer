HammerPrimer
============

Introduction to using the Hammer parser construction toolkit by Meredith L. Patterson

Lecture 1
=========

URL:

Abstract:

Files in lesson1/

Notes: (YMMV & gotchas)

Changes in Hammer since recording time: 
10:15 -- 

Lecture 2
=========





## Hammer parsing functions

Hammer is a parser combinator generation toolkit.

This entails creating functions (of type `HParser*`) which consume the input when combined or repeated.

Hammer has many methods of generating `HParser*`s. Here's a basic example:

```cpp
HParser *basic_parser = h_ch('a'); // This parser will take a single 'a' character.
```

Because this is something you'll type often hammer has a macro, `H_RULE`, which is demonstrated below.

```cpp
H_RULE(basic_parser, h_ch('a')); // equivalent to our basic_parser above
```

```cpp
h_ch_range lets you generate a parser for a character range, like all digits:
H_RULE(digit, h_ch_range(0x30, 0x39));
// or all lowercase letters
H_RULE(lowercase_letter, h_ch_range('a', 'z'));

// h_choice lets you can take one or another parser
H_RULE(whitespace, h_choice(tab_parser, newline_parser, space_parser));

// h_in lets you parse one character from an array (h_not_in checks a character does not exist in array)
H_RULE(vowel, h_in((uint8_t *)"AEIOU", 5));

// h_repeat_n lets you run a parser n times
H_RULE(three_consecutive_vowels, h_repeat_n(vowels, 3));

// h_optional lets you optionally match a character. Good for terminators, etc
H_RULE(optional_newline, h_optional(h_ch('\n')))

// h_many lets you run a parser at least one time
H_RULE(consecutive_letters, h_many(lowercase_letter));
// TODO h_many1 does what?

// h_sequence lets you run a series of parsers
H_RULE(words_separated_by_whitespace, h_sequence(
             consecutive_letters,   // has at least one lowercase letter
             h_optional(whitespace) // has an optional whitespace character
      ));

// h_whitespace lets you consume whitespace for provided parser
H_RULE(words_separated_by_whitespace, h_sequence(h_whitespace(consecutive_letters)));

// h_nothing_p lets you check that the input is null
H_RULE(emptiness, h_nothing_p()); // Will fail with presence of any data

// h_end_p lets you check the input has been fully consumed
H_RULE(end_marker, h_end_p());




// Some common parsers are included below

// All characters
H_RULE(letter, h_choice(h_ch_range('a','z'), h_ch_range('A','Z'), NULL));

// AUX.
HParser *base64_3 = h_repeat_n(bsfdig, 4);
HParser *base64_2 = h_sequence(bsfdig, bsfdig, bsfdig_4bit, equals, NULL);
HParser *base64_1 = h_sequence(bsfdig, bsfdig_2bit, equals, equals, NULL);
HParser *base64 = h_sequence(h_many(base64_3),
h_optional(h_choice(base64_2,
base64_1, NULL)),
NULL);

```


# Test c highlighting

```

```
